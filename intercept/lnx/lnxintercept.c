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

# ifndef _GNU_SOURCE
# define _GNU_SOURCE
# endif

# include "intercept/intercept.h"
# include "intercept/lnx/intercept.h"
# include "signal/lnx/standard.h"

typedef struct
{
	mcr_Grabber grabber ;
	int complete ;
} GrabComplete ;

static mtx_t _lock ;
static cnd_t _cnd ;
//static mcr_Map * _grabMapPt = NULL ; 	// Heap map, accessed by threads.
static mcr_Array _grabCompletes ;
static mcr_Array _grabPaths ;

// KEY_CNT / 8 is a floor value, and may have remainder of keys.
static char _bitRetrieval [ KEY_CNT / 8 + 1 ] ;

static int is_enabled_impl ( ) ;
static void enable_impl ( int enable ) ;
static int add_impl ( const char * grabPath ) ;
static int thread_enable ( void * threadArgs ) ;
static void grab_impl ( const char * grabPath ) ;
static int intercept_start ( void * threadArgs ) ;
static int read_grabber_exclusive ( mcr_Grabber * grabPt ) ;
static void unknown_event ( int fd, struct input_event * events,
		size_t size ) ;
static unsigned int modify_eventbits ( char * keybit_values ) ;
static unsigned int max_mod_val ( ) ;
//static void clear_grabbers ( ) ;
static void grabdisable_foreach ( GrabComplete ** ptPt, ... ) ;
static void grabcomplete_free_foreach ( GrabComplete ** it, ... ) ;
static void clear_grabbers_impl ( ) ;

# define MODBIT_SIZE ( ( sizeof ( char ) ) * max_mod_val ( ) / 8 + 1 )

int mcr_intercept_enabled ( )
{
	mtx_lock ( & _lock ) ;
	int ret = is_enabled_impl ( ) ;
	mtx_unlock ( & _lock ) ;
	return ret ;
}

void mcr_intercept_set_enabled ( int enable )
{
	mtx_lock ( & _lock ) ;
	// Do not disable if already disabled.
	if ( enable || is_enabled_impl ( ) )
	{
		thrd_t trd ;
		if ( thrd_create ( & trd, thread_enable, ( void * )
				( intptr_t ) enable ) == thrd_success )
		{
			thrd_detach ( trd ) ;
		}
	}
	mtx_unlock ( & _lock ) ;
}

unsigned int mcr_intercept_get_mods ( )
{
	int ret = ioctl ( mcr_keyDev.fd, EVIOCGKEY
			( MODBIT_SIZE ), _bitRetrieval ) ;
	if ( ret < 0 )
	{
		dmsg ;
		return mcr_internalMods ;
	}
	return modify_eventbits ( _bitRetrieval ) ;
}

void mcr_intercept_add_grab ( const char * grabPath )
{
	dassert ( grabPath ) ;
	mtx_lock ( & _lock ) ;
	add_impl ( grabPath ) ;
	mtx_unlock ( & _lock ) ;
}

void mcr_intercept_remove_grab ( const char * grabPath )
{
	dassert ( grabPath ) ;
	mtx_lock ( & _lock ) ;
	if ( ! _grabPaths.used )
	{
		mtx_unlock ( & _lock ) ;
		return ;
	}
	mcr_Array * found = bsearch ( & grabPath, _grabPaths.array,
			_grabPaths.used, sizeof ( mcr_Array ), mcr_str_compare ) ;
	if ( found )
	{
		mcr_Array_free ( found ) ;
		mcr_Array_remove ( & _grabPaths,
				MCR_ARR_INDEXOF ( _grabPaths, found ) ) ;
	}
	mtx_unlock ( & _lock ) ;
}

int mcr_intercept_set_grabs ( const char ** allGrabPaths,
		size_t pathCount )
{
	mtx_lock ( & _lock ) ;
	MCR_ARR_FOR_EACH ( _grabPaths,
			MCR_EXP ( mcr_Array_free_foreach ),) ;
	_grabPaths.used = 0 ;
	int ret = 1 ;
	for ( size_t i = 0 ; i < pathCount ; i ++ )
	{
		if ( ! add_impl ( allGrabPaths [ i ] ) )
		{
			dmsg ;
			ret = 0 ;
		}
	}
	mcr_Array_trim ( & _grabPaths ) ;
	mtx_unlock ( & _lock ) ;
	return ret ;
}

