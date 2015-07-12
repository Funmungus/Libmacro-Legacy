/* signal/lnx/lnxdevice.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "signal/lnx/device.h"
# include "signal/isignal.h"

MCR_API mcr_Device mcr_keyDev ;
MCR_API mcr_Device mcr_absDev ;
MCR_API mcr_Device mcr_relDev ;
const MCR_API struct input_event mcr_syncer = { { 0, 0 }, EV_SYN, \
		SYN_REPORT, 0 } ;
MCR_API size_t mcr_abs_resolution = MCR_ABS_RESOLUTION ;

static mcr_Array _uinputPath ;
static mcr_Array _eventPath ;
static mtx_t _deviceLock ;

// Open uinput ( devPt->fd ) and write device to it.
static int device_open ( mcr_Device * devPt ) ;
// Open event device ( devPt->eventFd ).
static int device_open_event ( mcr_Device * devPt ) ;
// Destroy uinput device ( devPt->fd ) and close devPt->fd.
static int device_close ( mcr_Device * devPt ) ;
// Close devPt->eventFd.
static int device_close_event ( mcr_Device * devPt ) ;
// Open event device ( devPt->eventFd ) from current working directory.
static int device_open_event_wd ( mcr_Device * devPt ) ;
// Verify devPt->eventFd is correct for given device.
static int device_verify_event_device ( mcr_Device * devPt,
		char * nameBuffer, int isJoy ) ;
// Write event to device for all values in bits.
static int device_write_bits ( mcr_Device * devPt, int event, \
		mcr_Array * bits ) ;
// Write all UI_SET_EVBIT values for given device.
static int device_write_evbit ( mcr_Device * devPt ) ;
// Write all non-UI_SET_EVBIT values for given device.
static int device_write_non_evbit ( mcr_Device * devPt ) ;
// Write UI_DEV_CREATE after writing all evbits to device.
static int device_create ( mcr_Device * devPt ) ;

static int keyDevice_init ( ) ;
static int absDevice_init ( ) ;
static int relDevice_init ( ) ;

void mcr_Device_set_uinput_path ( const char * path )
{
	dassert ( path ) ;
	if ( path [ 0 ] == '\0' )
		return ;
	mcr_String_from_string ( & _uinputPath, path ) ;
}

void mcr_Device_set_event_directory
		( const char * directoryPath )
{
	dassert ( directoryPath ) ;
	if ( directoryPath [ 0 ] == '\0' )
		return ;
	mcr_String_from_string ( & _eventPath, directoryPath ) ;
}

void mcr_Device_set_absolute_resolution ( size_t resolution )
{
	mcr_abs_resolution = resolution ;
	int wasEnabled = 0 ;
	if ( mcr_absDev.enabled )
		wasEnabled = 1 ;
	mcr_Device_free ( & mcr_absDev ) ;
	absDevice_init ( ) ;
	if ( wasEnabled )
		mcr_Device_enable ( & mcr_absDev, 1 ) ;
	if ( mcr_relDev.enabled )
		wasEnabled = 1 ;
	mcr_Device_free ( & mcr_relDev ) ;
	relDevice_init ( ) ;
	if ( wasEnabled )
		mcr_Device_enable ( & mcr_relDev, 1 ) ;
}

void mcr_Device_init ( mcr_Device * devPt )
{
	dassert ( devPt ) ;
	memset ( devPt, 0, sizeof ( mcr_Device ) ) ;
	mcr_Map_init ( & devPt->type_value_map, sizeof ( int ),
			sizeof ( mcr_Array ) ) ;
	devPt->type_value_map.compare = mcr_int_compare ;
	devPt->fd = -1 ;
	devPt->event_fd = -1 ;
}

void mcr_Device_free ( mcr_Device * devPt )
{
	dassert ( devPt ) ;
	mcr_Device_enable ( devPt, 0 ) ;
	MCR_MAP_FOR_EACH_VALUE ( & devPt->type_value_map,
			mcr_Array_free_foreach, 0 ) ;
	mcr_Map_free ( & devPt->type_value_map ) ;
	mcr_Device_init ( devPt ) ;
}

int mcr_Device_enable ( mcr_Device * devPt, int enable )
{
	dassert ( devPt ) ;
	devPt->enabled = 0 ;
	device_close_event ( devPt ) ;
	// If already enabled, need to close and restart.
	if ( ! device_close ( devPt ) )
	{
		dmsg ;
		return 0 ;
	}

	if ( ! enable ) // disable complete
		return 1 ;
	// Cannot have device with no evbits to set.
	// Must have at least 1 UI_SET_EVBIT.
	if ( ! mcr_Device_has_evbit ( devPt ) )
	{
		dmsg ;
		return 0 ;
	}

	// Start by opening.
	mtx_lock ( & _deviceLock ) ;
	if ( ! device_open ( devPt ) )
	{
		dmsg ;
		mtx_unlock ( & _deviceLock ) ;
		return 0 ;
	}

	// Force evbit satisfaction.
	if ( ! device_write_evbit ( devPt ) )
	{
		dmsg ;
		mtx_unlock ( & _deviceLock ) ;
		return 0 ;
	}

	// Then write all other bits. Ensured at least one mapping pair.
	int valid = device_write_non_evbit ( devPt ) ;

	if ( ! device_create ( devPt ) )
	{
		dmsg ;
		mtx_unlock ( & _deviceLock ) ;
		return 0 ;
	}

	// Created and ready. valid true is all good, false means some
	// non-UI_SET_EVBIT did not work.
	devPt->enabled = 1 ;
	if ( ! device_open_event ( devPt ) )
	{
		dmsg ;
		valid = 0 ;
	}
	mtx_unlock ( & _deviceLock ) ;
	return valid ;
}

int mcr_Device_enable_all ( int enable )
{
	int success = mcr_Device_enable ( & mcr_keyDev, enable ) ;
	if ( ! mcr_Device_enable ( & mcr_absDev, enable ) )
		success = 0 ;
	if ( ! mcr_Device_enable ( & mcr_relDev, enable ) )
		return 0 ;
	return success ;
}

void mcr_Device_usage ( )
{
	fprintf ( mcr_stdout, "%s\n", "" ) ;
}

int mcr_Device_set_bits ( mcr_Device * devPt, int bitType, int * bits,
		size_t bitLen )
{
	dassert ( devPt ) ;
	dassert ( bits ) ;
	dassert ( bitLen != ( size_t ) -1 ) ;
	mcr_Array value ;
	mcr_Array_init ( & value, sizeof ( int ) ) ;
	// Unable to append array, free and exit.
	if ( ! mcr_Array_from_array ( & value, bits, bitLen ) )
	{
		dmsg ;
		mcr_Array_free ( & value ) ;
		return 0 ;
	}
	// Unable to insert into map, free and exit.
	if ( ! mcr_Map_map ( & devPt->type_value_map, & bitType, & value ) )
	{
		dmsg ;
		mcr_Array_free ( & value ) ;
		return 0 ;
	}
	// Map successful, the mapped item is allocated array,
	// so do not destroy it !
	return 1 ;
}

mcr_Array * mcr_Device_get_bits ( mcr_Device * devPt, int bitType )
{
	dassert ( devPt ) ;
	return MCR_MAP_GET_VALUE ( & devPt->type_value_map, & bitType ) ;
}

int mcr_Device_has_evbit ( mcr_Device * devPt )
{
	dassert ( devPt ) ;
	int evbit = UI_SET_EVBIT ;
	mcr_Array * evbit_arr = MCR_MAP_GET_VALUE ( & devPt->type_value_map,
			& evbit ) ;
	return evbit_arr && evbit_arr->used ;
}

static int device_open ( mcr_Device * devPt )
{
	dassert ( devPt ) ;
	// Close in case previously opened.
	if ( ! device_close ( devPt ) )
	{
		dmsg ;
		return 0 ;
	}
	devPt->fd = open ( _uinputPath.array,
			O_RDWR | O_SYNC ) ;

	if ( devPt->fd == -1 )
	{
		dmsg ;
		return 0 ;
	}

	if ( write ( devPt->fd, & devPt->device, sizeof ( devPt->device ) ) !=
			sizeof ( devPt->device ) )
	{
		dmsg ;
		device_close ( devPt ) ;
		return 0 ;
	}

	return 1 ;
}

static int device_open_event ( mcr_Device * devPt )
{
	dassert ( devPt ) ;
	if ( ! device_close_event ( devPt ) )
	{
		dmsg ;
		return 0 ;
	}
	long int size ;
	size = pathconf ( ".", _PC_PATH_MAX ) ;
	if ( size == -1 )
	{
		dmsg ;
		return 0 ;
	}
	char * wd = malloc ( size ) ;
	char * ptr ;
	if ( ! wd )
	{
		dmsg ;
		return 0 ;
	}
	// Get the current working directory to return to later.
	ptr = getcwd ( wd, size ) ;
	UNUSED ( ptr ) ;
	if ( ! wd || chdir ( _eventPath.array ) == -1 )
	{
		dmsg ;
		free ( wd ) ;
		return 0 ;
	}

	if ( ! device_open_event_wd ( devPt ) ||
		chdir ( wd ) == -1 )
	{
		dmsg ;
		free ( wd ) ;
		return 0 ;
	}
	free ( wd ) ;
	return 1 ;
}

static int device_close ( mcr_Device * devPt )
{
	dassert ( devPt ) ;
	if ( devPt->fd != -1 )
	{
		if ( ioctl ( devPt->fd, UI_DEV_DESTROY ) == -1
				|| close ( devPt->fd ) == -1 )
		{
			dmsg ;
			return 0 ;
		}
		else
			devPt->fd = -1 ;
	}
	return 1 ;
}

static int device_close_event ( mcr_Device * devPt )
{
	dassert ( devPt ) ;
	if ( devPt->event_fd != -1 )
	{
		if ( close ( devPt->event_fd ) == -1 )
		{
			dmsg ;
			return 0 ;
		}
		else
			devPt->event_fd = -1 ;
	}
	return 1 ;
}

static int device_open_event_wd ( mcr_Device * devPt )
{
	dassert ( devPt ) ;
	DIR * dirp = opendir ( "." ) ;
	if ( dirp == NULL )
	{
		dmsg ;
		return 0 ;
	}

	char dev_name [ UINPUT_MAX_NAME_SIZE ] ;
	memset ( dev_name, 0, sizeof ( dev_name ) ) ;
	struct stat s ;
	int ret = 0 ;
	for ( struct dirent * entry = readdir ( dirp ) ;
			entry != NULL ; entry = readdir ( dirp ) )
	{	// if ! entry then end of directory
		if ( stat ( entry->d_name, & s ) == -1 )
		{
			dmsg ;
			continue ;
		}
		// Our uinput devices are always char devices.
		if ( S_ISCHR ( s.st_mode ) )
		{
			devPt->event_fd = open ( entry->d_name, O_RDONLY ) ;
			if ( devPt->event_fd == -1 )
				continue ;

			ret = ! strncasecmp ( entry->d_name, "js", 2 ) ||
					! strncasecmp ( entry->d_name, "joystick", 8 ) ;
			ret = device_verify_event_device ( devPt, dev_name, ret ) ;
			// Found correct fd for this uinput device
			if ( ret )
				break ;
			if ( ! device_close_event ( devPt ) )
			{
				dmsg ;
				break ;
			}
		}
	}
	closedir ( dirp ) ;
	return ret ;
}

static int device_verify_event_device ( mcr_Device * devPt,
		char * nameBuffer, int isJoy )
{
	dassert ( devPt ) ;
	int comp = 0 ;
	if ( isJoy )
		// joysticks have different ioctl for names.
	{
		comp = ioctl ( devPt->event_fd, JSIOCGNAME ( UINPUT_MAX_NAME_SIZE ),
				nameBuffer ) ;
	}
	else
	{
		comp = ioctl ( devPt->event_fd, EVIOCGNAME ( UINPUT_MAX_NAME_SIZE ),
				nameBuffer ) ;
	}
	// Could not retrieve name, unknown error.
	if ( comp == -1 )
	{
		dmsg ;
		// panic
		if ( ! device_close_event ( devPt ) )
		{
			dmsg ;
		}
		return 0 ;
	}
	return ! strncmp ( nameBuffer, devPt->device.name,
			UINPUT_MAX_NAME_SIZE ) ;
}

// bitPt is iterator through array
static void write_bit_redirect ( int * bitPt, int fd,
		int setBit, int * success )
{
	dassert ( bitPt ) ;
	dassert ( success ) ;
	dassert ( fd != -1 ) ;
	if ( ioctl ( fd, setBit, * bitPt ) == -1 )
	{
		dmsg ;
		 * success = 0 ;
	}
}

static int device_write_bits ( mcr_Device * devPt, int setBit,
		mcr_Array * bits )
{
	dassert ( devPt ) ;
	dassert ( bits ) ;
	if ( ! bits->used )
		return 1 ;
	int success = 1 ;
	MCR_ARR_FOR_EACH ( bits, write_bit_redirect, devPt->fd,
			setBit, & success ) ;
	return success ;
}

static int device_write_evbit ( mcr_Device * devPt )
{
	dassert ( devPt ) ;
	int evbit = UI_SET_EVBIT ;
	mcr_Array * evbit_arr = MCR_MAP_GET_VALUE ( & devPt->type_value_map,
			& evbit ) ;
	return evbit_arr && evbit_arr->used &&
			device_write_bits ( devPt, evbit, evbit_arr ) ;
}

static void write_non_evbit_redirect ( int * evbitPair,
		mcr_Device * devPt, int * success )
{
	dassert ( evbitPair ) ;
	dassert ( devPt ) ;
	dassert ( success ) ;
	if ( * evbitPair == UI_SET_EVBIT ) return ;
	mcr_Array * evbitArr = MCR_MAP_VALUE ( & devPt->type_value_map,
			evbitPair ) ;
	if ( ! device_write_bits ( devPt, * evbitPair, evbitArr ) )
	{
		dmsg ;
		 * success = 0 ;
	}
}

static int device_write_non_evbit ( mcr_Device * devPt )
{
	dassert ( devPt ) ;
	// All non-evbits.
	if ( ! devPt->type_value_map.set.used )
		return 0 ;
	int success = 1 ;
	MCR_MAP_FOR_EACH ( & devPt->type_value_map, write_non_evbit_redirect,
			devPt, & success ) ;
	return success ;
}

static int device_create ( mcr_Device * devPt )
{
	dassert ( devPt ) ;
	if ( ioctl ( devPt->fd, UI_DEV_CREATE ) < 0 )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

static int keyDevice_init ( )
{
	mcr_Device_init ( & mcr_keyDev ) ;

	snprintf ( mcr_keyDev.device.name, UINPUT_MAX_NAME_SIZE,
			"macrolibrary-key" ) ;

	mcr_keyDev.device.id.bustype = BUS_VIRTUAL ;
	mcr_keyDev.device.id.vendor = 1 ;
	mcr_keyDev.device.id.product = 1 ;
	mcr_keyDev.device.id.version = 1 ;

	int evbits [ ] = { EV_KEY, EV_MSC, EV_SYN } ;
	if ( ! mcr_Device_set_bits ( & mcr_keyDev, UI_SET_EVBIT, evbits, 3 ) )
	{
		dmsg ;
		return 0 ;
	}
	evbits [ 0 ] = MSC_SCAN ;
	if ( ! mcr_Device_set_bits ( & mcr_keyDev, UI_SET_MSCBIT, evbits, 1 ) )
	{
		dmsg ;
		return 0 ;
	}
	int keybits [ KEY_CNT ] ;
	for ( int i = 0 ; i < KEY_CNT ; i++ )
	{
		keybits [ i ] = i ;
	}

	// Do not write 0 value
	if ( ! mcr_Device_set_bits ( & mcr_keyDev, UI_SET_KEYBIT,
		 	keybits + 1, KEY_CNT - 1 ) )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

static int absDevice_init ( )
{
	mcr_Device_init ( & mcr_absDev ) ;

	snprintf ( mcr_absDev.device.name, UINPUT_MAX_NAME_SIZE,
			"macrolibrary-abs" ) ;

	mcr_absDev.device.id.bustype = BUS_VIRTUAL ;
	mcr_absDev.device.id.vendor = 1 ;
	mcr_absDev.device.id.product = 1 ;
	mcr_absDev.device.id.version = 1 ;

	for ( int i = 0 ; i < ABS_MISC ; i++ )
	{
		mcr_absDev.device.absmax [ i ] = mcr_abs_resolution ;
	}

	int evbits [ ] = { EV_ABS, EV_KEY, EV_SYN } ;
	if ( ! mcr_Device_set_bits ( & mcr_absDev, UI_SET_EVBIT, evbits, 3 ) )
	{
		dmsg ;
		return 0 ;
	}
	evbits [ 0 ] = BTN_LEFT ;
	if ( ! mcr_Device_set_bits ( & mcr_absDev, UI_SET_KEYBIT, evbits, 1 ) )
	{
		dmsg ;
		return 0 ;
	}
	int absbits [ ABS_MISC ] ;
	for ( int i = 0 ; i < ABS_MISC ; i++ )
	{
		absbits [ i ] = i ;
	}

	if ( ! mcr_Device_set_bits ( & mcr_absDev, UI_SET_ABSBIT, absbits,
			ABS_MISC ) )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

static int relDevice_init ( )
{
	mcr_Device_init ( & mcr_relDev ) ;

	snprintf ( mcr_relDev.device.name, UINPUT_MAX_NAME_SIZE,
			"macrolibrary-rel" ) ;

	mcr_relDev.device.id.bustype = BUS_VIRTUAL ;
	mcr_relDev.device.id.vendor = 1 ;
	mcr_relDev.device.id.product = 1 ;
	mcr_relDev.device.id.version = 1 ;

	for ( int i = 0 ; i < REL_MISC ; i++ )
	{
		mcr_relDev.device.absmax [ i ] = mcr_abs_resolution ;
	}

	int evbits [ ] = { EV_REL, EV_KEY, EV_SYN } ;
	if ( ! mcr_Device_set_bits ( & mcr_relDev, UI_SET_EVBIT, evbits, 3 ) )
	{
		dmsg ;
		return 0 ;
	}
	evbits [ 0 ] = BTN_LEFT ;
	if ( ! mcr_Device_set_bits ( & mcr_relDev, UI_SET_KEYBIT, evbits, 1 ) )
	{
		dmsg ;
		return 0 ;
	}
	int relbits [ REL_MISC ] ;
	for ( int i = 0 ; i < REL_MISC ; i++ )
	{
		relbits [ i ] = i ;
	}

	if ( ! mcr_Device_set_bits ( & mcr_relDev, UI_SET_RELBIT, relbits,
			REL_MISC ) )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

void mcr_Device_initialize ( )
{
	mtx_init ( & _deviceLock, mtx_plain ) ;
	mcr_String_init ( & _uinputPath ) ;
	mcr_String_init ( & _eventPath ) ;
	mcr_String_from_string ( & _uinputPath, STRINGIFY ( MCR_UINPUT_PATH ) ) ;
	mcr_String_from_string ( & _eventPath, STRINGIFY ( MCR_EVENT_PATH ) ) ;
	/* ioctl is unpredictable for valgrind. The first ioctl will
	 * be uninitialized, and others should not cause errors. */
	keyDevice_init ( ) ;
	absDevice_init ( ) ;
	relDevice_init ( ) ;
}

void mcr_Device_cleanup ( )
{
	mcr_Device_free ( & mcr_keyDev ) ;
	mcr_Device_free ( & mcr_absDev ) ;
	mcr_Device_free ( & mcr_relDev ) ;
	mcr_Array_free ( & _uinputPath ) ;
	mcr_Array_free ( & _eventPath ) ;
	mtx_destroy ( & _deviceLock ) ;
}
