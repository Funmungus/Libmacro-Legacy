/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
  Copyright (C) 2013  Jonathan D. Pelletier

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

# include "signal/lnx/device.h"
# include "signal/isignal.h"

MCR_API mcr_Device mcr_genDev ;
MCR_API mcr_Device mcr_absDev ;

const MCR_API struct input_event mcr_syncer = {
		.type = EV_SYN,
		.code = SYN_REPORT } ;
MCR_API __s32 mcr_abs_resolution = MCR_ABS_RESOLUTION ;

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

static int genDevice_init ( ) ;
static int absDevice_init ( ) ;

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

void mcr_Device_set_absolute_resolution ( __s32 resolution )
{
	mcr_abs_resolution = resolution ;
	int wasEnabled = 0 ;
	if ( mcr_absDev.enabled )
		wasEnabled = 1 ;
	mcr_Device_free ( & mcr_absDev ) ;
	absDevice_init ( ) ;
	if ( wasEnabled )
		mcr_Device_enable ( & mcr_absDev, 1 ) ;
//	if ( mcr_relDev.enabled )
//		wasEnabled = 1 ;
//	mcr_Device_free ( & mcr_relDev ) ;
//	relDevice_init ( ) ;
//	if ( wasEnabled )
//		mcr_Device_enable ( & mcr_relDev, 1 ) ;
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
	MCR_MAP_FOR_EACH_VALUE ( devPt->type_value_map,
			MCR_EXP ( mcr_Array_free_foreach ),) ;
	mcr_Map_free ( & devPt->type_value_map ) ;
	mcr_Device_init ( devPt ) ;
}

int mcr_Device_enable ( mcr_Device * devPt, int enable )
{
	int islocked = 0 ;
# define returnerror( expression ) \
	if ( expression ) \
	{ \
		dmsg ; \
		if ( ! device_close ( devPt ) ) \
		{ dmsg ; } \
		if ( islocked ) \
			mtx_unlock ( & _deviceLock ) ; \
		return 0 ; \
	}
	dassert ( devPt ) ;
	devPt->enabled = 0 ;
	device_close_event ( devPt ) ;
	// If already enabled, need to close and restart.
	returnerror ( ! device_close ( devPt ) )

	if ( ! enable ) // disable complete
		return 1 ;
	// Cannot have device with no evbits to set.
	// Must have at least 1 UI_SET_EVBIT.
	returnerror ( ! mcr_Device_has_evbit ( devPt ) )

	mtx_lock ( & _deviceLock ) ;
	++ islocked ;
	// Start by opening.
	returnerror ( ! device_open ( devPt ) )

	// Force evbit satisfaction.
	returnerror ( ! device_write_evbit ( devPt ) )

	// Then write all other bits. Ensured at least one mapping pair.
	int valid = device_write_non_evbit ( devPt ) ;

	returnerror ( ! device_create ( devPt ) )

	// Created and ready. valid true is all good, false means some
	// non-UI_SET_EVBIT did not work.
	++ devPt->enabled ;
	if ( ! device_open_event ( devPt ) )
	{
		dmsg ;
		valid = 0 ;
	}
	mtx_unlock ( & _deviceLock ) ;
	return valid ;
# undef returnerror
}

int mcr_Device_enable_all ( int enable )
{
	int success = mcr_Device_enable ( & mcr_genDev, enable ) ;
	if ( ! mcr_Device_enable ( & mcr_absDev, enable ) )
		success = 0 ;
//	if ( ! mcr_Device_enable ( & mcr_relDev, enable ) )
//		return 0 ;
	return success ;
}

