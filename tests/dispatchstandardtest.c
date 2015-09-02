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


# include "hotkey/hotkey.h"
# include <assert.h>

# define SIZE 0xFF
# define STDLEN 6

mcr_Dispatch * pt ;

mcr_Alarm alm ;
mcr_HIDEcho echo ;
mcr_Key key ;
mcr_MoveCursor mc ;
mcr_NoOp noop ;
mcr_Scroll scr ;

mcr_Signal sig ;

mcr_Hot hots [ SIZE ] ;

int specCalled = 0 ;

void chk_hot_disp ( mcr_Hot * a, mcr_Signal * b, unsigned int c )
{
	UNUSED ( a ) ; UNUSED ( b ) ;
	assert ( c == mcr_internalMods ) ;
	++ specCalled ;
}

void onComplete ( void )
{
	mcr_hotkey_cleanup ( ) ;
	mcr_signal_cleanup ( ) ;
}

void setup ( )
{
	mcr_set_stdio ( ) ;
	mcr_reg_cleanup ( onComplete ) ;
	mcr_signal_initialize ( ) ;
	mcr_hotkey_initialize ( ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_Hot_init_with ( hots + i, & mcr_iHotkey, NULL, 0, 0,
				chk_hot_disp, NULL ) ;
	}

	fprintf ( mcr_out, "Setup - OK\n" ) ;
}

# define MSET( isigPt, dataPt, id ) \
	sig.type = ( isigPt ) ; \
	sig.data.data = ( dataPt ) ; \
	pt = mcr_Dispatch_get ( id ) ; \
	mcr_internalMods = 0 ; \
	mcr_Dispatch_clear ( isigPt ) ; \
	assert ( ! pt->generics.used ) ; \
	mcr_Dispatch_set_enabled ( isigPt, 1 ) ;

# define TEST( sigPt, calledNumber ) \
	specCalled = 0 ; \
	mcr_dispatch ( sigPt ) ; \
	assert ( specCalled == ( calledNumber ) ) ;

# define TEST_ADDGENERIC( dispPt, addSpecFnc ) \
{ \
	addSpecFnc ( & sig, MCR_ANY_MOD, MCR_ALL, hots ) ; \
	assert ( dispPt->generics.used == 1 ) ; \
	TEST ( & sig, 1 ) ; \
	mcr_Dispatch_clear ( sig.type ) ; \
}

// Alarm is mapped from modifier only.
void alarmTest ( )
{
	MSET ( & mcr_iAlarm, & alm, mcr_iAlarm.iface.id ) ;

	memset ( & alm, 0, sizeof ( mcr_Alarm ) ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchAlarm_add ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchAlarm_add ( & sig, 0, MCR_ALL, hots + i ) ;
		TEST ( & sig, i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchAlarm_remove ( hots + i ) ;
		TEST ( & sig, SIZE - 1 - i ) ;
	}

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchAlarm_add ( & sig, 0, MCR_ALL, hots + i ) ;
	}
	mcr_Dispatch_clear ( sig.type ) ;
	TEST ( & sig, 0 ) ;

	fprintf ( mcr_out, "mcr_DispatchAlarm - OK\n" ) ;
}

// Echo maps modifier to echo code to hotkey.
void echoTest ( )
{
	MSET ( & mcr_iHIDEcho, & echo, mcr_iHIDEcho.iface.id ) ;

	mcr_Echo_init_with ( & echo, MCR_ANY_MOD ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchHIDEcho_add ) ;

	mcr_Echo_set_echo ( & echo, 0 ) ;
	mcr_DispatchHIDEcho_add ( & sig, 0, MCR_ALL, hots ) ; // 0 + 1
	TEST ( & sig, 1 ) ;
	mcr_DispatchHIDEcho_add ( & sig,
			MCR_ANY_MOD, MCR_ALL, hots ) ; // 1 + 1
	TEST ( & sig, 2 ) ;
	mcr_DispatchHIDEcho_add ( NULL, 0, MCR_ALL, hots ) ; // 2 + 1
	TEST ( & sig, 3 ) ;
	mcr_DispatchHIDEcho_add ( & sig, 0, MCR_ALL, hots + 1 ) ;
	mcr_DispatchHIDEcho_remove ( hots ) ;
	TEST ( & sig, 1 ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchHIDEcho_add ( & sig, 0, MCR_ALL, hots + i ) ;
	}
	mcr_DispatchHIDEcho_clear ( ) ;
	TEST ( & sig, 0 ) ;

	fprintf ( mcr_out, "mcr_DispatchHIDEcho - OK\n" ) ;
}

