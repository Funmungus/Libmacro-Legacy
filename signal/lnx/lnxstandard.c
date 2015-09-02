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

# include "signal/standard.h"
# include "signal/lnx/standard.h"

MCR_API mcr_Array mcr_echoEvents ;
MCR_API mcr_Map mcr_keyToEcho [ 2 ] ;
MCR_API mcr_SpacePosition mcr_cursor = { 0 } ;

static mcr_Key _keyInitial ;

void mcr_cursor_position ( mcr_SpacePosition buffer )
{
	memcpy ( buffer, mcr_cursor, sizeof ( mcr_SpacePosition ) ) ;
}

void mcr_standard_enable ( int enable )
{
	if ( mcr_set_privileged ( 1 ) )
	{
		mcr_Device_enable_all ( enable ) ;
	}
	else
	{
		dmsg ;
	}
# ifdef MCR_USE_X
	mcr_usex_enable ( enable ) ;
# endif
	mcr_set_privileged ( 0 ) ;
}

int mcr_Echo_set_key ( int echoCode, mcr_Key * keyPt )
{
	dassert ( keyPt ) ;
	if ( echoCode == -1 )
	{
		dmsg ;
		return -1 ;
	}
	int ret = 1 ;
	if ( ( unsigned int ) echoCode < mcr_echoEvents.used )
	{
		if ( ! mcr_Array_set ( & mcr_echoEvents,
				( unsigned int ) echoCode, keyPt ) )
		{
			dmsg ;
			ret = 0 ;
		}
	}
	else
	{
		if ( ! mcr_Array_insert_filled ( & mcr_echoEvents,
				( unsigned int ) echoCode, keyPt, & _keyInitial ) )
		{
			dmsg ;
			ret = 0 ;
		}
	}
	if ( ret )
	{
		if ( ! mcr_Map_map ( mcr_keyToEcho + ( MCR_KEY_UP_TYPE
				( * keyPt ) ? 1 : 0 ),
				& MCR_KEY_KEY ( * keyPt ), & echoCode ) )
		{
			dmsg ;
			ret = 0 ;
		}
	}
	return ret ;
}

void mcr_Echo_init ( void * echoPt )
{
	dassert ( echoPt ) ;
	( ( mcr_HIDEcho * ) echoPt )->event = 0 ;
}

void mcr_Key_init ( void * keyPt )
{
	dassert ( keyPt ) ;
	mcr_Key * kPt = keyPt ;
	memset ( kPt, 0, sizeof ( mcr_Key ) ) ;
	kPt->events [ 0 ].type = EV_MSC ;
	kPt->events [ 0 ].code = MSC_SCAN ;
	kPt->events [ 1 ].type = EV_KEY ;
	memcpy ( kPt->events + 2, & mcr_syncer,
			sizeof ( struct input_event ) ) ;
	kPt->up_type = MCR_BOTH ;
}
void mcr_MoveCursor_init ( void * mcPt )
{
	dassert ( mcPt ) ;
	mcr_MoveCursor * mPt = mcPt ;
	memset ( mPt, 0, sizeof ( mcr_MoveCursor ) ) ;
	mPt->absvent [ 0 ].type = mPt->absvent [ 1 ].type =
			mPt->absvent [ 2 ].type = EV_ABS ;
	mPt->relvent [ 0 ].type = mPt->relvent [ 1 ].type =
			mPt->relvent [ 2 ].type = EV_REL ;
	mPt->absvent [ MCR_X ].code = ABS_X ;
	mPt->absvent [ MCR_Y ].code = ABS_Y ;
	mPt->absvent [ MCR_Z ].code = ABS_Z ;
	mPt->relvent [ MCR_X ].code = REL_X ;
	mPt->relvent [ MCR_Y ].code = REL_Y ;
	mPt->relvent [ MCR_Z ].code = REL_Z ;
	mPt->relvent [ MCR_DIMENSION_CNT ] =
			mPt->absvent [ MCR_DIMENSION_CNT ] = mcr_syncer ;
	mPt->justify = 1 ;
}
void mcr_Scroll_init ( void * scrollPt )
{
	dassert ( scrollPt ) ;
	mcr_Scroll * sPt = scrollPt ;
	memset ( sPt, 0, sizeof ( mcr_Scroll ) ) ;
	sPt->events [ 0 ].type = sPt->events [ 1 ].type =
			sPt->events [ 2 ].type = EV_REL ;
	sPt->events [ MCR_X ].code = REL_HWHEEL ;
	sPt->events [ MCR_Y ].code = REL_WHEEL ;
	sPt->events [ MCR_Z ].code = REL_DIAL ;
	sPt->events [ MCR_DIMENSION_CNT ] = mcr_syncer ;
}

void mcr_standard_native_initialize ( )
{
	// Device initialization has a break, to wait for devices to be ready.
	mcr_Device_initialize ( ) ;
	mcr_Array_init ( & mcr_echoEvents, sizeof ( mcr_Key ) ) ;
	mcr_Map_init ( mcr_keyToEcho, sizeof ( int ), sizeof ( int ) ) ;
	mcr_Map_init ( mcr_keyToEcho + 1, sizeof ( int ), sizeof ( int ) ) ;
	mcr_keyToEcho [ 0 ].compare = mcr_keyToEcho [ 1 ].compare =
			mcr_int_compare ;
	mcr_Key_init ( & _keyInitial ) ;
# ifdef MCR_USE_X
	mcr_usex_initialize ( ) ;
# endif
}

void mcr_standard_native_cleanup ( void )
{
	mcr_Device_cleanup ( ) ;
	mcr_Array_free ( & mcr_echoEvents ) ;
	mcr_Map_free ( mcr_keyToEcho ) ;
	mcr_Map_free ( mcr_keyToEcho + 1 ) ;
# ifdef MCR_USE_X
	mcr_usex_cleanup ( ) ;
# endif
}