static int is_enabled_impl ( )
{
	GrabComplete ** ptArr = MCR_ARR_AT ( _grabCompletes, 0 ) ;
	for ( size_t i = 0 ; i < _grabCompletes.used ; i ++ )
	{
		if ( MCR_GRABBER_ENABLED ( ptArr [ i ]->grabber ) &&
				! ptArr [ i ]->complete )
			return 1 ;
	}
	return 0 ;
//	mcr_Grabber ** end = MCR_ARR_END ( & _grabMapPt->set ) ;
//	for ( mcr_Grabber ** grabPtPt = MCR_MAP_VALUE ( _grabMapPt, MCR_ARR_AT
//			( & _grabMapPt->set, 0 ) ) ; grabPtPt < end ;
//			grabPtPt = MCR_ARR_NEXT ( & _grabMapPt->set, grabPtPt ) )
//	{
//		if ( MCR_GRABBER_ENABLED ( * grabPtPt ) )
//		{
//			return 1 ;
//		}
//	}
//	return 0 ;
}

static void enable_impl ( int enable )
{
	if ( enable )
	{
		mcr_Array * pathArr = ( mcr_Array * ) _grabPaths.array ;
		for ( size_t i = 0 ; i < _grabPaths.used ; i ++ )
		{
			grab_impl ( pathArr [ i ].array ) ;
		}
	}
	else
	{
		clear_grabbers_impl ( ) ;
	}
}

static int add_impl ( const char * grabPath )
{
	void * found = NULL ;
	if ( _grabPaths.used )
		found = bsearch ( & grabPath, _grabPaths.array, _grabPaths.used,
				sizeof ( mcr_Array ), mcr_str_compare ) ;
	if ( ! found )
	{
		mcr_Array str ;
		mcr_String_init ( & str ) ;
		if ( mcr_String_from_string ( & str, grabPath ) &&
				mcr_Array_push ( & _grabPaths, & str ) )
		{
			qsort ( _grabPaths.array, _grabPaths.used,
					sizeof ( mcr_Array ), mcr_str_compare ) ;
		}
		else
		{
			dmsg ;
			mcr_Array_free ( & str ) ;
			return 0 ;
		}
	}
	return 1 ;
}

static int thread_enable ( void * threadArgs )
{
	int enable = ( int ) ( intptr_t ) threadArgs ;
	mcr_dclock ( clock_t start ) ;
	mtx_lock ( & _lock ) ;
	enable_impl ( enable ) ;
	mtx_unlock ( & _lock ) ;
	mcr_profile ( start ) ;
	return thrd_success ;
}

static void grab_impl ( const char * grabPath )
{
	dassert ( grabPath ) ;
	GrabComplete * pt = malloc ( sizeof ( GrabComplete ) ) ;
	int err = 0 ;
	if ( pt )
	{
		thrd_t trd ;
		pt->complete = 0 ;
		mcr_Grabber_init_with ( & pt->grabber, grabPath, 0 ) ;
		if ( mcr_Array_push ( & _grabCompletes, & pt ) )
		{
			if ( thrd_create ( & trd, intercept_start, pt ) ==
				thrd_success )
			{
				if ( thrd_detach ( trd ) != thrd_success )
				{ dmsg ; }
				cnd_broadcast ( & _cnd ) ;
			}
			else
			{
				dmsg ;
				mcr_Array_pop ( & _grabCompletes ) ;
				err = 1 ;
			}
		}
		else
		{
			dmsg ;
			err = 1 ;
		}
		if ( err )
		{
			mcr_Grabber_free ( & pt->grabber ) ;
			free ( pt ) ;
		}
	}
	else
	{ dmsg ; }
	/* Must have
	 * 1 ) grabber allocated
	 * 2 ) mapped from grab path
	 * 3 ) thread started properly
	 **/
//	mcr_Grabber ** grabPtPt = mcr_Map_get ( _grabMapPt, & grabPath ) ;
//	if ( ! grabPtPt )
//	{
//		mcr_Grabber * grabPt = malloc ( sizeof ( mcr_Grabber ) ) ;
//		if ( grabPt )
//		{
//			thrd_t trd ;
//			mcr_Grabber_init_with ( grabPt, grabPath, 0 ) ;
//			if ( mcr_Map_map ( _grabMapPt, & grabPt->path.array,
//					& grabPt ) &&
//					thrd_create ( & trd, intercept_start, grabPt ) ==
//					thrd_success )
//			{
//				if ( thrd_detach ( trd ) != thrd_success )
//				{
//					dmsg ;
//				}
//				cnd_broadcast ( & _cnd ) ;
//			}
//			else
//			{
//				dmsg ;
//				mcr_Map_unmap ( _grabMapPt, & grabPath ) ;
//				mcr_Grabber_free ( grabPt ) ;
//				free ( grabPt ) ;
//			}
//		}
//		else
//		{ dmsg ; }
//	}
}

