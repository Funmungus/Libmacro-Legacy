
# include "signal/lnx/device.h"
# include "signal/isignal.h"

static mtx_t _deviceLock ;

MCR_API mcr_Device mcr_KeyDev ;
MCR_API mcr_Device mcr_AbsDev ;
MCR_API mcr_Device mcr_RelDev ;

const MCR_API struct input_event mcr_Syncer = { { 0, 0 }, EV_SYN, \
		SYN_REPORT, 0 } ;

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

void mcr_Device_init ( mcr_Device * devPt )
{
	memset ( devPt, 0, sizeof ( mcr_Device ) ) ;
	mcr_Map_init ( & devPt->type_value_map, sizeof ( int ), \
			sizeof ( mcr_Array ) ) ;
	devPt->type_value_map.compare = mcr_int_compare ;
	devPt->fd = -1 ;
	devPt->event_fd = -1 ;
}
void mcr_Device_free ( mcr_Device * devPt )
{
	device_close_event ( devPt ) ;
	device_close ( devPt ) ;
	if ( devPt->type_value_map.set.used > 0 )
	{
		void * arr = devPt->type_value_map.set.array ;
		// iterate mapped arrays
		arr = MCR_MAP_VALUE ( & devPt->type_value_map, arr ) ;
		// Free allocated dynamic arrays.
		// One past last.
		void * end = MCR_ARR_END ( & devPt->type_value_map.set ) ;
		for ( ; arr < end ;
				arr = MCR_ARR_NEXT ( & devPt->type_value_map.set, arr ) )
		{
			mcr_Array_free ( arr ) ;
		}
	}
	mcr_Map_free ( & devPt->type_value_map ) ;
	mcr_Device_init ( devPt ) ;
}

int mcr_Device_enable ( mcr_Device * devPt, int enable )
{
	// If already enabled, need to close and restart.
	if ( ! device_close ( devPt ) )
	{
		DMSG ( "%s\n", "Could not disable old device." ) ;
		return 0 ;
	}
	devPt->enabled = 0 ;

	if ( ! enable ) // disable complete
		return 1 ;
	// Cannot have device with no evbits to set.
	// Must have at least 1 UI_SET_EVBIT.
	if ( ! mcr_Device_has_evbit ( devPt ) )
	{
		DMSG ( "%s\n", "No UI_SET_EVBIT available." ) ;
		return 0 ;
	}

	// Start by opening.
	mtx_lock ( & _deviceLock ) ;
	if ( ! device_open ( devPt ) )
	{
		DMSG ( "%s\n", "Cannot open." ) ;
		mtx_unlock ( & _deviceLock ) ;
		return 0 ;
	}

	// Force evbit satisfaction.
	if ( ! device_write_evbit ( devPt ) )
	{
		DMSG ( "%s\n", "Cannot write evbit." ) ;
		mtx_unlock ( & _deviceLock ) ;
		return 0 ;
	}

	// Then write all other bits. Ensured at least one mapping pair.
	int valid = device_write_non_evbit ( devPt ) ;

	if ( ! device_create ( devPt ) )
	{
		DMSG ( "%s\n", "Cannot create device." ) ;
		mtx_unlock ( & _deviceLock ) ;
		return 0 ;
	}

	// Created and ready. valid true is all good, false means some
	// non-UI_SET_EVBIT did not work.
	devPt->enabled = 1 ;
	if ( ! device_open_event ( devPt ) )
	{
		DMSG ( "%s\n", "Cannot find device event file." ) ;
		valid = 0 ;
	}
	mtx_unlock ( & _deviceLock ) ;
	return valid ;
}

void mcr_Device_usage ( )
{
	printf ( "%s\n", "" ) ;
}

int mcr_Device_set_bits ( mcr_Device * devPt, int bitType, int * bits,
		size_t bitLen )
{
	mcr_Array value ;
	mcr_Array_init ( & value, sizeof ( int ) ) ;
	// Unable to append array, free and exit.
	if ( ! mcr_Array_from_array ( & value, bits, bitLen ) )
	{
		DMSG ( "%s\n", "set_bits." ) ;
		mcr_Array_free ( & value ) ;
		return 0 ;
	}
	// Unable to insert into map, free and exit.
	if ( ! mcr_Map_map ( & devPt->type_value_map, & bitType, & value ) )
	{
		DMSG ( "%s\n", "set_bits." ) ;
		mcr_Array_free ( & value ) ;
		return 0 ;
	}
	// Map successful, the mapped item is allocated array,
	// so do not destroy it !
	return 1 ;
}

