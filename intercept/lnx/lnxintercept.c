/* intercept/lnx/lnxintercept.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# ifndef _GNU_SOURCE
# define _GNU_SOURCE
# endif

# include "intercept/intercept.h"
# include "intercept/lnx/intercept.h"
# include "signal/lnx/standard.h"

static mtx_t _enableLock ;
static cnd_t _countCnd ;
static mcr_Map * _grabMapPt = NULL ; 	// Heap map, accessed by threads.
static mcr_Array _grabPaths ;

// KEY_CNT / 8 is a floor value, and may have remainder of keys.
static char _bitRetrieval [ KEY_CNT / 8 + 1 ] ;

static int is_enabled_impl ( ) ;
static void enable_impl ( int enable ) ;
static int add_impl ( const char * grabPath ) ;
static int thread_enable ( void * threadArgs ) ;
static void grab_impl ( const char * grabPath ) ;
static int intercept_start ( void * threadArgs ) ;
static int read_grabber ( mcr_Grabber * grabPt ) ;
static unsigned int modify_eventbits ( char * keybit_values ) ;
static unsigned int max_mod_val ( ) ;
//static void clear_grabbers ( ) ;
static void clear_grabbers_impl ( ) ;

# define MODBIT_SIZE ( ( sizeof ( char ) ) * max_mod_val ( ) / 8 + 1 )

int mcr_intercept_is_enabled ( )
{
	mtx_lock ( & _enableLock ) ;
	int ret = is_enabled_impl ( ) ;
	mtx_unlock ( & _enableLock ) ;
	return ret ;
}

void mcr_intercept_enable ( int enable )
{
	mtx_lock ( & _enableLock ) ;
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
	mtx_unlock ( & _enableLock ) ;
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
	mtx_lock ( & _enableLock ) ;
	add_impl ( grabPath ) ;
	mtx_unlock ( & _enableLock ) ;
}

void mcr_intercept_remove_grab ( const char * grabPath )
{
	dassert ( grabPath ) ;
	mtx_lock ( & _enableLock ) ;
	if ( ! _grabPaths.used )
		return ;
	mcr_Array * found = bsearch ( & grabPath, _grabPaths.array,
			_grabPaths.used, sizeof ( mcr_Array ), mcr_str_compare ) ;
	if ( found )
	{
		mcr_Array_free ( found ) ;
		mcr_Array_remove ( & _grabPaths,
				MCR_ARR_INDEXOF ( & _grabPaths, found ) ) ;
	}
	mtx_unlock ( & _enableLock ) ;
}
//TODO
int mcr_intercept_set_grabs ( const char ** allGrabPaths,
		size_t pathCount )
{
	mtx_lock ( & _enableLock ) ;
	MCR_ARR_FOR_EACH ( & _grabPaths, mcr_Array_free_foreach, 0 ) ;
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
	mtx_unlock ( & _enableLock ) ;
	return ret ;
}

static int is_enabled_impl ( )
{
	mcr_Grabber ** end = MCR_ARR_END ( & _grabMapPt->set ) ;
	for ( mcr_Grabber ** grabPtPt = MCR_MAP_VALUE ( _grabMapPt, MCR_ARR_AT
			( & _grabMapPt->set, 0 ) ) ; grabPtPt < end ;
			grabPtPt = MCR_ARR_NEXT ( & _grabMapPt->set, grabPtPt ) )
	{
		if ( MCR_GRABBER_ENABLED ( * grabPtPt ) )
		{
			return 1 ;
		}
	}
	return 0 ;
}

static void enable_impl ( int enable )
{
	if ( enable )
	{
		for ( size_t i = 0 ; i < _grabPaths.used ; i ++ )
		{
			grab_impl ( * ( const char ** )
					MCR_ARR_AT ( & _grabPaths, i ) ) ;
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
		mcr_Array arr ;
		mcr_String_init ( & arr ) ;
		if ( mcr_String_from_string ( & arr, grabPath ) &&
				mcr_Array_push ( & _grabPaths, & arr ) )
		{
			qsort ( _grabPaths.array, _grabPaths.used,
					sizeof ( mcr_Array ), mcr_str_compare ) ;
		}
		else
		{
			dmsg ;
			mcr_Array_free ( & arr ) ;
			return 0 ;
		}
	}
	return 1 ;
}

static int thread_enable ( void * threadArgs )
{
	int enable = ( int ) ( intptr_t ) threadArgs ;
	mtx_lock ( & _enableLock ) ;
	enable_impl ( enable ) ;
	mtx_unlock ( & _enableLock ) ;
	return thrd_success ;
}

static void grab_impl ( const char * grabPath )
{
	dassert ( grabPath ) ;
	/* Must have
	 * 1 ) grabber allocated
	 * 2 ) mapped from grab path
	 * 3 ) thread started properly
	 **/
	mcr_Grabber ** grabPtPt = mcr_Map_get ( _grabMapPt, & grabPath ) ;
	if ( ! grabPtPt )
	{
		mcr_Grabber * grabPt = malloc ( sizeof ( mcr_Grabber ) ) ;
		if ( grabPt )
		{
			thrd_t trd ;
			mcr_Grabber_init_with ( grabPt, grabPath, 0 ) ;
			if ( mcr_Map_map ( _grabMapPt, & grabPath, & grabPt ) &&
					thrd_create ( & trd, intercept_start, grabPt ) ==
					thrd_success )
			{
				if ( thrd_detach ( trd ) != thrd_success )
				{
					dmsg ;
				}
				cnd_broadcast ( & _countCnd ) ;
			}
			else
			{
				dmsg ;
				mcr_Map_unmap ( _grabMapPt, & grabPath ) ;
				mcr_Grabber_free ( grabPt ) ;
				free ( grabPt ) ;
			}
		}
		else
		{
			dmsg ;
		}
	}
}

