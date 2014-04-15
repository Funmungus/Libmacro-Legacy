
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

void chk_hot_disp ( mcr_Hot * a, mcr_Signal * b, unsigned int * c )
{
	UNUSED ( a ) ; UNUSED ( b ) ;
	assert ( c ) ;
	assert ( c == & mcr_InternalMods ) ;
	++ specCalled ;
}

void onComplete ( void )
{
	mcr_hotkey_cleanup ( ) ;
	mcr_signal_cleanup ( ) ;
}

void setup ( )
{
	mcr_reg_cleanup_filed ( onComplete, __FILE__ ) ;
	mcr_signal_initialize ( ) ;
	mcr_hotkey_initialize ( ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_Hot_init_with ( hots + i, 0, chk_hot_disp, NULL ) ;
	}

	printf ( "Setup - OK\n" ) ;
}

# define MSET( isigPt, dataPt, id ) \
	sig.type = ( isigPt ) ; \
	sig.data = ( dataPt ) ; \
	pt = mcr_Dispatch_get ( id ) ; \
	mcr_InternalMods = 0 ; \
	mcr_Dispatch_clear ( pt ) ; \
	assert ( ! pt->generics.used ) ;

# define TEST( sigPt, calledNumber ) \
	specCalled = 0 ; \
	mcr_dispatch ( sigPt ) ; \
	assert ( specCalled == ( calledNumber ) ) ;

# define TESTN( calledNumber ) \
	specCalled = 0 ; \
	pt->dispatch_specific ( pt, NULL, \
			& mcr_InternalMods ) ; \
	assert ( specCalled == ( calledNumber ) ) ;

# define TEST_ADDGENERIC( dispPt, addSpecFnc ) \
{ \
	addSpecFnc ( dispPt, hots, & sig, MCR_ANY_MOD ) ; \
	assert ( dispPt->generics.used == 1 ) ; \
	TEST ( & sig, 1 ) ; \
	mcr_Dispatch_clear ( dispPt ) ; \
	addSpecFnc ( dispPt, hots, NULL, MCR_ANY_MOD ) ; \
	assert ( dispPt->generics.used == 1 ) ; \
	TEST ( & sig, 1 ) ; \
	mcr_Dispatch_clear ( dispPt ) ; \
}

// Alarm is mapped from modifier only.
void alarmTest ( )
{
	MSET ( & mcr_IAlarm, & alm, mcr_IAlarm.id ) ;

	memset ( & alm.time_point, 0, sizeof ( struct tm ) ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchAlarm_add_specific ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchAlarm_add_specific ( pt, hots + i, & sig, 0 ) ;
		TEST ( & sig, i + 1 ) ;
		TESTN ( i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchAlarm_remove_specific ( pt, hots + i ) ;
		TEST ( & sig, SIZE - 1 - i ) ;
		TESTN ( SIZE - 1 - i ) ;
	}

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchAlarm_add_specific ( pt, hots + i, & sig, 0 ) ;
	}
	mcr_Dispatch_clear ( pt ) ;
	TEST ( & sig, 0 ) ;
	TESTN ( 0 ) ;

	printf ( "mcr_DispatchAlarm - OK\n" ) ;
}

// Echo maps modifier to echo code to hotkey.
void echoTest ( )
{
	MSET ( & mcr_IHIDEcho, & echo, mcr_IHIDEcho.id ) ;

	mcr_Echo_init_with ( & echo, MCR_ANY_MOD ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchHIDEcho_add_specific ) ;

	mcr_Echo_set ( & echo, 0 ) ;
	mcr_DispatchHIDEcho_add_specific ( pt, hots, & sig, 0 ) ; // 0 + 1
	TEST ( & sig, 1 ) ;
	TESTN ( 0 ) ;
	mcr_DispatchHIDEcho_add_specific ( pt, hots, & sig,
			MCR_ANY_MOD ) ; // 1 + 1
	TEST ( & sig, 2 ) ;
	TESTN ( 0 ) ;
	mcr_DispatchHIDEcho_add_specific ( pt, hots, NULL, 0 ) ; // 2 + 1
	TEST ( & sig, 3 ) ;
	TESTN ( 1 ) ;
	mcr_DispatchHIDEcho_add_specific ( pt, hots + 1, & sig, 0 ) ;
	mcr_DispatchHIDEcho_remove_specific ( pt, hots ) ;
	TEST ( & sig, 1 ) ;
	TESTN ( 0 ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchHIDEcho_add_specific ( pt, hots + i, & sig, 0 ) ;
	}
	mcr_DispatchHIDEcho_clear ( pt ) ;
	TEST ( & sig, 0 ) ;

	printf ( "mcr_DispatchHIDEcho - OK\n" ) ;
}

