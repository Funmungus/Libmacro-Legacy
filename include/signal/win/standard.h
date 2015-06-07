/* include/signal/win/standard.h - Windows definitions for standard signals.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# ifndef MCR_WIN_STANDARD_H
# define MCR_WIN_STANDARD_H

# include "signal/standard.h"
# include "signal/win/def.h"

// dynamic int [ ] for mcr_HIDEcho MOUSEEVENTF
extern mcr_Array mcr_echoEvents ;

typedef struct mcr_HIDEcho
{
	int event ;
} mcr_HIDEcho ;

typedef struct mcr_Key
{
	int key ;
	int scan ;
	mcr_KeyUpType key_up ;
} mcr_Key ;

typedef struct mcr_MoveCursor
{
	mcr_SpacePosition pos ;
	int cursor_justify ;
} mcr_MoveCursor ;

typedef struct mcr_Scroll
{
	mcr_Dimensions dm ;
} mcr_Scroll ;

MCR_API int mcr_Echo_set_mouseflag ( int echoCode, int mouseEventFlags ) ;

# undef MCR_ECHO_GET
# undef MCR_ECHO_SET
# undef MCR_ECHO_SEND
# undef MCR_ECHO_QUICKSEND
# define MCR_ECHO_GET( echoPt ) \
	( echoPt )->event
# define MCR_ECHO_SET( echoPt, code ) \
	( echoPt )->event = ( code )
# define MCR_ECHO_QUICKSEND( echoPt ) \
	if ( ( ( size_t ) ( ( echoPt )->event ) ) < mcr_EchoEvents.used ) \
	{ \
		mouse_event ( * ( int * ) MCR_ARR_AT ( & mcr_EchoEvents, \
				( echoPt )->event ), 0, 0, 0, 0 ) ; \
	}
# define MCR_ECHO_SEND( echoPt, success ) MCR_ECHO_QUICKSEND(echoPt)

# undef MCR_KEY_GET
# undef MCR_KEY_SET
# undef MCR_KEY_GET_SCAN
# undef MCR_KEY_SET_SCAN
# undef MCR_KEY_GET_UP_TYPE
# undef MCR_KEY_SET_UP_TYPE
# undef MCR_KEY_QUICKSEND
# undef MCR_KEY_SEND
# define MCR_KEY_GET( keyPt ) \
	( keyPt )->key
# define MCR_KEY_SET( keyPt, _key_ ) \
	( keyPt )->key = ( _key_ )
# define MCR_KEY_GET_SCAN( keyPt ) \
	( keyPt )->scan
# define MCR_KEY_SET_SCAN( keyPt, _scan_ ) \
	( keyPt )->scan = ( _scan_ )
# define MCR_KEY_GET_UP_TYPE( keyPt ) \
	( keyPt )->key_up
# define MCR_KEY_SET_UP_TYPE( keyPt, _key_up_ ) \
	( keyPt )->key_up = ( _key_up_ )
# define MCR_KEY_QUICKSEND( keyPt ) \
	if ( ( keyPt )->key_up != MCR_UP ) \
	{ \
		keybd_event ( ( BYTE ) ( keyPt )->key, ( BYTE ) ( keyPt )->scan, \
				KEYEVENTF_EXTENDEDKEY, 0 ) ; \
	} \
	if ( ( keyPt )->key_up != MCR_DOWN ) \
	{ \
		keybd_event ( ( BYTE ) ( keyPt )->key, ( BYTE ) ( keyPt )->scan, \
				KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 ) ; \
	}
# define MCR_KEY_SEND( keyPt, success ) MCR_KEY_QUICKSEND(keyPt)

# undef MCR_MOVECURSOR_GET
# undef MCR_MOVECURSOR_SET
# undef MCR_MOVECURSOR_GET_POSITION
# undef MCR_MOVECURSOR_SET_POSITION
# undef MCR_MOVECURSOR_IS_JUSTIFY
# undef MCR_MOVECURSOR_ENABLE_JUSTIFY
# undef MCR_MOVECURSOR_SEND
# undef MCR_MOVECURSOR_QUICKSEND
# define MCR_MOVECURSOR_GET( mcPt, buffer ) \
	( buffer ) [ MCR_X ] = ( mcPt )->pos [ MCR_X ] ; \
	( buffer ) [ MCR_Y ] = ( mcPt )->pos [ MCR_Y ] ; \
	( buffer ) [ MCR_Z ] = ( mcPt )->pos [ MCR_Z ] ;
# define MCR_MOVECURSOR_SET( mcPt, buffer ) \
	( mcPt )->pos [ MCR_X ] = ( buffer ) [ MCR_X ] ; \
	( mcPt )->pos [ MCR_Y ] = ( buffer ) [ MCR_Y ] ; \
	( mcPt )->pos [ MCR_Z ] = ( buffer ) [ MCR_Z ] ;
# define MCR_MOVECURSOR_GET_POSITION( mcPt, _pos_ ) \
	( mcPt )->pos [ _pos_ ]
# define MCR_MOVECURSOR_SET_POSITION( mcPt, _pos_, value ) \
	( mcPt )->pos [ _pos_ ] = value ;
# define MCR_MOVECURSOR_IS_JUSTIFY( mcPt ) \
	( mcPt )->cursor_justify
# define MCR_MOVECURSOR_ENABLE_JUSTIFY( mcPt, enable ) \
	( mcPt )->cursor_justify = enable
# define MCR_MOVECURSOR_QUICKSEND( mcPt ) \
	mouse_event ( ( mcPt )->cursor_justify ? \
			MOUSEEVENTF_MOVE : MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, \
			( DWORD ) ( mcPt )->pos [ MCR_X ], \
			( DWORD ) ( mcPt )->pos [ MCR_Y ], 0, 0 ) ;
# define MCR_MOVECURSOR_SEND( mcPt, success ) \
	MCR_MOVECURSOR_QUICKSEND ( mcPt )

# undef MCR_SCROLL_GET
# undef MCR_SCROLL_SET
# undef MCR_SCROLL_GET_DIMENSION
# undef MCR_SCROLL_SET_DIMENSION
# undef MCR_SCROLL_SEND
# undef MCR_SCROLL_QUICKSEND
# define MCR_SCROLL_GET( scrollPt, buffer ) \
	( buffer ) [ MCR_X ] = ( scrollPt )->dm [ MCR_X ] ; \
	( buffer ) [ MCR_Y ] = ( scrollPt )->dm [ MCR_Y ] ; \
	( buffer ) [ MCR_Z ] = ( scrollPt )->dm [ MCR_Z ] ;
# define MCR_SCROLL_SET( scrollPt, buffer ) \
	( scrollPt )->dm [ MCR_X ] = ( buffer ) [ MCR_X ] ; \
	( scrollPt )->dm [ MCR_Y ] = ( buffer ) [ MCR_Y ] ; \
	( scrollPt )->dm [ MCR_Z ] = ( buffer ) [ MCR_Z ] ;
# define MCR_SCROLL_GET_DIMENSION( scrollPt, pos ) \
	( scrollPt )->dm [ pos ]
# define MCR_SCROLL_SET_DIMENSION( scrollPt, pos, val ) \
	( scrollPt )->dm [ pos ] = val
# define MCR_SCROLL_QUICKSEND( scrollPt ) \
	mouse_event ( MOUSEEVENTF_WHEEL, \
		0, 0, ( DWORD ) ( scrollPt )->dm [ MCR_Y ], 0 ) ; \
	mouse_event ( MOUSEEVENTF_HWHEEL, \
		0, 0, ( DWORD ) ( scrollPt )->dm [ MCR_X ], 0 ) ;
# define MCR_SCROLL_SEND( scrollPt, success ) \
	MCR_SCROLL_QUICKSEND ( scrollPt )

# endif // MCR_WIN_STANDARD_H