# define KEY_SETALL( keyObj, key, scan, uptype ) \
	MCR_KEY_SET_KEY ( keyObj, key ) ; \
	MCR_KEY_SET_SCAN ( keyObj, scan ) ; \
	MCR_KEY_SET_UP_TYPE ( keyObj, uptype ) ;

# define MC_SETALL( mcObj, posBuff, justify ) \
	MCR_MOVECURSOR_SET_POSITION ( mcObj, posBuff ) ; \
	MCR_MOVECURSOR_SET_JUSTIFY ( mcObj, justify ) ;

// 0 to not block, otherwise do block.
# define DISP( sig ) \
	( ( sig ).type->dispatch ? \
			( sig ).type->dispatch ( & ( sig ) ) : 0 )

# define abspos( evPos, setPos ) \
	if ( evPos == ABS_X ) 	\
		setPos = MCR_X ; 	\
	else if ( evPos == ABS_Y ) 	\
		setPos = MCR_Y ; 	\
	else if ( evPos == ABS_Z ) 	\
		setPos = MCR_Z ; 	\
	else	\
		break ; 	/* Unknown, breakout */

# define relpos( evPos, setPos ) \
	if ( evPos == REL_X || evPos == REL_HWHEEL ) 	\
		setPos = MCR_X ; 	\
	else if ( evPos == REL_Y || evPos == REL_WHEEL ) 	\
		setPos = MCR_Y ; 	\
	else if ( evPos == REL_Z || evPos == REL_DIAL ) 	\
		setPos = MCR_Z ; 	\
	else	\
		break ; 	/* Unknown, breakout */

# define ABS_SETCURRENT( abs, hasPosArray ) \
	if ( ! ( hasPosArray ) [ MCR_X ] ) \
	{ \
		MCR_MOVECURSOR_SET_COORDINATE ( abs, MCR_X, \
				mcr_cursor [ MCR_X ] ) ; \
	} \
	if ( ! ( hasPosArray ) [ MCR_Y ] ) \
	{ \
		MCR_MOVECURSOR_SET_COORDINATE ( abs, MCR_Y, \
				mcr_cursor [ MCR_Y ] ) ; \
	} \
	if ( ! ( hasPosArray ) [ MCR_Z ] ) \
	{ \
		MCR_MOVECURSOR_SET_COORDINATE ( abs, MCR_Z, \
				mcr_cursor [ MCR_Z ] ) ; \
	}

// Note: Excess time setting up and releasing is ok. Please try to limit
// time-consumption during grabbing and callbacks.
/* \pre Thread args is heap-allocated mcr_Grabber. It is assumed to
 * already have a path, which is already mapped in the _grabMapPt.
 * */