mcr_Array * mcr_Device_get_bits ( mcr_Device * devPt, int bitType )
{
	void * found = MCR_MAP_GET ( & devPt->type_value_map, & bitType ) ;
	if ( found == NULL )
		return NULL ;
	return MCR_MAP_VALUE ( & devPt->type_value_map, found ) ;
}

int mcr_Device_has_evbit ( mcr_Device * devPt )
{
	int evbit = UI_SET_EVBIT ;
	void * evbit_pair = MCR_MAP_GET ( & devPt->type_value_map, & evbit ) ;
	mcr_Array * evbit_arr = MCR_MAP_VALUE (
			& devPt->type_value_map, evbit_pair ) ;
	if ( evbit_pair == NULL || evbit_arr->used == 0 )
	{
		return 0 ;
	}
	return 1 ;
}

static int device_open ( mcr_Device * devPt )
{
	// Close in case previously opened.
	if ( ! device_close ( devPt ) )
	{
		DMSG ( "%s\n", "Cannot close previous udev file." ) ;
		return 0 ;
	}
	devPt->fd = open ( STRINGIFY ( MCR_UINPUT_PATH ), O_RDWR | O_SYNC ) ;

	if ( devPt->fd == -1 )
	{
		DMSG ( "%s\n, ", "Error opening uinput." ) ;
		return 0 ;
	}

	if ( write ( devPt->fd, & devPt->device, sizeof ( devPt->device ) ) !=
			sizeof ( devPt->device ) )
	{
		DMSG ( "%s\n, ", "Error writing user device" ) ;
		device_close ( devPt ) ;
		return 0 ;
	}

	return 1 ;
}

static int device_open_event ( mcr_Device * devPt )
{
	if ( ! device_close_event ( devPt ) )
	{
		DMSG ( "%s\n", "Cannot close previous event file." ) ;
		return 0 ;
	}
	long int size ;
	size = pathconf ( ".", _PC_PATH_MAX ) ;
	if ( size == -1 )
	{
		DMSG ( "%s\n", "Cannot determine current dir length." ) ;
		return 0 ;
	}
	char * wd = malloc ( size ) ;
	char * ptr ;
	if ( ! wd )
	{
		DMSG ( "%s\n", "open_event" ) ;
		return 0 ;
	}
	ptr = getcwd ( wd, size ) ;
	UNUSED ( ptr ) ;
	if ( ! wd || chdir ( STRINGIFY ( MCR_EVENT_PATH ) ) == -1 )
	{
		DMSG ( "%s\n", "open_event cd" ) ;
		free ( wd ) ;
		return 0 ;
	}

	if ( ! device_open_event_wd ( devPt ) ||
		chdir ( wd ) == -1 )
	{
		DMSG ( "%s\n", "open_event" ) ;
		free ( wd ) ;
		return 0 ;
	}
	free ( wd ) ;
	return 1 ;
}

static int device_close ( mcr_Device * devPt )
{
	if ( devPt->fd != -1 )
	{
		if ( ioctl ( devPt->fd, UI_DEV_DESTROY ) == -1
				|| close ( devPt->fd ) == -1 )
		{
			DMSG ( "%s\n", "close" ) ;
			return 0 ;
		}
		else
			devPt->fd = -1 ;
	}
	return 1 ;
}

static int device_close_event ( mcr_Device * devPt )
{
	if ( devPt->event_fd != -1 )
	{
		if ( close ( devPt->event_fd ) == -1 )
		{
			DMSG ( "%s\n", "close" ) ;
			return 0 ;
		}
		else
			devPt->event_fd = -1 ;
	}
	return 1 ;
}

