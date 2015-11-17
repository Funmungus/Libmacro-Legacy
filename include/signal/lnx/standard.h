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

# ifndef MCR_LNX_STANDARD_H
# define MCR_LNX_STANDARD_H

# include "signal/standard.h"
# include "signal/lnx/device.h"
# ifdef MCR_USE_X
# include "signal/lnx/usex.h"
# endif

// In order to inline we cannot privatize this.
extern MCR_API mcr_Array mcr_echoEvents ;
extern MCR_API mcr_Map mcr_keyToEcho [ 2 ] ;
extern MCR_API mcr_SpacePosition mcr_cursor ;

typedef struct mcr_HIDEcho
{
	int event ;
} mcr_HIDEcho ;

typedef struct mcr_Key
{
	// scan, key, sync
	struct input_event events [ 3 ] ;
	mcr_KeyUpType up_type ;
} mcr_Key ;

typedef struct mcr_MoveCursor
{
	// x, y, z, sync
	// Relative with coordinates.
	struct input_event relvent [ MCR_DIMENSION_CNT + 1 ] ;
	// Absolute with coordinates.
	struct input_event absvent [ MCR_DIMENSION_CNT + 1 ] ;
	int justify ;
} mcr_MoveCursor ;

typedef struct mcr_Scroll
{
	// x, y, z, sync
	struct input_event
		events [ MCR_DIMENSION_CNT + 1 ] ;
} mcr_Scroll ;

MCR_API int mcr_Echo_set_key ( int echoCode, mcr_Key * keyPt ) ;

//extern mcr_Array events ;

// Key sending is needed in other macros.
# undef MCR_KEY_SEND
# undef MCR_KEY_QUICKSEND
# define MCR_KEY_QUICKSEND( key ) \
if ( ( key ).up_type != MCR_UP ) \
{ \
	( key ).events [ 1 ].value = 1 ; \
	MCR_DEV_SEND ( mcr_genDev, ( key ).events, \
			sizeof ( ( key ).events ) ) ; \
} \
if ( ( key ).up_type != MCR_DOWN ) \
{ \
	( key ).events [ 1 ].value = 0 ; \
	MCR_DEV_SEND ( mcr_genDev, ( key ).events, \
			sizeof ( ( key ).events ) ) ; \
}

# define MCR_KEY_SEND( key, success ) \
if ( ( key ).up_type != MCR_UP ) \
{ \
	( key ).events [ 1 ].value = 1 ; \
	if ( ! MCR_DEV_SEND ( mcr_genDev, ( key ).events, \
			sizeof ( ( key ).events ) ) ) \
		success = 0 ; \
} \
if ( ( key ).up_type != MCR_DOWN ) \
{ \
	( key ).events [ 1 ].value = 0 ; \
	if ( ! MCR_DEV_SEND ( mcr_genDev, ( key ).events, \
			sizeof ( ( key ).events ) ) ) \
		success = 0 ; \
}

// mcr_HIDEcho
# undef MCR_ECHO_ECHO
# undef MCR_ECHO_SET_ECHO
# undef MCR_ECHO_SEND
# undef MCR_ECHO_QUICKSEND
# define MCR_ECHO_ECHO( echo ) \
( echo ).event
# define MCR_ECHO_SET_ECHO( echo, code ) \
( echo ).event = ( code )
# define MCR_ECHO_SEND( echo, success ) \
if ( ( unsigned ) ( ( echo ).event ) < mcr_echoEvents.used ) \
{ \
	MCR_KEY_SEND ( * ( mcr_Key * ) MCR_ARR_AT ( mcr_echoEvents, \
			( unsigned ) ( echo ).event ), success ) \
} \
else \
	dmsg ; \

# define MCR_ECHO_QUICKSEND( echo ) \
if ( ( unsigned ) ( ( echo ).event ) < mcr_echoEvents.used ) \
{ \
	MCR_KEY_QUICKSEND ( * ( mcr_Key * ) MCR_ARR_AT ( mcr_echoEvents, \
			( unsigned ) ( echo ).event ) ) \
} \
else \
	dmsg \