static int intercept_start ( void * threadArgs )
{
	dassert ( threadArgs ) ;
	GrabComplete * gpPt = threadArgs ;
	mcr_Grabber * grabPt = & gpPt->grabber ;
	int thrdStatus ;
	mcr_NoOp delay ;
	delay.tv_sec = 0 ;
	delay.tv_nsec = 1000000 /* 1 milli */ * 10 /* 10 milli */ ;
	/* 1. Create grab from path.
	 * 2. Enable.
	 * 3. Use grabber.
	 * 4. Remove grabber reference in main before thread ends.
	 **/

	mtx_lock ( & _lock ) ;
	if ( ! mcr_Grabber_set_enabled ( grabPt, 1 ) )
	{ dmsg ; }
	// Give a delay between grabs before unlocking.
	MCR_NOOP_QUICKSEND ( delay ) ;
	mtx_unlock ( & _lock ) ;

	// Will return an error if not enabled.
	thrdStatus = read_grabber_exclusive ( grabPt ) ;

	mtx_lock ( & _lock ) ;
	if ( ! mcr_Grabber_set_enabled ( grabPt, 0 ) )
	{ dmsg ; }
//	dassert ( _grabMapPt ) ;
//	mcr_Map_unmap ( _grabMapPt, & grabPt->path.array ) ;
//	mcr_Grabber_free ( grabPt ) ;
//	free ( grabPt ) ;
//	grabPt = NULL ;
	gpPt->complete = 1 ;
	cnd_broadcast ( & _cnd ) ;
	mtx_unlock ( & _lock ) ;
	return thrdStatus ;
}