# define key_setall( keyPt, key, scan, uptype ) \
	MCR_KEY_SET ( keyPt, key ) ; \
	MCR_KEY_SET_SCAN ( keyPt, scan ) ; \
	MCR_KEY_SET_UP_TYPE ( keyPt, uptype ) ;

# define mc_setall( mcPt, posBuff, justify ) \
	MCR_MOVECURSOR_SET ( mcPt, posBuff ) ; \
	MCR_MOVECURSOR_ENABLE_JUSTIFY ( mcPt, justify ) ;

// 0 to not block, otherwise do block.
# define disp( sig ) \
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

# define abssetcurrent( absPt, hasPosArray ) \
	if ( ! ( hasPosArray ) [ MCR_X ] ) \
	{ \
		MCR_MOVECURSOR_SET_POSITION ( absPt, MCR_X, \
				mcr_cursor [ MCR_X ] ) ; \
	} \
	if ( ! ( hasPosArray ) [ MCR_Y ] ) \
	{ \
		MCR_MOVECURSOR_SET_POSITION ( absPt, MCR_Y, \
				mcr_cursor [ MCR_Y ] ) ; \
	} \
	if ( ! ( hasPosArray ) [ MCR_Z ] ) \
	{ \
		MCR_MOVECURSOR_SET_POSITION ( absPt, MCR_Z, \
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
	mcr_Grabber * grabPt = threadArgs ;
	int thrdStatus ;
	mcr_NoOp delay ;
	delay.tv_sec = 0 ;
	delay.tv_nsec = 1000000 /* 1 milli */ * 10 /* 10 milli */ ;
	/* 1. Create grab from path.
	 * 2. Enable.
	 * 3. Use grabber.
	 * 4. Remove grabber reference in main before thread ends.
	 **/

	mtx_lock ( & _enableLock ) ;
	if ( ! mcr_Grabber_enable ( grabPt, 1 ) )
		dmsg ;
	// Give a delay between grabs before unlocking.
	MCR_NOOP_QUICKSEND ( & delay ) ;
	mtx_unlock ( & _enableLock ) ;

	// Will return an error if not enabled.
	thrdStatus = read_grabber ( grabPt ) ;

	mtx_lock ( & _enableLock ) ;
	if ( ! mcr_Grabber_enable ( grabPt, 0 ) )
		dmsg ;
	dassert ( _grabMapPt ) ;
	mcr_Map_unmap ( _grabMapPt, & grabPt->path.array ) ;
	mcr_Grabber_free ( grabPt ) ;
	free ( grabPt ) ;
	grabPt = NULL ;
	cnd_broadcast ( & _countCnd ) ;
	mtx_unlock ( & _enableLock ) ;
	return thrdStatus ;
}

static int read_grabber ( mcr_Grabber * grabPt )
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
	mcr_Signal keysig = { & mcr_iKey, { & key, 0 } },
			echosig = { & mcr_iHIDEcho, { & echo, 0 } },
			abssig = { & mcr_iMoveCursor, { & abs, 0 } },
			relsig = { & mcr_iMoveCursor, { & rel, 0 } },
			scrsig = { & mcr_iScroll, { & scr, 0 } } ;
	mcr_Key_init_with ( & key, 0, 0, MCR_DOWN ) ;
	mcr_Echo_init_with ( & echo, 0 ) ;
	mcr_MoveCursor_init_with ( & abs, nopos, 0 ) ;
	mcr_MoveCursor_init_with ( & rel, nopos, 1 ) ;
	mcr_Scroll_init_with ( & scr, nopos ) ;
	while ( MCR_GRABBER_ENABLED ( grabPt ) ) 	/* Disable point 1 */
	{	/* Disable point 2 */
		rdb = read ( grabPt->fd, events, sizeof ( events ) ) ;
		/* Disable point 3 */
		if ( ! MCR_GRABBER_ENABLED ( grabPt ) )
			return thrd_success ;
		if ( rdb < ( int ) sizeof ( struct input_event ) )
			return rdb < ( int ) sizeof ( struct input_event ) ?
					thrd_error : thrd_success ;
		rdn = rdb / sizeof ( struct input_event ) ;
		for ( i = 0 ; i < rdn ; i ++ )
		{
			switch ( events [ i ].type )
			{
			case EV_MSC :	/* Handle MSC */
				if ( events [ i ].code == MSC_SCAN )
				{
					// Consume a key that already has scan code.
					if ( MCR_KEY_GET_SCAN ( & key ) )
					{
						MCR_SEND ( & keysig ) ;
						key_setall ( & key, 0, events [ i ].value,
								MCR_BOTH ) ;
					}
					else
					{
						MCR_KEY_SET_SCAN ( & key, events [ i ].value ) ;
					}
				}
				break ;
			case EV_KEY :	/* Handle KEY */
				if ( MCR_KEY_GET ( & key ) )
				{
					echoFound = MCR_MAP_GET_VALUE ( mcr_keyToEcho +
							MCR_KEY_GET_UP_TYPE ( & key ),
							& MCR_KEY_GET ( & key ) ) ;
					if ( echoFound )
					{
						MCR_ECHO_SET ( & echo, * echoFound ) ;
						if ( disp ( echosig ) )
						{
							key_setall ( & key, events [ i ].code, 0,
								events [ i ].value ? MCR_DOWN : MCR_UP ) ;
							break ;
						}
					}
					MCR_SEND ( & keysig ) ;
					key_setall ( & key, events [ i ].code, 0,
							events [ i ].value ? MCR_DOWN : MCR_UP ) ;
				}
				else
				{
					MCR_KEY_SET ( & key, events [ i ].code ) ;
					MCR_KEY_SET_UP_TYPE ( & key,
							events [ i ].value ? MCR_DOWN :
							MCR_UP ) ;
				}
				break ;
			case EV_ABS :	/* Handle ABS */
				abspos ( events [ i ].code, pos ) ;
				if ( bAbs [ pos ] )
				{
					abssetcurrent ( & abs, bAbs ) ;
					MCR_SEND ( & abssig ) ;
					memset ( bAbs, 0, sizeof ( bAbs ) ) ;
				}
				MCR_MOVECURSOR_SET_POSITION ( & abs, pos,
						events [ i ].value ) ;
				++ bAbs [ pos ] ;
				break ;
			case EV_REL :
				relpos ( events [ i ].code, pos ) ;
				if ( events [ i ].code == REL_X || events [ i ].code
						== REL_Y || events [ i ].code == REL_Z )
				{
					if ( MCR_MOVECURSOR_GET_POSITION ( & rel, pos ) )
					{
						MCR_SEND ( & relsig ) ;
						MCR_MOVECURSOR_SET ( & rel, nopos ) ;
					}
					MCR_MOVECURSOR_SET_POSITION ( & rel, pos,
							events [ i ].value ) ;
				}
				// Currently assuming scroll if not relative movement.
				else
				{
					if ( MCR_SCROLL_GET_DIMENSION ( & scr, pos ) )
					{
						MCR_SEND ( & scrsig ) ;
						MCR_SCROLL_SET ( & scr, nopos ) ;
					}
					MCR_SCROLL_SET_DIMENSION ( & scr, pos,
							events [ i ].value ) ;
				}
				break ;
			}
		}
		//Call final event, it was not called yet.
		if ( MCR_KEY_GET ( & key ) || MCR_KEY_GET_SCAN ( & key ) )
		{
			if ( MCR_KEY_GET ( & key ) )
			{
				echoFound = MCR_MAP_GET_VALUE ( mcr_keyToEcho +
						MCR_KEY_GET_UP_TYPE ( & key ),
						& MCR_KEY_GET ( & key ) ) ;
				if ( echoFound )
				{
					MCR_ECHO_SET ( & echo, * echoFound ) ;
					if ( ! disp ( echosig ) )
					{
						MCR_SEND ( & keysig ) ;
					}
				}
				else
				{
					MCR_SEND ( & keysig ) ;
				}
			}
			key_setall ( & key, 0, 0, MCR_BOTH ) ;
		}
		for ( i = 0 ; i < MCR_DIMENSION_CNT ; i ++ )
		{
			if ( bAbs [ i ] )
			{
				abssetcurrent ( & abs, bAbs ) ;
				MCR_SEND ( & abssig ) ;
				memset ( bAbs, 0, sizeof ( bAbs ) ) ;
				break ;
			}
		}
		for ( i = 0 ; i < MCR_DIMENSION_CNT ; i ++ )
		{
			if ( MCR_MOVECURSOR_GET_POSITION ( & rel, i ) )
			{
				MCR_SEND ( & relsig ) ;
				MCR_MOVECURSOR_SET ( & rel, nopos ) ;
				break ;
			}
		}
		for ( i = 0 ; i < MCR_DIMENSION_CNT ; i ++ )
		{
			if ( MCR_SCROLL_GET_DIMENSION ( & scr, i ) )
			{
				MCR_SEND ( & scrsig ) ;
				MCR_SCROLL_SET ( & scr, nopos ) ;
				break ;
			}
		}
	}
	return thrd_success ;
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
	mcr_Mod_key_get_all ( ( int * ) keyBuffer.array, keyBuffer.used ) ;
	int * keyArr = ( int * ) keyBuffer.array ;
	for ( unsigned int i = 0 ; i < keyBuffer.used ; i++ )
	{
		if ( ( keybitValues [ MCR_EVENTINDEX ( keyArr [ i ] ) ] &
				MCR_EVENTBIT ( keyArr [ i ] ) ) )
			modVal |= mcr_Mod_key_get ( keyArr [ i ] ) ;
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
	int i = 0 ;
	mtx_init ( & _enableLock, mtx_plain ) ;
	cnd_init ( & _countCnd ) ;
	// Fatal if path to grab map cannot be allocated.
	_grabMapPt = malloc ( sizeof ( mcr_Map ) ) ;
	while ( ! _grabMapPt && i ++ < 10 )
	{
		dmsg ;
		sleep ( 1 ) ;
		_grabMapPt = malloc ( sizeof ( mcr_Map ) ) ;
	}
	if ( ! _grabMapPt )
	{
		dmsg ;
		exit ( thrd_nomem ) ;
	}
	mcr_Map_init ( _grabMapPt, sizeof ( const char * ),
			sizeof ( mcr_Grabber * ) ) ;
	_grabMapPt->compare = mcr_str_compare ;
	mcr_Array_init ( & _grabPaths, sizeof ( mcr_Array ) ) ;
}

static void grabdisable_redirect ( mcr_Grabber ** grabPtPt, ... )
{
	mcr_Grabber_enable ( * grabPtPt, 0 ) ;
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
	dassert ( _grabMapPt ) ;
	// Disable all, wait for all batch operation.
	MCR_MAP_FOR_EACH_VALUE ( _grabMapPt, grabdisable_redirect, 0 ) ;
	size_t prev = _grabMapPt->set.used ;
	while ( _grabMapPt->set.used )
	{
		// Disable any that are being created while clearing.
		if ( _grabMapPt->set.used >= prev )
		{
			mcr_Grabber ** grabPtPt = MCR_MAP_VALUE ( _grabMapPt,
					MCR_ARR_AT ( & _grabMapPt->set, 0 ) ) ;
			mcr_Grabber_enable ( * grabPtPt, 0 ) ;
		}
		prev = _grabMapPt->set.used ;
		cnd_wait ( & _countCnd, & _enableLock ) ;
	}
}
void mcr_intercept_native_cleanup ( void )
{
	dassert ( _grabMapPt ) ;
	mtx_lock ( & _enableLock ) ;
	clear_grabbers_impl ( ) ;
	MCR_ARR_FOR_EACH ( & _grabPaths, mcr_Array_free_foreach, 0 ) ;
	mcr_Array_free ( & _grabPaths ) ;
	// Grabbers will now be empty.
	mcr_Map_free ( _grabMapPt ) ;
	free ( _grabMapPt ) ;
	_grabMapPt = NULL ;
	mtx_unlock ( & _enableLock ) ;
	mtx_destroy ( & _enableLock ) ;
	cnd_destroy ( & _countCnd ) ;
}
