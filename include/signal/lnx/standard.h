
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
# define MCR_KEY_SEND( keyPt, success ) \
	if ( ( keyPt )->key_up != MCR_UP ) \
	{ \
		( keyPt )->events [ 1 ].value = 1 ; \
		MCR_DEV_SEND ( mcr_KeyDev, ( keyPt )->events, \
				sizeof ( ( keyPt )->events ), success ) ; \
	} \
	if ( success && ( keyPt )->key_up != MCR_DOWN ) \
	{ \
		( keyPt )->events [ 1 ].value = 0 ; \
		MCR_DEV_SEND ( mcr_KeyDev, ( keyPt )->events, \
				sizeof ( ( keyPt )->events ), success ) ; \
	}

// mcr_HIDEcho
# undef MCR_ECHO_GET
# undef MCR_ECHO_SET
# undef MCR_ECHO_SEND
# define MCR_ECHO_GET( echoPt ) \
	( echoPt )->event
# define MCR_ECHO_SET( echoPt, code ) \
	( echoPt )->event = ( code )
# define MCR_ECHO_SEND( echoPt, success ) \
	if ( ( size_t ) ( ( echoPt )->event ) < mcr_EchoEvents.used ) \
	{ \
		MCR_KEY_SEND ( ( mcr_Key * ) MCR_ARR_AT ( & mcr_EchoEvents , \
				( unsigned int ) ( echoPt )->event ), success ) \
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
# define MCR_MOVECURSOR_SEND( mcPt, success ) \
	if ( ( mcPt )->cursor_justify ) \
	{ \
		MCR_DEV_SEND ( mcr_RelDev, ( mcPt )->relvent, \
				sizeof ( ( mcPt )->relvent ), success ) ; \
	} \
	else \
	{ \
		MCR_DEV_SEND ( mcr_AbsDev, ( mcPt )->absvent, \
				sizeof ( ( mcPt )->absvent ), success ) ; \
	}

// mcr_Scroll
# undef MCR_SCROLL_GET
# undef MCR_SCROLL_SET
# undef MCR_SCROLL_GET_DIMENSION
# undef MCR_SCROLL_SET_DIMENSION
# undef MCR_SCROLL_SEND
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
	MCR_DEV_SEND ( mcr_RelDev, ( scrollPt )->events, \
			sizeof ( ( scrollPt )->events ), success ) ;

# endif // MCR_LNX_STANDARD_H
