/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */


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

	fprintf ( mcr_stdout, "Setup - OK\n" ) ;
}

# define MSET( isigPt, dataPt, id ) \
	sig.type = ( isigPt ) ; \
	sig.data.data = ( dataPt ) ; \
	pt = mcr_Dispatch_get ( id ) ; \
	mcr_internalMods = 0 ; \
	mcr_Dispatch_clear ( isigPt ) ; \
	assert ( ! pt->generics.used ) ; \
	mcr_Dispatch_enable ( isigPt, 1 ) ;

# define TEST( sigPt, calledNumber ) \
	specCalled = 0 ; \
	mcr_dispatch ( sigPt ) ; \
	assert ( specCalled == ( calledNumber ) ) ;

# define TEST_ADDGENERIC( dispPt, addSpecFnc ) \
{ \
	addSpecFnc ( & sig, MCR_ANY_MOD, hots ) ; \
	assert ( dispPt->generics.used == 1 ) ; \
	TEST ( & sig, 1 ) ; \
	mcr_Dispatch_clear ( sig.type ) ; \
}

// Alarm is mapped from modifier only.
void alarmTest ( )
{
	MSET ( & mcr_iAlarm, & alm, mcr_iAlarm.iface.id ) ;

	memset ( & alm, 0, sizeof ( mcr_Alarm ) ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchAlarm_add_specific ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchAlarm_add_specific ( & sig, 0, hots + i ) ;
		TEST ( & sig, i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchAlarm_remove_specific ( hots + i ) ;
		TEST ( & sig, SIZE - 1 - i ) ;
	}

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchAlarm_add_specific ( & sig, 0, hots + i ) ;
	}
	mcr_Dispatch_clear ( sig.type ) ;
	TEST ( & sig, 0 ) ;

	fprintf ( mcr_stdout, "mcr_DispatchAlarm - OK\n" ) ;
}

// Echo maps modifier to echo code to hotkey.
void echoTest ( )
{
	MSET ( & mcr_iHIDEcho, & echo, mcr_iHIDEcho.iface.id ) ;

	mcr_Echo_init_with ( & echo, MCR_ANY_MOD ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchHIDEcho_add_specific ) ;

	mcr_Echo_set ( & echo, 0 ) ;
	mcr_DispatchHIDEcho_add_specific ( & sig, 0, hots ) ; // 0 + 1
	TEST ( & sig, 1 ) ;
	mcr_DispatchHIDEcho_add_specific ( & sig,
			MCR_ANY_MOD, hots ) ; // 1 + 1
	TEST ( & sig, 2 ) ;
	mcr_DispatchHIDEcho_add_specific ( NULL, 0, hots ) ; // 2 + 1
	TEST ( & sig, 3 ) ;
	mcr_DispatchHIDEcho_add_specific ( & sig, 0, hots + 1 ) ;
	mcr_DispatchHIDEcho_remove_specific ( hots ) ;
	TEST ( & sig, 1 ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchHIDEcho_add_specific ( & sig, 0, hots + i ) ;
	}
	mcr_DispatchHIDEcho_clear ( sig.type ) ;
	TEST ( & sig, 0 ) ;

	fprintf ( mcr_stdout, "mcr_DispatchHIDEcho - OK\n" ) ;
}