void mcr_Device_usage ( )
{
	fprintf ( mcr_out, "%s\n", "" ) ;
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

mcr_Array * mcr_Device_bits ( mcr_Device * devPt, int bitType )
{
	dassert ( devPt ) ;
	return MCR_MAP_GET_VALUE ( devPt->type_value_map, & bitType ) ;
}

int mcr_Device_has_evbit ( mcr_Device * devPt )
{
	dassert ( devPt ) ;
	int evbit = UI_SET_EVBIT ;
	mcr_Array * evbit_arr = MCR_MAP_GET_VALUE ( devPt->type_value_map,
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
		if ( ioctl ( devPt->fd, UI_DEV_DESTROY ) < 0
				|| close ( devPt->fd ) < 0 )
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
		if ( close ( devPt->event_fd ) < 0 )
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
		if ( stat ( entry->d_name, & s ) < 0 )
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
	if ( comp < 0 )
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
	if ( ioctl ( fd, setBit, * bitPt ) < 0 )
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
	// TODO write evbit here and not check while enabling
	MCR_ARR_FOR_EACH ( * bits, write_bit_redirect, devPt->fd,
			setBit, & success ) ;
	return success ;
}

static int device_write_evbit ( mcr_Device * devPt )
{
	dassert ( devPt ) ;
	int evbit = UI_SET_EVBIT ;
	mcr_Array * evbit_arr = MCR_MAP_GET_VALUE ( devPt->type_value_map,
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
	mcr_Array * evbitArr = MCR_MAP_VALUE ( devPt->type_value_map,
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
	MCR_MAP_FOR_EACH ( devPt->type_value_map, write_non_evbit_redirect,
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

static void rangeindex ( int * arr, int cnt )
{
	for ( int i = 0 ; i < cnt ; i ++ )
	{ arr [ i ] = i ; }
}

static int genDevice_init ( )
{
# define devset( arr, max, uibit ) \
	if ( ! mcr_Device_set_bits ( & mcr_genDev, uibit, arr, max ) ) \
	{ \
		dmsg ; \
		return 0 ; \
	}

	int i, j ;
	mcr_Device_init ( & mcr_genDev ) ;

	snprintf ( mcr_genDev.device.name, UINPUT_MAX_NAME_SIZE,
			"macrolibrary-gen" ) ;

	mcr_genDev.device.id.bustype = BUS_VIRTUAL ;
	mcr_genDev.device.id.vendor = 1 ;
	mcr_genDev.device.id.product = 1 ;
	mcr_genDev.device.id.version = 1 ;

	int evbits [ KEY_CNT ] ;
	for ( i = j = 0 ; i < EV_CNT ; i ++ )
	{
		if ( i != EV_ABS )
			evbits [ j ++ ] = i ;
	}
	devset ( evbits, EV_CNT - 1, UI_SET_EVBIT )
	rangeindex ( evbits, KEY_CNT ) ;
	// Do not write 0 value
	devset ( evbits, KEY_CNT, UI_SET_KEYBIT )
	devset ( evbits, REL_CNT, UI_SET_RELBIT )
	devset ( evbits, MSC_CNT, UI_SET_MSCBIT )
	devset ( evbits, SW_CNT, UI_SET_SWBIT )
	devset ( evbits, LED_CNT, UI_SET_LEDBIT )
	devset ( evbits, SND_CNT, UI_SET_SNDBIT )
	devset ( evbits, FF_CNT, UI_SET_FFBIT )
	evbits [ 0 ] = INPUT_PROP_POINTER ;
	evbits [ 1 ] = INPUT_PROP_DIRECT ;
	devset ( evbits, 2, UI_SET_PROPBIT )

	return 1 ;
# undef devset
}

static int absDevice_init ( )
{
	int i ;
	mcr_Device_init ( & mcr_absDev ) ;

	snprintf ( mcr_absDev.device.name, UINPUT_MAX_NAME_SIZE,
			"macrolibrary-abs" ) ;

	mcr_absDev.device.id.bustype = BUS_VIRTUAL ;
	mcr_absDev.device.id.vendor = 1 ;
	mcr_absDev.device.id.product = 1 ;
	mcr_absDev.device.id.version = 1 ;

	for ( int i = 0 ; i <= ABS_MISC ; i++ )
	{
		mcr_absDev.device.absmax [ i ] = mcr_abs_resolution ;
	}

	int evbits [ ] = { EV_SYN, EV_KEY, EV_ABS } ;
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
	evbits [ 0 ] = INPUT_PROP_POINTER ;
	evbits [ 1 ] = INPUT_PROP_DIRECT ;
	if ( ! mcr_Device_set_bits ( & mcr_absDev, UI_SET_PROPBIT, evbits, 2 ) )
	{
		dmsg ;
		return 0 ;
	}
	int absbits [ ABS_MISC + 1 ] ;
	for ( i = 0 ; i <= ABS_MISC ; i++ )
	{
		absbits [ i ] = i ;
	}

	if ( ! mcr_Device_set_bits ( & mcr_absDev, UI_SET_ABSBIT, absbits,
			ABS_MISC + 1 ) )
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
	mcr_String_from_string ( & _uinputPath, MCR_STR ( MCR_UINPUT_PATH ) ) ;
	mcr_String_from_string ( & _eventPath, MCR_STR ( MCR_EVENT_PATH ) ) ;
	/* ioctl is unpredictable for valgrind. The first ioctl will
	 * be uninitialized, and others should not cause errors. */
	genDevice_init ( ) ;
	absDevice_init ( ) ;
}

void mcr_Device_cleanup ( )
{
	mcr_Device_free ( & mcr_genDev ) ;
	mcr_Device_free ( & mcr_absDev ) ;
	mcr_Array_free ( & _uinputPath ) ;
	mcr_Array_free ( & _eventPath ) ;
	mtx_destroy ( & _deviceLock ) ;
}