static int read_grabber_exclusive ( mcr_Grabber * grabPt )
{
	dassert ( grabPt ) ;
	struct input_event events [ MCR_GRAB_SET_LENGTH ] ;
	int rdb, rdn, i, pos ;
	int bAbs [ MCR_DIMENSION_CNT ] = { 0 } ;
	int * echoFound = NULL ;
	mcr_SpacePosition nopos = { 0 } ;
	mcr_Key key ;
	mcr_HIDEcho echo ;
	mcr_MoveCursor abs, rel ;
	mcr_Scroll scr ;
	mcr_Signal keysig = { & mcr_iKey, { & key, 0 }, 1 },
			echosig = { & mcr_iHIDEcho, { & echo, 0 }, 1 },
			abssig = { & mcr_iMoveCursor, { & abs, 0 }, 1 },
			relsig = { & mcr_iMoveCursor, { & rel, 0 }, 1 },
			scrsig = { & mcr_iScroll, { & scr, 0 }, 1 } ;
	mcr_Key_init_with ( & key, 0, 0, MCR_DOWN ) ;
	mcr_Echo_init_with ( & echo, 0 ) ;
	mcr_MoveCursor_init_with ( & abs, nopos, 0 ) ;
	mcr_MoveCursor_init_with ( & rel, nopos, 1 ) ;
	mcr_Scroll_init_with ( & scr, nopos ) ;
	struct pollfd fd = { 0 } ;
	fd.events = -1 ;
	fd.fd = grabPt->fd ;
	while ( MCR_GRABBER_ENABLED ( * grabPt ) ) 	/* Disable point 1 */
	{	/* Disable point 2. */
		i = poll ( & fd, 1, 3000 ) ;
		if ( ! MCR_GRABBER_ENABLED ( * grabPt ) )
			return thrd_success ;
		if ( ! i || i < 0 )	/* Error or no data in 10 seconds. */
			continue ;			/* Try again. */
		/* Disable point 3, but assume readable from select above.
		 * If rdb < input_event size then we are returning an error
		 * anyways. */
		rdb = read ( fd.fd, events, sizeof ( events ) ) ;
		if ( ! MCR_GRABBER_ENABLED ( * grabPt ) )
			return thrd_success ;
		if ( rdb < ( int ) sizeof ( struct input_event ) )
		{
			dmsg ;
			return thrd_error ;
		}
		rdn = rdb / sizeof ( struct input_event ) ;
		for ( i = 0 ; i < rdn ; i ++ )
		{
			switch ( events [ i ].type )
			{
			case EV_MSC :	/* Handle MSC */
				if ( events [ i ].code == MSC_SCAN )
				{
					// Consume a key that already has scan code.
					if ( MCR_KEY_SCAN ( key ) )
					{
						MCR_SEND ( keysig ) ;
						KEY_SETALL ( key, 0, events [ i ].value,
								MCR_BOTH ) ;
					}
					else
					{
						MCR_KEY_SET_SCAN ( key, events [ i ].value ) ;
					}
				}
				break ;
			case EV_KEY :	/* Handle KEY */
				if ( MCR_KEY_KEY ( key ) )
				{
					echoFound = MCR_MAP_GET_VALUE ( mcr_keyToEcho
							[ MCR_KEY_UP_TYPE ( key ) ],
							& MCR_KEY_KEY ( key ) ) ;
					if ( echoFound )
					{
						MCR_ECHO_SET_ECHO ( echo, * echoFound ) ;
						if ( DISP ( echosig ) )
						{
							KEY_SETALL ( key, events [ i ].code, 0,
								events [ i ].value ? MCR_DOWN : MCR_UP ) ;
							break ;
						}
					}
					MCR_SEND ( keysig ) ;
					KEY_SETALL ( key, events [ i ].code, 0,
							events [ i ].value ? MCR_DOWN : MCR_UP ) ;
				}
				else
				{
					MCR_KEY_SET_KEY ( key, events [ i ].code ) ;
					MCR_KEY_SET_UP_TYPE ( key,
							events [ i ].value ? MCR_DOWN :
							MCR_UP ) ;
				}
				break ;
			case EV_ABS :	/* Handle ABS */
				abspos ( events [ i ].code, pos ) ;
				if ( bAbs [ pos ] )
				{
					ABS_SETCURRENT ( abs, bAbs ) ;
					MCR_SEND ( abssig ) ;
					memset ( bAbs, 0, sizeof ( bAbs ) ) ;
				}
				MCR_MOVECURSOR_SET_COORDINATE ( abs, pos,
						events [ i ].value ) ;
				++ bAbs [ pos ] ;
				break ;
			case EV_REL :
				relpos ( events [ i ].code, pos ) ;
				if ( events [ i ].code == REL_X || events [ i ].code
						== REL_Y || events [ i ].code == REL_Z )
				{
					if ( MCR_MOVECURSOR_COORDINATE ( rel, pos ) )
					{
						MCR_SEND ( relsig ) ;
						MCR_MOVECURSOR_SET_POSITION ( rel, nopos ) ;
					}
					MCR_MOVECURSOR_SET_COORDINATE ( rel, pos,
							events [ i ].value ) ;
				}
				// Currently assuming scroll if not relative movement.
				else
				{
					if ( MCR_SCROLL_COORDINATE ( scr, pos ) )
					{
						MCR_SEND ( scrsig ) ;
						MCR_SCROLL_SET_DIMENSIONS ( scr, nopos ) ;
					}
					MCR_SCROLL_SET_COORDINATE ( scr, pos,
							events [ i ].value ) ;
				}
				break ;
# ifdef DEBUG
			default :
			{
				// synch is not directly handled, but it is known.
				if ( events [ i ].type != EV_SYN )
				{
					unknown_event ( fd.fd, events, MCR_GRAB_SET_SIZE ) ;
					KEY_SETALL ( key, 0, 0, MCR_BOTH ) ;
					memset ( bAbs, 0, sizeof ( bAbs ) ) ;
					MCR_MOVECURSOR_SET_POSITION ( rel, nopos ) ;
					MCR_SCROLL_SET_DIMENSIONS ( scr, nopos ) ;
					/* all events should be consumed by unknown_event */
					rdn = 0 ;
				}
			}
# endif
			}
		}
		//Call final event, it was not called yet.
		if ( MCR_KEY_KEY ( key ) || MCR_KEY_SCAN ( key ) )
		{
			if ( MCR_KEY_KEY ( key ) )
			{
				echoFound = MCR_MAP_GET_VALUE ( mcr_keyToEcho
						[ MCR_KEY_UP_TYPE ( key ) ],
						& MCR_KEY_KEY ( key ) ) ;
				if ( echoFound )
				{
					MCR_ECHO_SET_ECHO ( echo, * echoFound ) ;
					if ( ! DISP ( echosig ) )
					{
						MCR_SEND ( keysig ) ;
					}
				}
				else
				{
					MCR_SEND ( keysig ) ;
				}
			}
			KEY_SETALL ( key, 0, 0, MCR_BOTH ) ;
		}
		for ( i = 0 ; i < MCR_DIMENSION_CNT ; i ++ )
		{
			if ( bAbs [ i ] )
			{
				ABS_SETCURRENT ( abs, bAbs ) ;
				MCR_SEND ( abssig ) ;
				memset ( bAbs, 0, sizeof ( bAbs ) ) ;
				break ;
			}
		}
		for ( i = 0 ; i < MCR_DIMENSION_CNT ; i ++ )
		{
			if ( MCR_MOVECURSOR_COORDINATE ( rel, i ) )
			{
				MCR_SEND ( relsig ) ;
				MCR_MOVECURSOR_SET_POSITION ( rel, nopos ) ;
				break ;
			}
		}
		for ( i = 0 ; i < MCR_DIMENSION_CNT ; i ++ )
		{
			if ( MCR_SCROLL_COORDINATE ( scr, i ) )
			{
				MCR_SEND ( scrsig ) ;
				MCR_SCROLL_SET_DIMENSIONS ( scr, nopos ) ;
				break ;
			}
		}
	}
	return thrd_success ;
}