// Key maps key down/up to modifier to either key or scan code to hotkey.
void keyTest ( )
{
	MSET ( & mcr_iKey, & key, mcr_iKey.iface.id ) ;

	mcr_Key_init_with ( & key, MCR_ANY_KEY, MCR_ANY_KEY, MCR_BOTH ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchKey_add_specific ) ;

	// Key down + up for modifier. Will be added for key code.
	mcr_DispatchKey_add_specific ( & sig, 0, hots ) ; // 0 + 2
	// Key down + up for key, modifier and not.
	mcr_Key_set ( & key, 1 ) ;
	mcr_DispatchKey_add_specific ( & sig, 0, hots ) ;
			// 2 + 2
	TEST ( & sig, 4 ) ;
	mcr_DispatchKey_add_specific ( & sig, MCR_ANY_MOD, hots ) ;
			// 4 + 2
	TEST ( & sig, 6 ) ;
	mcr_DispatchKey_remove_specific ( hots ) ;
	// Key and scan, down + up, modifier and not.
	mcr_Key_set_scan ( & key, 1 ) ;
	mcr_DispatchKey_add_specific ( & sig, 0, hots ) ; // 0 + 4
	mcr_DispatchKey_add_specific ( & sig, MCR_ANY_MOD, hots ) ;
			// 4 + 4
	TEST ( & sig, 8 ) ;
	mcr_DispatchKey_remove_specific ( hots ) ;
	// Scan down + up, modifier and not.
	mcr_Key_set ( & key, MCR_ANY_KEY ) ;
	mcr_DispatchKey_add_specific ( & sig, 0, hots ) ; // 0 + 2
	mcr_DispatchKey_add_specific ( & sig, MCR_ANY_MOD, hots ) ;
			// 2 + 2
	mcr_Key_set ( & key, 1 ) ;
	assert ( ! pt->generics.used ) ;
	TEST ( & sig, 4 ) ;

	mcr_Dispatch_clear ( sig.type ) ;
	TEST ( & sig, 0 ) ;

	mcr_Key_set_scan ( & key, MCR_ANY_KEY ) ;
	mcr_Key_set_up_type ( & key, MCR_DOWN ) ;
	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchKey_add_specific ( & sig, 0, hots + i ) ;
		TEST ( & sig, i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;
	mcr_DispatchKey_remove_specific ( hots ) ;
	TEST ( & sig, SIZE - 1 ) ;

	mcr_DispatchKey_clear ( sig.type ) ;
	TEST ( & sig, 0 ) ;

	fprintf ( mcr_stdout, "mcr_DispatchKey - OK\n" ) ;
}

// Absolute or absolute mapped by modifier. Justify -1 is both.
void cursorTest ( )
{
	MSET ( & mcr_iMoveCursor, & mc, mcr_iMoveCursor.iface.id ) ;

	mcr_SpacePosition pos = { 0 } ;
	mcr_MoveCursor_init_with ( & mc, pos, -1 ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchMoveCursor_add_specific ) ;

	mcr_MoveCursor_enable_justify ( & mc, 1 ) ;
	mcr_DispatchMoveCursor_add_specific ( & sig,
			mcr_internalMods, hots ) ; // 2 + 1
	TEST ( & sig, 1 ) ;
	mcr_MoveCursor_enable_justify ( & mc, 0 ) ;
	mcr_DispatchMoveCursor_add_specific ( & sig,
			mcr_internalMods, hots ) ; // 2 + 1
	TEST ( & sig, 1 ) ;
	mcr_DispatchMoveCursor_add_specific ( & sig,
			MCR_ANY_MOD, hots ) ; // 2 + 1
	TEST ( & sig, 2 ) ;
	mcr_MoveCursor_enable_justify ( & mc, 1 ) ;
	mcr_DispatchMoveCursor_add_specific ( & sig,
			MCR_ANY_MOD, hots ) ; // 3 + 1
	TEST ( & sig, 2 ) ;

	mcr_DispatchMoveCursor_clear ( sig.type ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchMoveCursor_add_specific ( & sig, 0, hots + i ) ;
		TEST ( & sig, i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;
	mcr_DispatchMoveCursor_remove_specific ( hots ) ;
	TEST ( & sig, SIZE - 1 ) ;

	mcr_DispatchMoveCursor_clear ( sig.type ) ;

	fprintf ( mcr_stdout, "mcr_DispatchMoveCursor - OK\n" ) ;
}

// Mapped only by modifier.
void noopTest ( )
{
	MSET ( & mcr_iNoOp, & noop, mcr_iNoOp.iface.id ) ;

	memset ( & noop, 0, sizeof ( mcr_NoOp ) ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchNoOp_add_specific ) ;

	mcr_DispatchNoOp_add_specific ( & sig, 0, hots ) ;
	TEST ( & sig, 1 ) ;

	mcr_Dispatch_clear ( sig.type ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchNoOp_add_specific ( & sig, 0, hots + i ) ;
		TEST ( & sig, i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;
	mcr_DispatchNoOp_remove_specific ( hots ) ;
	TEST ( & sig, SIZE - 1 ) ;

	mcr_DispatchNoOp_clear ( sig.type ) ;

	fprintf ( mcr_stdout, "mcr_DispatchNoOp - OK\n" ) ;
}

// Mapped only by modifier.
void scrollTest ( )
{
	MSET ( & mcr_iScroll, & scr, mcr_iScroll.iface.id ) ;

	mcr_Dimensions dm = { 0 } ;
	mcr_Scroll_init_with ( & scr, dm ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchScroll_add_specific ) ;

	mcr_DispatchScroll_add_specific ( & sig, 0, hots ) ;
	TEST ( & sig, 1 ) ;

	mcr_Dispatch_clear ( sig.type ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchScroll_add_specific ( & sig, 0, hots + i ) ;
		TEST ( & sig, i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;
	mcr_DispatchScroll_remove_specific ( hots ) ;
	TEST ( & sig, SIZE - 1 ) ;

	mcr_DispatchScroll_clear ( sig.type ) ;

	fprintf ( mcr_stdout, "mcr_Dispatch_Scroll - OK\n" ) ;
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

	fprintf ( mcr_stdout, "Test complete without assertion error.\n" ) ;

	return 0 ;
}
