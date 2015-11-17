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

# ifndef MCR_WIN_STANDARD_H
# define MCR_WIN_STANDARD_H

# include "signal/standard.h"
# include "signal/win/def.h"

// dynamic int [ ] for mcr_HIDEcho MOUSEEVENTF
MCR_API extern mcr_Array mcr_echoEvents ;

typedef struct mcr_HIDEcho
{
	int event ;
} mcr_HIDEcho ;

typedef struct mcr_Key
{
	int key ;
	int scan ;
	mcr_KeyUpType up_type ;
} mcr_Key ;

typedef struct mcr_MoveCursor
{
	mcr_SpacePosition pos ;
	int justify ;
} mcr_MoveCursor ;

typedef struct mcr_Scroll
{
	mcr_Dimensions dm ;
} mcr_Scroll ;

MCR_API int mcr_Echo_set_mouseflag ( int echoCode, int mouseEventFlags ) ;

# undef MCR_ECHO_ECHO
# undef MCR_ECHO_SET_ECHO
# undef MCR_ECHO_SEND
# undef MCR_ECHO_QUICKSEND
# define MCR_ECHO_ECHO( echo ) \
	( echo ).event
# define MCR_ECHO_SET_ECHO( echo, code ) \
	( echo ).event = ( code )
# define MCR_ECHO_QUICKSEND( echo ) \
if ( ( ( size_t ) ( ( echo ).event ) ) < mcr_echoEvents.used ) \
{ \
	mouse_event ( * ( int * ) MCR_ARR_AT ( mcr_echoEvents, \
			( echo ).event ), 0, 0, 0, 0 ) ; \
}
# define MCR_ECHO_SEND( echo, success ) MCR_ECHO_QUICKSEND ( echo )

# undef MCR_KEY_KEY
# undef MCR_KEY_SET_KEY
# undef MCR_KEY_SCAN
# undef MCR_KEY_SET_SCAN
# undef MCR_KEY_UP_TYPE
# undef MCR_KEY_SET_UP_TYPE
# undef MCR_KEY_QUICKSEND
# undef MCR_KEY_SEND
# define MCR_KEY_KEY( keyObj ) \
( keyObj ).key
# define MCR_KEY_SET_KEY( keyObj, keyCode ) \
( keyObj ).key = ( keyCode )
# define MCR_KEY_SCAN( keyObj ) \
( keyObj ).scan
# define MCR_KEY_SET_SCAN( keyObj, scanCode ) \
( keyObj ).scan = ( scanCode )
# define MCR_KEY_UP_TYPE( keyObj ) \
( keyObj ).up_type
# define MCR_KEY_SET_UP_TYPE( keyObj, upType ) \
( keyObj ).up_type = ( upType )
# define MCR_KEY_QUICKSEND( keyObj ) \
if ( ( keyObj ).up_type != MCR_UP ) \
{ \
	keybd_event ( ( BYTE ) ( keyObj ).key, ( BYTE ) ( keyObj ).scan, \
			KEYEVENTF_EXTENDEDKEY, 0 ) ; \
} \
if ( ( keyObj ).up_type != MCR_DOWN ) \
{ \
	keybd_event ( ( BYTE ) ( keyObj ).key, ( BYTE ) ( keyObj ).scan, \
			KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0 ) ; \
}
# define MCR_KEY_SEND( keyObj, success ) MCR_KEY_QUICKSEND ( keyObj )

# undef MCR_MOVECURSOR_POSITION
# undef MCR_MOVECURSOR_SET_POSITION
# undef MCR_MOVECURSOR_COORDINATE
# undef MCR_MOVECURSOR_SET_COORDINATE
# undef MCR_MOVECURSOR_JUSTIFY
# undef MCR_MOVECURSOR_SET_JUSTIFY
# undef MCR_MOVECURSOR_SEND
# undef MCR_MOVECURSOR_QUICKSEND
# define MCR_MOVECURSOR_POSITION( mc, buffer ) \
( buffer ) [ MCR_X ] = ( mc ).pos [ MCR_X ] ; \
( buffer ) [ MCR_Y ] = ( mc ).pos [ MCR_Y ] ; \
( buffer ) [ MCR_Z ] = ( mc ).pos [ MCR_Z ] ;
# define MCR_MOVECURSOR_SET_POSITION( mc, buffer ) \
( mc ).pos [ MCR_X ] = ( buffer ) [ MCR_X ] ; \
( mc ).pos [ MCR_Y ] = ( buffer ) [ MCR_Y ] ; \
( mc ).pos [ MCR_Z ] = ( buffer ) [ MCR_Z ] ;
# define MCR_MOVECURSOR_COORDINATE( mc, _pos_ ) \
( mc ).pos [ _pos_ ]
# define MCR_MOVECURSOR_SET_COORDINATE( mc, _pos_, value ) \
( mc ).pos [ _pos_ ] = value ;
# define MCR_MOVECURSOR_JUSTIFY( mc ) \
( mc ).justify
# define MCR_MOVECURSOR_SET_JUSTIFY( mc, enable ) \
( mc ).justify = enable
# define MCR_MOVECURSOR_QUICKSEND( mc ) \
mouse_event ( ( mc ).justify ? MOUSEEVENTF_MOVE : \
		MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, \
		( DWORD ) ( mc ).pos [ MCR_X ], \
		( DWORD ) ( mc ).pos [ MCR_Y ], 0, 0 ) ;
# define MCR_MOVECURSOR_SEND( mc, success ) \
MCR_MOVECURSOR_QUICKSEND ( mc )

# undef MCR_SCROLL_DIMENSIONS
# undef MCR_SCROLL_SET_DIMENSIONS
# undef MCR_SCROLL_COORDINATE
# undef MCR_SCROLL_SET_COORDINATE
# undef MCR_SCROLL_SEND
# undef MCR_SCROLL_QUICKSEND
# define MCR_SCROLL_DIMENSIONS( scroll, buffer ) \
( buffer ) [ MCR_X ] = ( scroll ).dm [ MCR_X ] ; \
( buffer ) [ MCR_Y ] = ( scroll ).dm [ MCR_Y ] ; \
( buffer ) [ MCR_Z ] = ( scroll ).dm [ MCR_Z ] ;
# define MCR_SCROLL_SET_DIMENSIONS( scroll, buffer ) \
( scroll ).dm [ MCR_X ] = ( buffer ) [ MCR_X ] ; \
( scroll ).dm [ MCR_Y ] = ( buffer ) [ MCR_Y ] ; \
( scroll ).dm [ MCR_Z ] = ( buffer ) [ MCR_Z ] ;
# define MCR_SCROLL_COORDINATE( scroll, pos ) \
( scroll ).dm [ pos ]
# define MCR_SCROLL_SET_COORDINATE( scroll, pos, val ) \
( scroll ).dm [ pos ] = val
# define MCR_SCROLL_QUICKSEND( scroll ) \
mouse_event ( MOUSEEVENTF_WHEEL, \
		0, 0, ( DWORD ) ( scroll ).dm [ MCR_Y ], 0 ) ; \
mouse_event ( MOUSEEVENTF_HWHEEL, \
		0, 0, ( DWORD ) ( scroll ).dm [ MCR_X ], 0 ) ;
# define MCR_SCROLL_SEND( scroll, success ) \
MCR_SCROLL_QUICKSEND ( scroll )

# endif // MCR_WIN_STANDARD_H