static void unknown_event ( int fd, struct input_event * events,
		size_t size )
{
	if ( ioctl ( fd, EVIOCGRAB, 0 ) == -1 )
	{ dmsg ; return ; }
	write ( fd, events, size ) ;
	read ( fd, events, size ) ;
	if ( ioctl ( fd, EVIOCGRAB, 1 ) == -1 )
	{ dmsg ; }
}

static unsigned int modify_eventbits ( char * keybitValues )
{
	unsigned int modVal = 0 ;
	mcr_Array keyBuffer ;
	mcr_Array_init ( & keyBuffer, sizeof ( int ) ) ;
	if ( ! mcr_Array_resize ( & keyBuffer, mcr_Mod_key_count ( ) ) )
	{
		dmsg ;
		mcr_Array_free ( & keyBuffer ) ;
		return 0 ;
	}
	keyBuffer.used = keyBuffer.size ;
	mcr_Mod_key_all ( ( int * ) keyBuffer.array, keyBuffer.used ) ;
	int * keyArr = ( int * ) keyBuffer.array ;
	for ( unsigned int i = 0 ; i < keyBuffer.used ; i++ )
	{
		if ( ( keybitValues [ MCR_EVENTINDEX ( keyArr [ i ] ) ] &
				MCR_EVENTBIT ( keyArr [ i ] ) ) )
			modVal |= mcr_Mod_from_key ( keyArr [ i ] ) ;
	}
	mcr_Array_free ( & keyBuffer ) ;
	return modVal ;
}

static unsigned int max_mod_val ( )
{
	unsigned int val = KEY_LEFTALT ;
	val = KEY_RIGHTALT > val ? KEY_RIGHTALT : val ;
	val = KEY_OPTION > val ? KEY_OPTION : val ;
	val = KEY_COMPOSE > val ? KEY_COMPOSE : val ;
	val = KEY_LEFTCTRL > val ? KEY_LEFTCTRL : val ;
	val = KEY_RIGHTCTRL > val ? KEY_RIGHTCTRL : val ;
	val = KEY_FN > val ? KEY_FN : val ;
	val = KEY_FRONT > val ? KEY_FRONT : val ;
	val = KEY_LEFTSHIFT > val ? KEY_LEFTSHIFT : val ;
	val = KEY_RIGHTSHIFT > val ? KEY_RIGHTSHIFT : val ;
	val = KEY_LEFTMETA > val ? KEY_LEFTMETA : val ;
	val = KEY_RIGHTMETA > val ? KEY_RIGHTMETA : val ;
	return val ;
}