// Key maps key down/up to modifier to either key or scan code to hotkey.
void keyTest ( )
{
	MSET ( & mcr_iKey, & key, mcr_iKey.iface.id ) ;

	mcr_Key_init_with ( & key, MCR_ANY_KEY, MCR_ANY_KEY, MCR_BOTH ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchKey_add ) ;

	// Key down + up for modifier. Will be added for key code.
	mcr_DispatchKey_add ( & sig, 0, MCR_ALL, hots ) ; // 0 + 2
	// Key down + up for key, modifier and not.
	mcr_Key_set_key ( & key, 1 ) ;
	mcr_DispatchKey_add ( & sig, 0, MCR_ALL, hots ) ;
			// 2 + 2
	TEST ( & sig, 4 ) ;
	mcr_DispatchKey_add ( & sig, MCR_ANY_MOD, MCR_ALL, hots ) ;
			// 4 + 2
	TEST ( & sig, 6 ) ;
	mcr_DispatchKey_remove ( hots ) ;
	// Key and scan, down + up, modifier and not.
	mcr_Key_set_scan ( & key, 1 ) ;
	mcr_DispatchKey_add ( & sig, 0, MCR_ALL, hots ) ; // 0 + 4
	mcr_DispatchKey_add ( & sig, MCR_ANY_MOD, MCR_ALL, hots ) ;
			// 4 + 4
	TEST ( & sig, 8 ) ;
	mcr_DispatchKey_remove ( hots ) ;
	// Scan down + up, modifier and not.
	mcr_Key_set_key ( & key, MCR_ANY_KEY ) ;
	mcr_DispatchKey_add ( & sig, 0, MCR_ALL, hots ) ; // 0 + 2
	mcr_DispatchKey_add ( & sig, MCR_ANY_MOD, MCR_ALL, hots ) ;
			// 2 + 2
	mcr_Key_set_key ( & key, 1 ) ;
	assert ( ! pt->generics.used ) ;
	TEST ( & sig, 4 ) ;

	mcr_Dispatch_clear ( sig.type ) ;
	TEST ( & sig, 0 ) ;

	mcr_Key_set_scan ( & key, MCR_ANY_KEY ) ;
	mcr_Key_set_up_type ( & key, MCR_DOWN ) ;
	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchKey_add ( & sig, 0, MCR_ALL, hots + i ) ;
		TEST ( & sig, i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;
	mcr_DispatchKey_remove ( hots ) ;
	TEST ( & sig, SIZE - 1 ) ;

	mcr_DispatchKey_clear ( ) ;
	TEST ( & sig, 0 ) ;

	fprintf ( mcr_out, "mcr_DispatchKey - OK\n" ) ;
}

// Absolute or absolute mapped by modifier. Justify -1 is both.
void cursorTest ( )
{
	MSET ( & mcr_iMoveCursor, & mc, mcr_iMoveCursor.iface.id ) ;

	mcr_SpacePosition pos = { 0 } ;
	mcr_MoveCursor_init_with ( & mc, pos, -1 ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchMoveCursor_add ) ;

	mcr_MoveCursor_set_justify ( & mc, 1 ) ;
	mcr_DispatchMoveCursor_add ( & sig,
			mcr_internalMods, MCR_ALL, hots ) ; // 2 + 1
	TEST ( & sig, 1 ) ;
	mcr_MoveCursor_set_justify ( & mc, 0 ) ;
	mcr_DispatchMoveCursor_add ( & sig,
			mcr_internalMods, MCR_ALL, hots ) ; // 2 + 1
	TEST ( & sig, 1 ) ;
	mcr_DispatchMoveCursor_add ( & sig,
			MCR_ANY_MOD, MCR_ALL, hots ) ; // 2 + 1
	TEST ( & sig, 2 ) ;
	mcr_MoveCursor_set_justify ( & mc, 1 ) ;
	mcr_DispatchMoveCursor_add ( & sig,
			MCR_ANY_MOD, MCR_ALL, hots ) ; // 3 + 1
	TEST ( & sig, 2 ) ;

	mcr_DispatchMoveCursor_clear ( ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchMoveCursor_add ( & sig, 0, MCR_ALL, hots + i ) ;
		TEST ( & sig, i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;
	mcr_DispatchMoveCursor_remove ( hots ) ;
	TEST ( & sig, SIZE - 1 ) ;

	mcr_DispatchMoveCursor_clear ( ) ;

	fprintf ( mcr_out, "mcr_DispatchMoveCursor - OK\n" ) ;
}

// Mapped only by modifier.
void noopTest ( )
{
	MSET ( & mcr_iNoOp, & noop, mcr_iNoOp.iface.id ) ;

	memset ( & noop, 0, sizeof ( mcr_NoOp ) ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchNoOp_add ) ;

	mcr_DispatchNoOp_add ( & sig, 0, MCR_ALL, hots ) ;
	TEST ( & sig, 1 ) ;

	mcr_Dispatch_clear ( sig.type ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchNoOp_add ( & sig, 0, MCR_ALL, hots + i ) ;
		TEST ( & sig, i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;
	mcr_DispatchNoOp_remove ( hots ) ;
	TEST ( & sig, SIZE - 1 ) ;

	mcr_DispatchNoOp_clear ( ) ;

	fprintf ( mcr_out, "mcr_DispatchNoOp - OK\n" ) ;
}

// Mapped only by modifier.
void scrollTest ( )
{
	MSET ( & mcr_iScroll, & scr, mcr_iScroll.iface.id ) ;

	mcr_Dimensions dm = { 0 } ;
	mcr_Scroll_init_with ( & scr, dm ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchScroll_add ) ;

	mcr_DispatchScroll_add ( & sig, 0, MCR_ALL, hots ) ;
	TEST ( & sig, 1 ) ;

	mcr_Dispatch_clear ( sig.type ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchScroll_add ( & sig, 0, MCR_ALL, hots + i ) ;
		TEST ( & sig, i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;
	mcr_DispatchScroll_remove ( hots ) ;
	TEST ( & sig, SIZE - 1 ) ;

	mcr_DispatchScroll_clear ( ) ;

	fprintf ( mcr_out, "mcr_Dispatch_Scroll - OK\n" ) ;
}

// Dispatch standards, 6 standard types.
int main ( void )
{
	setup ( ) ;

	alarmTest ( ) ;
	echoTest ( ) ;
	keyTest ( ) ;
	cursorTest ( ) ;
	noopTest ( ) ;
	scrollTest ( ) ;

	fprintf ( mcr_out, "Test complete without assertion error.\n" ) ;

	return 0 ;
}