// mcr_Key
# undef MCR_KEY_KEY
# undef MCR_KEY_SET_KEY
# undef MCR_KEY_SCAN
# undef MCR_KEY_SET_SCAN
# undef MCR_KEY_UP_TYPE
# undef MCR_KEY_SET_UP_TYPE
# define MCR_KEY_KEY( key ) \
( key ).events [ 1 ].code
# define MCR_KEY_SET_KEY( key, keyCode ) \
( key ).events [ 1 ].code = ( keyCode )
# define MCR_KEY_SCAN( key ) \
( key ).events [ 0 ].value
# define MCR_KEY_SET_SCAN( key, scan ) \
( key ).events [ 0 ].value = ( scan )
# define MCR_KEY_UP_TYPE( key ) \
( key ).up_type
# define MCR_KEY_SET_UP_TYPE( key, keyUp ) \
( key ).up_type = ( keyUp )

// mcr_MoveCursor
# undef MCR_MOVECURSOR_POSITION
# undef MCR_MOVECURSOR_SET_POSITION
# undef MCR_MOVECURSOR_COORDINATE
# undef MCR_MOVECURSOR_SET_COORDINATE
# undef MCR_MOVECURSOR_JUSTIFY
# undef MCR_MOVECURSOR_SET_JUSTIFY
# undef MCR_MOVECURSOR_SEND
# undef MCR_MOVECURSOR_QUICKSEND
# define MCR_MOVECURSOR_POSITION( mc, buffer ) \
( buffer ) [ MCR_X ] = ( mc ).relvent [ MCR_X ].value ; \
( buffer ) [ MCR_Y ] = ( mc ).relvent [ MCR_Y ].value ; \
( buffer ) [ MCR_Z ] = ( mc ).relvent [ MCR_Z ].value ;
# define MCR_MOVECURSOR_SET_POSITION( mc, buffer ) \
( mc ).relvent [ MCR_X ].value = \
( mc ).absvent [ MCR_X ].value = ( buffer ) [ MCR_X ] ; \
( mc ).relvent [ MCR_Y ].value = \
( mc ).absvent [ MCR_Y ].value = ( buffer ) [ MCR_Y ] ; \
( mc ).relvent [ MCR_Z ].value = \
( mc ).absvent [ MCR_Z ].value = ( buffer ) [ MCR_Z ] ;
# define MCR_MOVECURSOR_COORDINATE( mc, posType ) \
( mc ).relvent [ posType ].value
# define MCR_MOVECURSOR_SET_COORDINATE( mc, posType, val ) \
( mc ).relvent [ posType ].value = \
( mc ).absvent [ posType ].value = val ;
# define MCR_MOVECURSOR_JUSTIFY( mc ) \
( mc ).justify
# define MCR_MOVECURSOR_SET_JUSTIFY( mc, enable ) \
( mc ).justify = ( enable )
# define MCR_MOVECURSOR_JUSTIFY_IMPL( mc ) \
if ( ( mc ).justify ) \
{ \
	mcr_cursor [ MCR_X ] += ( mc ).relvent [ MCR_X ].value ; \
	mcr_cursor [ MCR_Y ] += ( mc ).relvent [ MCR_Y ].value ; \
	mcr_cursor [ MCR_Z ] += ( mc ).relvent [ MCR_Z ].value ; \
	if ( mcr_cursor [ MCR_X ] > ( long long ) mcr_abs_resolution ) \
		mcr_cursor [ MCR_X ] = mcr_abs_resolution ; \
	else if ( mcr_cursor [ MCR_X ] < 0 ) \
		mcr_cursor [ MCR_X ] = 0 ; \
	if ( mcr_cursor [ MCR_Y ] > ( long long ) mcr_abs_resolution ) \
		mcr_cursor [ MCR_Y ] = mcr_abs_resolution ; \
	else if ( mcr_cursor [ MCR_Y ] < 0 ) \
		mcr_cursor [ MCR_Y ] = 0 ; \
	if ( mcr_cursor [ MCR_Z ] > ( long long ) mcr_abs_resolution ) \
		mcr_cursor [ MCR_Z ] = mcr_abs_resolution ; \
	else if ( mcr_cursor [ MCR_Z ] < 0 ) \
		mcr_cursor [ MCR_Z ] = 0 ; \
}