// Key maps key down/up to modifier to either key or scan code to hotkey.
void keyTest ( )
{
	MSET ( & mcr_IKey, & key, mcr_IKey.id ) ;

	mcr_Key_init_with ( & key, MCR_ANY_KEY, MCR_ANY_KEY, MCR_BOTH ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchKey_add_specific ) ;

	// Key down + up for modifier. Will be added for key code.
	mcr_DispatchKey_add_specific ( pt, hots, & sig, 0 ) ; // 0 + 2
	TESTN ( 2 ) ;
	// Key down + up for key, modifier and not.
	mcr_Key_set ( & key, 1 ) ;
	mcr_DispatchKey_add_specific ( pt, hots, & sig, 0 ) ;
			// 2 + 2
	TEST ( & sig, 4 ) ;
	TESTN ( 2 ) ;
	mcr_DispatchKey_add_specific ( pt, hots, & sig, MCR_ANY_MOD ) ;
			// 4 + 2
	TEST ( & sig, 6 ) ;
	TESTN ( 2 ) ;
	mcr_DispatchKey_remove_specific ( pt, hots ) ;
	// Key and scan, down + up, modifier and not.
	mcr_Key_set_scan ( & key, 1 ) ;
	mcr_DispatchKey_add_specific ( pt, hots, & sig, 0 ) ; // 0 + 4
	mcr_DispatchKey_add_specific ( pt, hots, & sig, MCR_ANY_MOD ) ;
			// 4 + 4
	TEST ( & sig, 8 ) ;
	TESTN ( 0 ) ;
	mcr_DispatchKey_remove_specific ( pt, hots ) ;
	// Scan down + up, modifier and not.
	mcr_Key_set ( & key, MCR_ANY_KEY ) ;
	mcr_DispatchKey_add_specific ( pt, hots, & sig, 0 ) ; // 0 + 2
	mcr_DispatchKey_add_specific ( pt, hots, & sig, MCR_ANY_MOD ) ;
			// 2 + 2
	mcr_Key_set ( & key, 1 ) ;
	assert ( ! pt->generics.used ) ;
	TEST ( & sig, 4 ) ;

	mcr_Dispatch_clear ( pt ) ;
	TEST ( & sig, 0 ) ;

	mcr_Key_set_scan ( & key, MCR_ANY_KEY ) ;
	mcr_Key_set_up_type ( & key, MCR_DOWN ) ;
	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchKey_add_specific ( pt, hots + i, & sig, 0 ) ;
		TEST ( & sig, i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;
	mcr_DispatchKey_remove_specific ( pt, hots ) ;
	TEST ( & sig, SIZE - 1 ) ;

	mcr_DispatchKey_clear ( pt ) ;
	TEST ( & sig, 0 ) ;

	printf ( "mcr_DispatchKey - OK\n" ) ;
}

// Absolute or absolute mapped by modifier. Justify -1 is both.
void cursorTest ( )
{
	MSET ( & mcr_IMoveCursor, & mc, mcr_IMoveCursor.id ) ;

	mcr_SpacePosition pos = { 0 } ;
	mcr_MoveCursor_init_with ( & mc, pos, -1 ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchMoveCursor_add_specific ) ;

	mcr_DispatchMoveCursor_add_specific ( pt, hots, NULL, 0 ) ; // 0 + 2
	TEST ( & sig, 2 ) ;
	TESTN ( 2 ) ;
	mcr_MoveCursor_enable_justify ( & mc, 1 ) ;
	TEST ( & sig, 1 ) ;
	mcr_MoveCursor_enable_justify ( & mc, 0 ) ;
	TEST ( & sig, 1 ) ;
	mcr_DispatchMoveCursor_add_specific ( pt, hots, & sig,
			MCR_ANY_MOD ) ; // 2 + 1
	TEST ( & sig, 2 ) ;
	TESTN ( 3 ) ;
	mcr_MoveCursor_enable_justify ( & mc, 1 ) ;
	mcr_DispatchMoveCursor_add_specific ( pt, hots, & sig,
			MCR_ANY_MOD ) ; // 3 + 1
	TEST ( & sig, 2 ) ;
	TESTN ( 4 ) ;

	mcr_DispatchMoveCursor_clear ( pt ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchMoveCursor_add_specific ( pt, hots + i, & sig, 0 ) ;
		TEST ( & sig, i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;
	mcr_DispatchMoveCursor_remove_specific ( pt, hots ) ;
	TEST ( & sig, SIZE - 1 ) ;

	mcr_DispatchMoveCursor_clear ( pt ) ;
	TESTN ( 0 ) ;

	printf ( "mcr_DispatchMoveCursor - OK\n" ) ;
}

// Mapped only by modifier.
void noopTest ( )
{
	MSET ( & mcr_INoOp, & noop, mcr_INoOp.id ) ;

	memset ( & noop.tv, 0, sizeof ( struct timeval ) ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchNoOp_add_specific ) ;

	mcr_DispatchNoOp_add_specific ( pt, hots, & sig, 0 ) ;
	TEST ( & sig, 1 ) ;
	TESTN ( 1 ) ;

	mcr_Dispatch_clear ( pt ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchNoOp_add_specific ( pt, hots + i, & sig, 0 ) ;
		TEST ( & sig, i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;
	mcr_DispatchNoOp_remove_specific ( pt, hots ) ;
	TEST ( & sig, SIZE - 1 ) ;

	mcr_DispatchNoOp_clear ( pt ) ;
	TESTN ( 0 ) ;

	printf ( "mcr_DispatchNoOp - OK\n" ) ;
}

// Mapped only by modifier.
void scrollTest ( )
{
	MSET ( & mcr_IScroll, & scr, mcr_IScroll.id ) ;

	mcr_Dimensions dm = { 0 } ;
	mcr_Scroll_init_with ( & scr, dm ) ;

	TEST_ADDGENERIC ( pt, mcr_DispatchScroll_add_specific ) ;

	mcr_DispatchScroll_add_specific ( pt, hots, & sig, 0 ) ;
	TEST ( & sig, 1 ) ;
	TESTN ( 1 ) ;

	mcr_Dispatch_clear ( pt ) ;

	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_DispatchScroll_add_specific ( pt, hots + i, & sig, 0 ) ;
		TEST ( & sig, i + 1 ) ;
	}
	assert ( ! pt->generics.used ) ;
	mcr_DispatchScroll_remove_specific ( pt, hots ) ;
	TEST ( & sig, SIZE - 1 ) ;

	mcr_DispatchScroll_clear ( pt ) ;
	TESTN ( 0 ) ;

	printf ( "mcr_Dispatch_Scroll - OK\n" ) ;
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

	printf ( "Test complete without assertion error.\n" ) ;

	return 0 ;
}