static int device_open_event_wd ( mcr_Device * devPt )
{
	DIR * dirp = opendir ( "." ) ;
	if ( dirp == NULL )
	{
		DMSG ( "%s\n", "Error open_event_wd" ) ;
		return 0 ;
	}

	char dev_name [ UINPUT_MAX_NAME_SIZE ] ;
	dev_name [ 0 ] = '\0' ;
	struct stat s ;
	int ret = 0 ;
	for ( struct dirent * entry = readdir ( dirp ) ;
			entry != NULL ; entry = readdir ( dirp ) )
	{	// if ! entry then end of directory
		if ( stat ( entry->d_name, & s ) == -1 )
		{
			DMSG ( "%s\n", "Cannot stat." ) ;
			continue ;
		}
		if ( S_ISCHR ( s.st_mode ) )
		{	// if entry is a char device
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
				DMSG ( "%s\n", "Error closing incorrect device." ) ;
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
		DMSG ( "%s\n", "Read name unknown error." ) ;
		// panic
		if ( ! device_close_event ( devPt ) )
		{
			DMSG ( "%s\n", "Error closing incorrect device." ) ;
		}
		return 0 ;
	}
	return ! strncmp ( nameBuffer, devPt->device.name,
			UINPUT_MAX_NAME_SIZE ) ;
}

static int device_write_bits ( mcr_Device * devPt, int setBit,
		mcr_Array * bits )
{
	if ( bits->used == 0 )
		return 1 ;
	int success = 1 ;

	int * end = MCR_ARR_END ( bits ) ; // one past last
	for ( int * i = ( int * ) bits->array ; i < end ; i++ )
	{
		if ( ioctl ( devPt->fd, setBit, * i ) == -1 )
		{
			DMSG ( "%s%d\n", "error set bit, type ", setBit ) ;
			success = 0 ;
		}
	}

	return success ;
}

static int device_write_evbit ( mcr_Device * devPt )
{
	int evbit = UI_SET_EVBIT ;
	void * evbit_pair = MCR_MAP_GET ( & devPt->type_value_map, & evbit ) ;
	mcr_Array * evbit_arr = MCR_MAP_VALUE (
			& devPt->type_value_map, evbit_pair ) ;
	if ( evbit_pair == NULL || evbit_arr->used == 0 )
	{
		return 0 ;
	}
	return device_write_bits ( devPt, evbit, evbit_arr ) ;
}

static int device_write_non_evbit ( mcr_Device * devPt )
{
	// All non-evbits.
	int * evbit_pair = ( int * ) devPt->type_value_map.set.array ;
	mcr_Array * evbit_arr = MCR_MAP_VALUE ( & devPt->type_value_map,
			evbit_pair ) ;
	if ( evbit_pair == NULL || evbit_arr->used == 0 )
		return 0 ;
	int * end = MCR_ARR_END ( & devPt->type_value_map.set ) ;
	int valid = 1 ;
	while ( evbit_pair < end )
	{
		if ( * evbit_pair != UI_SET_EVBIT )
		{
			valid = device_write_bits ( devPt, * evbit_pair, evbit_arr ) ?
					valid : 0 ;
		}
		evbit_pair = MCR_ARR_NEXT ( & devPt->type_value_map.set,
				evbit_pair ) ;
		evbit_arr = MCR_ARR_NEXT ( & devPt->type_value_map.set,
				evbit_arr ) ;
	}
	return valid ;
}

static int device_create ( mcr_Device * devPt )
{
	if ( ioctl ( devPt->fd, UI_DEV_CREATE ) < 0 )
	{
		DMSG ( "%s%d\n", "error UI_DEV_CREATE, fd ", devPt->fd ) ;
		return 0 ;
	}

	// Device is not recognized for i/o until time has passed.
	// 1 milli = 1000000 nano
	//struct timespec time_point = { 0, 1000000 } ;
	//thrd_sleep ( & time_point, NULL ) ;
	return 1 ;
}

static int keyDevice_init ( )
{
	mcr_Device_init ( & mcr_KeyDev ) ;

	snprintf ( mcr_KeyDev.device.name, UINPUT_MAX_NAME_SIZE,
			"macro-key" ) ;

	mcr_KeyDev.device.id.bustype = BUS_USB ;
	mcr_KeyDev.device.id.vendor = 1 ;
	mcr_KeyDev.device.id.product = 1 ;
	mcr_KeyDev.device.id.version = 1 ;

	int evbits [ ] = { EV_KEY, EV_MSC, EV_SYN } ;
	if ( ! mcr_Device_set_bits ( & mcr_KeyDev, UI_SET_EVBIT, evbits, 3 ) )
	{
		DMSG ( "%s\n", "key init" ) ;
		return 0 ;
	}
	evbits [ 0 ] = MSC_SCAN ;
	if ( ! mcr_Device_set_bits ( & mcr_KeyDev, UI_SET_MSCBIT, evbits, 1 ) )
	{
		DMSG ( "%s\n", "key init" ) ;
		return 0 ;
	}
	int keybits [ KEY_CNT ] ;
	for ( int i = 0 ; i < KEY_CNT ; i++ )
	{
		keybits [ i ] = i ;
	}

	if ( ! mcr_Device_set_bits ( & mcr_KeyDev, UI_SET_KEYBIT, keybits,
			KEY_CNT ) )
	{
		DMSG ( "%s\n", "key init" ) ;
		return 0 ;
	}
	return mcr_Device_enable ( & mcr_KeyDev, 1 ) ;
}

static int absDevice_init ( )
{
	mcr_Device_init ( & mcr_AbsDev ) ;

	snprintf ( mcr_AbsDev.device.name, UINPUT_MAX_NAME_SIZE,
			"macro-abs" ) ;

	mcr_AbsDev.device.id.bustype = BUS_USB ;
	mcr_AbsDev.device.id.vendor = 1 ;
	mcr_AbsDev.device.id.product = 1 ;
	mcr_AbsDev.device.id.version = 1 ;

	for ( int i = 0 ; i < ABS_MISC ; i++ )
	{
		mcr_AbsDev.device.absmax [ i ] = MCR_ABS_RESOLUTION ;
	}

	int evbits [ ] = { EV_ABS, EV_KEY, EV_SYN } ;
	if ( ! mcr_Device_set_bits ( & mcr_AbsDev, UI_SET_EVBIT, evbits, 3 ) )
	{
		DMSG ( "%s\n", "abs init" ) ;
		return 0 ;
	}
	evbits [ 0 ] = BTN_LEFT ;
	if ( ! mcr_Device_set_bits ( & mcr_AbsDev, UI_SET_KEYBIT, evbits, 1 ) )
	{
		DMSG ( "%s\n", "abs init" ) ;
		return 0 ;
	}
	int absbits [ ABS_MISC ] ;
	for ( int i = 0 ; i < ABS_MISC ; i++ )
	{
		absbits [ i ] = i ;
	}

	if ( ! mcr_Device_set_bits ( & mcr_AbsDev, UI_SET_ABSBIT, absbits,
			ABS_MISC ) )
	{
		DMSG ( "%s\n", "abs init" ) ;
		return 0 ;
	}
	return mcr_Device_enable ( & mcr_AbsDev, 1 ) ;
}

static int relDevice_init ( )
{
	mcr_Device_init ( & mcr_RelDev ) ;

	snprintf ( mcr_RelDev.device.name, UINPUT_MAX_NAME_SIZE,
			"macro-rel" ) ;

	mcr_RelDev.device.id.bustype = BUS_USB ;
	mcr_RelDev.device.id.vendor = 1 ;
	mcr_RelDev.device.id.product = 1 ;
	mcr_RelDev.device.id.version = 1 ;

	for ( int i = 0 ; i < REL_MISC ; i++ )
	{
		mcr_RelDev.device.absmax [ i ] = MCR_ABS_RESOLUTION ;
	}

	int evbits [ ] = { EV_REL, EV_KEY, EV_SYN } ;
	if ( ! mcr_Device_set_bits ( & mcr_RelDev, UI_SET_EVBIT, evbits, 3 ) )
	{
		DMSG ( "%s\n", "abs init" ) ;
		return 0 ;
	}
	evbits [ 0 ] = BTN_LEFT ;
	if ( ! mcr_Device_set_bits ( & mcr_RelDev, UI_SET_KEYBIT, evbits, 1 ) )
	{
		DMSG ( "%s\n", "abs init" ) ;
		return 0 ;
	}
	int relbits [ REL_MISC ] ;
	for ( int i = 0 ; i < REL_MISC ; i++ )
	{
		relbits [ i ] = i ;
	}

	if ( ! mcr_Device_set_bits ( & mcr_RelDev, UI_SET_RELBIT, relbits,
			REL_MISC ) )
	{
		DMSG ( "%s\n", "abs init" ) ;
		return 0 ;
	}
	return mcr_Device_enable ( & mcr_RelDev, 1 ) ;
}

void mcr_Device_initialize ( )
{
	mtx_init ( & _deviceLock, mtx_plain ) ;
	keyDevice_init ( ) ;
	absDevice_init ( ) ;
	relDevice_init ( ) ;
	// Magic sleep to wait for device handlers.
	sleep ( 1 ) ;
}

void mcr_Device_cleanup ( )
{
	mcr_Device_free ( & mcr_KeyDev ) ;
	mcr_Device_free ( & mcr_AbsDev ) ;
	mcr_Device_free ( & mcr_RelDev ) ;
	mtx_destroy ( & _deviceLock ) ;
}