# define MCR_MOVECURSOR_SEND( mc, success ) \
if ( ( mc ).justify ) \
{ \
	if ( ! MCR_DEV_SEND ( mcr_genDev, ( mc ).relvent, \
			sizeof ( ( mc ).relvent ) ) ) \
		success = 0 ; \
	MCR_MOVECURSOR_JUSTIFY_IMPL ( mc ) ; \
} \
else \
{ \
	if ( ! MCR_DEV_SEND ( mcr_absDev, ( mc ).absvent, \
			sizeof ( ( mc ).absvent ) ) ) \
		success = 0 ; \
	mcr_cursor [ MCR_X ] = ( mc ).absvent [ MCR_X ].value ; \
	mcr_cursor [ MCR_Y ] = ( mc ).absvent [ MCR_Y ].value ; \
	mcr_cursor [ MCR_Z ] = ( mc ).absvent [ MCR_Z ].value ; \
}

# define MCR_MOVECURSOR_QUICKSEND( mc ) \
if ( ( mc ).justify ) \
{ \
	MCR_DEV_SEND ( mcr_genDev, ( mc ).relvent, \
			sizeof ( ( mc ).relvent ) ) ; \
	MCR_MOVECURSOR_JUSTIFY_IMPL ( mc ) ; \
} \
else \
{ \
	MCR_DEV_SEND ( mcr_absDev, ( mc ).absvent, \
			sizeof ( ( mc ).absvent ) ) ; \
	mcr_cursor [ MCR_X ] = ( mc ).absvent [ MCR_X ].value ; \
	mcr_cursor [ MCR_Y ] = ( mc ).absvent [ MCR_Y ].value ; \
	mcr_cursor [ MCR_Z ] = ( mc ).absvent [ MCR_Z ].value ; \
}

// mcr_Scroll
# undef MCR_SCROLL_DIMENSIONS
# undef MCR_SCROLL_SET_DIMENSIONS
# undef MCR_SCROLL_COORDINATE
# undef MCR_SCROLL_SET_COORDINATE
# undef MCR_SCROLL_SEND
# undef MCR_SCROLL_QUICKSEND
# define MCR_SCROLL_DIMENSIONS( scroll, buffer ) \
( buffer ) [ MCR_X ] = ( scroll ).events [ MCR_X ].value ; \
( buffer ) [ MCR_Y ] = ( scroll ).events [ MCR_Y ].value ; \
( buffer ) [ MCR_Z ] = ( scroll ).events [ MCR_Z ].value ;
# define MCR_SCROLL_SET_DIMENSIONS( scroll, buffer ) \
( scroll ).events [ MCR_X ].value = ( buffer ) [ MCR_X ] ; \
( scroll ).events [ MCR_Y ].value = ( buffer ) [ MCR_Y ] ; \
( scroll ).events [ MCR_Z ].value = ( buffer ) [ MCR_Z ] ;
# define MCR_SCROLL_COORDINATE( scroll, pos ) \
( scroll ).events [ pos ].value
# define MCR_SCROLL_SET_COORDINATE( scroll, pos, val ) \
( scroll ).events [ pos ].value = val
# define MCR_SCROLL_SEND( scroll, success ) \
if ( ! MCR_DEV_SEND ( mcr_genDev, ( scroll ).events, \
		sizeof ( ( scroll ).events ) ) ) \
	success = 0 ;
# define MCR_SCROLL_QUICKSEND( scroll ) \
MCR_DEV_SEND ( mcr_genDev, ( scroll ).events, \
		sizeof ( ( scroll ).events ) )

# endif // MCR_LNX_STANDARD_H