void mcr_intercept_native_initialize ( )
{
//	int i = 0 ;
	mtx_init ( & _lock, mtx_plain ) ;
	cnd_init ( & _cnd ) ;
	// Fatal if path to grab map cannot be allocated.
//	_grabMapPt = malloc ( sizeof ( mcr_Map ) ) ;
//	while ( ! _grabMapPt && i ++ < 10 )
//	{
//		dmsg ;
//		sleep ( 1 ) ;
//		_grabMapPt = malloc ( sizeof ( mcr_Map ) ) ;
//	}
//	if ( ! _grabMapPt )
//	{
//		dmsg ;
//		exit ( thrd_nomem ) ;
//	}
//	mcr_Map_init ( _grabMapPt, sizeof ( const char * ),
//			sizeof ( mcr_Grabber * ) ) ;
//	_grabMapPt->compare = mcr_str_compare ;
	mcr_Array_init ( & _grabCompletes, sizeof ( GrabComplete * ) ) ;
	mcr_Array_init ( & _grabPaths, sizeof ( mcr_Array ) ) ;
}

static void grabdisable_foreach ( GrabComplete ** ptPt, ... )
{
	mcr_Grabber_set_enabled ( & ( * ptPt )->grabber, 0 ) ;
}

static void grabcomplete_free_foreach ( GrabComplete ** ptPt, ... )
{
	mcr_Grabber_free ( & ( * ptPt )->grabber ) ;
	free ( * ptPt ) ;
}

//static void clear_grabbers ( )
//{
//	mtx_lock ( & _enableLock ) ;
//	clear_grabbers_impl ( ) ;
//	mtx_unlock ( & _enableLock ) ;
//}

//! \pre _enableLock locked
//! \post _enableLock locked
static void clear_grabbers_impl ( )
{
	// TODO: thread destroy on timeout.
//	dassert ( _grabMapPt ) ;
	// Disable all, wait for all batch operation.
//	size_t prev = _grabMapPt->set.used ;
//	MCR_MAP_FOR_EACH_VALUE ( _grabMapPt, grabdisable_redirect, 0 ) ;
	size_t prev = _grabCompletes.used, i ;
	MCR_ARR_FOR_EACH ( _grabCompletes, grabdisable_foreach, 0 ) ;
	struct timespec timeout = { 1, 0 } ;
	GrabComplete ** ptArr ;
	while ( _grabCompletes.used )
	{
		// Disable any that are being created while clearing.
		if ( _grabCompletes.used >= prev )
		{
			MCR_ARR_FOR_EACH ( _grabCompletes, grabdisable_foreach, 0 ) ;
		}
		ptArr = MCR_ARR_AT ( _grabCompletes, 0 ) ;
		for ( i = 0 ; i < _grabCompletes.used ; )
		{
			if ( ptArr [ i ]->complete )
			{
				mcr_Grabber_free ( & ptArr [ i ]->grabber ) ;
				free ( ptArr [ i ] ) ;
				mcr_Array_remove ( & _grabCompletes, i ) ;
			}
			else
			{ ++ i ; }
		}
		prev = _grabCompletes.used ;
		// Impatient free-all
		if ( cnd_timedwait ( & _cnd, & _lock, & timeout ) ==
				thrd_timeout )
		{
			MCR_ARR_FOR_EACH ( _grabCompletes,
					grabdisable_foreach, 0 ) ;
			MCR_ARR_FOR_EACH ( _grabCompletes,
					grabcomplete_free_foreach, 0 ) ;
			_grabCompletes.used = 0 ;
		}
	}
}

void mcr_intercept_native_cleanup ( void )
{
//	dassert ( _grabMapPt ) ;
	mtx_lock ( & _lock ) ;
	clear_grabbers_impl ( ) ;
	MCR_ARR_FOR_EACH ( _grabPaths,
			MCR_EXP ( mcr_Array_free_foreach ), ) ;
	mcr_Array_free ( & _grabPaths ) ;
	mcr_Array_free ( & _grabCompletes ) ;
//	// Grabbers will now be empty.
//	mcr_Map_free ( _grabMapPt ) ;
//	free ( _grabMapPt ) ;
//	_grabMapPt = NULL ;
	mtx_unlock ( & _lock ) ;
	mtx_destroy ( & _lock ) ;
	cnd_destroy ( & _cnd ) ;
}
