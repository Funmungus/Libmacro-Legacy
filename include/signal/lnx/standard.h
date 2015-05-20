/* include/signal/lnx/standard.h - Linux definitions of standard signals.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# ifndef MCR_LNX_STANDARD_H
# define MCR_LNX_STANDARD_H

# include "signal/standard.h"
# include "signal/lnx/device.h"

// In order to inline we cannot privatize this.
extern mcr_Array mcr_EchoEvents ;

typedef struct mcr_HIDEcho
{
	int event ;
} mcr_HIDEcho ;

typedef struct mcr_Key
{
	// scan, key, sync
	struct input_event events [ 3 ] ;
	mcr_KeyUpType key_up ;
} mcr_Key ;

typedef struct mcr_MoveCursor
{
	// x, y, z, sync
	// Relative with coordinates.
	struct input_event relvent [ MCR_DIMENSION_CNT + 1 ] ;
	// Absolute with coordinates.
	struct input_event absvent [ MCR_DIMENSION_CNT + 1 ] ;
	int cursor_justify ;
} mcr_MoveCursor ;

typedef struct mcr_Scroll
{
	// x, y, z, sync
	struct input_event
		events [ MCR_DIMENSION_CNT + 1 ] ;
} mcr_Scroll ;

//extern mcr_Array events ;

// Key sending is needed in other macros.
# undef MCR_KEY_SEND
# undef MCR_KEY_QUICKSEND
# define MCR_KEY_QUICKSEND( keyPt ) \
	if ( ( keyPt )->key_up != MCR_UP ) \
	{ \
		( keyPt )->events [ 1 ].value = 1 ; \
		MCR_DEV_QUICKSEND ( mcr_keyDev, ( keyPt )->events, \
				sizeof ( ( keyPt )->events ) ) ; \
	} \
	if ( ( keyPt )->key_up != MCR_DOWN ) \
	{ \
		( keyPt )->events [ 1 ].value = 0 ; \
		MCR_DEV_QUICKSEND ( mcr_keyDev, ( keyPt )->events, \
				sizeof ( ( keyPt )->events ) ) ; \
	}

# define MCR_KEY_SEND( keyPt, success ) \
	if ( ( keyPt )->key_up != MCR_UP ) \
	{ \
		( keyPt )->events [ 1 ].value = 1 ; \
		MCR_DEV_SEND ( mcr_keyDev, ( keyPt )->events, \
				sizeof ( ( keyPt )->events ), success ) ; \
	} \
	if ( success && ( keyPt )->key_up != MCR_DOWN ) \
	{ \
		( keyPt )->events [ 1 ].value = 0 ; \
		MCR_DEV_SEND ( mcr_keyDev, ( keyPt )->events, \
				sizeof ( ( keyPt )->events ), success ) ; \
	}

// mcr_HIDEcho
# undef MCR_ECHO_GET
# undef MCR_ECHO_SET
# undef MCR_ECHO_SEND
# undef MCR_ECHO_QUICKSEND
# define MCR_ECHO_GET( echoPt ) \
	( echoPt )->event
# define MCR_ECHO_SET( echoPt, code ) \
	( echoPt )->event = ( code )
# define MCR_ECHO_SEND( echoPt, success ) \
	if ( ( size_t ) ( ( echoPt )->event ) < mcr_EchoEvents.used ) \
	{ \
		MCR_KEY_SEND ( ( mcr_Key * ) MCR_ARR_AT ( & mcr_EchoEvents, \
				( unsigned int ) ( echoPt )->event ), success ) \
	} \
	else \
	{ \
		dmsg ( "Event out of range.\n" ) ; \
	}

# define MCR_ECHO_QUICKSEND( echoPt ) \
	if ( ( size_t ) ( ( echoPt )->event ) < mcr_EchoEvents.used ) \
	{ \
		MCR_KEY_QUICKSEND ( ( mcr_Key * ) MCR_ARR_AT ( & mcr_EchoEvents, \
				( unsigned int ) ( echoPt )->event ) ) \
	} \
	else \
	{ \
		dmsg ( "Event out of range.\n" ) ; \
	}

// mcr_Key
# undef MCR_KEY_GET
# undef MCR_KEY_SET
# undef MCR_KEY_GET_SCAN
# undef MCR_KEY_SET_SCAN
# undef MCR_KEY_GET_UP_TYPE
# undef MCR_KEY_SET_UP_TYPE
# define MCR_KEY_GET( keyPt ) \
	( keyPt )->events [ 1 ].code
# define MCR_KEY_SET( keyPt, key ) \
	( keyPt )->events [ 1 ].code = ( key )
# define MCR_KEY_GET_SCAN( keyPt ) \
	( keyPt )->events [ 0 ].value
# define MCR_KEY_SET_SCAN( keyPt, scan ) \
	( keyPt )->events [ 0 ].value = ( scan )
# define MCR_KEY_GET_UP_TYPE( keyPt ) \
	( keyPt )->key_up
# define MCR_KEY_SET_UP_TYPE( keyPt, keyUp ) \
	( keyPt )->key_up = ( keyUp )

// mcr_MoveCursor
# undef MCR_MOVECURSOR_GET
# undef MCR_MOVECURSOR_SET
# undef MCR_MOVECURSOR_GET_POSITION
# undef MCR_MOVECURSOR_SET_POSITION
# undef MCR_MOVECURSOR_IS_JUSTIFY
# undef MCR_MOVECURSOR_ENABLE_JUSTIFY
# undef MCR_MOVECURSOR_SEND
# undef MCR_MOVECURSOR_QUICKSEND
# define MCR_MOVECURSOR_GET( mcPt, buffer ) \
	( buffer ) [ MCR_X ] = ( mcPt )->relvent [ MCR_X ].value ; \
	( buffer ) [ MCR_Y ] = ( mcPt )->relvent [ MCR_Y ].value ; \
	( buffer ) [ MCR_Z ] = ( mcPt )->relvent [ MCR_Z ].value ;
# define MCR_MOVECURSOR_SET( mcPt, buffer ) \
	( mcPt )->relvent [ MCR_X ].value = \
	( mcPt )->absvent [ MCR_X ].value = ( buffer ) [ MCR_X ] ; \
	( mcPt )->relvent [ MCR_Y ].value = \
	( mcPt )->absvent [ MCR_Y ].value = ( buffer ) [ MCR_Y ] ; \
	( mcPt )->relvent [ MCR_Z ].value = \
	( mcPt )->absvent [ MCR_Z ].value = ( buffer ) [ MCR_Z ] ;
# define MCR_MOVECURSOR_GET_POSITION( mcPt, posType ) \
	( mcPt )->relvent [ posType ].value
# define MCR_MOVECURSOR_SET_POSITION( mcPt, posType, val ) \
	( mcPt )->relvent [ posType ].value = val ; \
	( mcPt )->absvent [ posType ].value = val ;
# define MCR_MOVECURSOR_IS_JUSTIFY( mcPt ) \
	( mcPt )->cursor_justify
# define MCR_MOVECURSOR_ENABLE_JUSTIFY( mcPt, enable ) \
	( mcPt )->cursor_justify = enable
# define MCR_MOVECURSOR_JUSTIFY_IMPL( mcPt ) \
	if ( ( mcPt )->cursor_justify ) \
	{ \
		mcr_cursor [ MCR_X ] += ( mcPt )->relvent [ MCR_X ].value ; \
		mcr_cursor [ MCR_Y ] += ( mcPt )->relvent [ MCR_Y ].value ; \
		mcr_cursor [ MCR_Z ] += ( mcPt )->relvent [ MCR_Z ].value ; \
		if ( mcr_cursor [ MCR_X ] > MCR_ABS_RESOLUTION ) \
			mcr_cursor [ MCR_X ] = MCR_ABS_RESOLUTION ; \
		else if ( mcr_cursor [ MCR_X ] < 0 ) \
			mcr_cursor [ MCR_X ] = 0 ; \
		if ( mcr_cursor [ MCR_Y ] > MCR_ABS_RESOLUTION ) \
			mcr_cursor [ MCR_Y ] = MCR_ABS_RESOLUTION ; \
		else if ( mcr_cursor [ MCR_Y ] < 0 ) \
			mcr_cursor [ MCR_Y ] = 0 ; \
		if ( mcr_cursor [ MCR_Z ] > MCR_ABS_RESOLUTION ) \
			mcr_cursor [ MCR_Z ] = MCR_ABS_RESOLUTION ; \
		else if ( mcr_cursor [ MCR_Z ] < 0 ) \
			mcr_cursor [ MCR_Z ] = 0 ; \
	}

# define MCR_MOVECURSOR_SEND( mcPt, success ) \
	if ( ( mcPt )->cursor_justify ) \
	{ \
		MCR_DEV_SEND ( mcr_relDev, ( mcPt )->relvent, \
				sizeof ( ( mcPt )->relvent ), success ) ; \
		MCR_MOVECURSOR_JUSTIFY_IMPL ( mcPt ) ; \
	} \
	else \
	{ \
		MCR_DEV_SEND ( mcr_absDev, ( mcPt )->absvent, \
				sizeof ( ( mcPt )->absvent ), success ) ; \
		mcr_cursor [ MCR_X ] = ( mcPt )->absvent [ MCR_X ].value ; \
		mcr_cursor [ MCR_Y ] = ( mcPt )->absvent [ MCR_Y ].value ; \
		mcr_cursor [ MCR_Z ] = ( mcPt )->absvent [ MCR_Z ].value ; \
	}

# define MCR_MOVECURSOR_QUICKSEND( mcPt ) \
	if ( ( mcPt )->cursor_justify ) \
	{ \
		MCR_DEV_QUICKSEND ( mcr_relDev, ( mcPt )->relvent, \
				sizeof ( ( mcPt )->relvent ) ) ; \
		MCR_MOVECURSOR_JUSTIFY_IMPL ( mcPt ) ; \
	} \
	else \
	{ \
		MCR_DEV_QUICKSEND ( mcr_absDev, ( mcPt )->absvent, \
				sizeof ( ( mcPt )->absvent ) ) ; \
		mcr_cursor [ MCR_X ] = ( mcPt )->absvent [ MCR_X ].value ; \
		mcr_cursor [ MCR_Y ] = ( mcPt )->absvent [ MCR_Y ].value ; \
		mcr_cursor [ MCR_Z ] = ( mcPt )->absvent [ MCR_Z ].value ; \
	}

// mcr_Scroll
# undef MCR_SCROLL_GET
# undef MCR_SCROLL_SET
# undef MCR_SCROLL_GET_DIMENSION
# undef MCR_SCROLL_SET_DIMENSION
# undef MCR_SCROLL_SEND
# undef MCR_SCROLL_QUICKSEND
# define MCR_SCROLL_GET( scrollPt, buffer ) \
	( buffer ) [ MCR_X ] = ( scrollPt )->events [ MCR_X ].value ; \
	( buffer ) [ MCR_Y ] = ( scrollPt )->events [ MCR_Y ].value ; \
	( buffer ) [ MCR_Z ] = ( scrollPt )->events [ MCR_Z ].value ;
# define MCR_SCROLL_SET( scrollPt, buffer ) \
	( scrollPt )->events [ MCR_X ].value = ( buffer ) [ MCR_X ] ; \
	( scrollPt )->events [ MCR_Y ].value = ( buffer ) [ MCR_Y ] ; \
	( scrollPt )->events [ MCR_Z ].value = ( buffer ) [ MCR_Z ] ;
# define MCR_SCROLL_GET_DIMENSION( scrollPt, pos ) \
	( scrollPt )->events [ pos ].value
# define MCR_SCROLL_SET_DIMENSION( scrollPt, pos, val ) \
	( scrollPt )->events [ pos ].value = val
# define MCR_SCROLL_SEND( scrollPt, success ) \
	MCR_DEV_SEND ( mcr_relDev, ( scrollPt )->events, \
			sizeof ( ( scrollPt )->events ), success ) ;
# define MCR_SCROLL_QUICKSEND( scrollPt ) \
	MCR_DEV_QUICKSEND ( mcr_relDev, ( scrollPt )->events, \
			sizeof ( ( scrollPt )->events ) ) ;

# endif // MCR_LNX_STANDARD_H
