/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */


# include "hotkey/hotkey.h"
# include "hotkey/stage.h"
# include <assert.h>

mcr_Stage stage ;
mcr_Signal sig = { 0 } ;

void cleanup ( )
{
	mcr_hotkey_cleanup ( ) ;
	mcr_signal_cleanup ( ) ;
}

void setup ( )
{
	mcr_reg_cleanup ( cleanup ) ;
	mcr_signal_initialize ( ) ;
	mcr_hotkey_initialize ( ) ;
	assert ( mcr_Stage_ismeGeneric == mcr_Stage_isref ) ;

	printf ( "test setup - OK\n" ) ;
}

mcr_ISignal * knowns [ ] =
{
	& mcr_iAlarm, & mcr_iHIDEcho, & mcr_iKey,
	& mcr_iMoveCursor, & mcr_iNoOp, & mcr_iScroll, & mcr_iMod
} ;

mcr_isme_fnc fncs [ ] =
{
	mcr_Stage_isalarm, mcr_Stage_isecho, mcr_Stage_iskey,
	mcr_Stage_ismovecursor, mcr_Stage_isnoop, mcr_Stage_isscroll,
	mcr_Stage_ismod
} ;

const size_t kLen = sizeof ( knowns ) / sizeof ( mcr_ISignal * ) ;

void test_Stage_init_with ( )
{
	mcr_Stage_init_with ( & stage, 0, NULL, 5, 42 ) ;
	assert ( stage.activated == 0 ) ;
	assert ( stage.blocking == 0 ) ;
	assert ( stage.intercept_pt == NULL ) ;
	assert ( stage.isme == mcr_Stage_ismeGeneric ) ;
	assert ( stage.measurement_error == 5 ) ;
	assert ( stage.modifiers == 42 ) ;
	stage.isme = NULL ;
	stage.modifiers = 0 ;
	mcr_Stage_set ( & stage, NULL, 42 ) ;
	assert ( stage.isme == mcr_Stage_ismeGeneric ) ;
	assert ( stage.modifiers == 42 ) ;
	stage.isme = NULL ;
	stage.modifiers = 0 ;
	mcr_Stage_init_with ( & stage, 0, & sig, 5, 42 ) ;
	assert ( stage.activated == 0 ) ;
	assert ( stage.blocking == 0 ) ;
	assert ( stage.intercept_pt == & sig ) ;
	assert ( stage.isme == mcr_Stage_ismeGeneric ) ;
	assert ( stage.measurement_error == 5 ) ;
	assert ( stage.modifiers == 42 ) ;
	stage.isme = NULL ;
	stage.modifiers = 0 ;
	mcr_Stage_set ( & stage, & sig, 42 ) ;
	assert ( stage.isme == mcr_Stage_ismeGeneric ) ;
	assert ( stage.modifiers == 42 ) ;
	for ( unsigned int i = 0 ; i < kLen ; i ++ )
	{
		unsigned int chker = i % 2 ;
		sig.type = knowns [ i ] ;
		mcr_Stage_init_with ( & stage, ( int ) chker, & sig,
				chker, chker ) ;
		assert ( stage.activated == 0 ) ;
		assert ( stage.blocking == ( int ) chker ) ;
		assert ( stage.intercept_pt == & sig ) ;
		assert ( stage.isme == fncs [ i ] ) ;
		assert ( stage.measurement_error == chker ) ;
		assert ( stage.modifiers == chker ) ;
		stage.isme = NULL ;
		stage.modifiers = 42 ;
		mcr_Stage_set ( & stage, & sig, chker ) ;
		assert ( stage.blocking == ( int ) chker ) ;
		assert ( stage.isme == fncs [ i ] ) ;
		assert ( stage.modifiers == chker ) ;
	}

	printf ( "mcr_Stage_init_with - OK\n" ) ;
	printf ( "mcr_Stage_set - OK\n" ) ;
}

void test_Stage_count ( )
{
	assert ( mcr_Stage_count ( ) == mcr_ISignal_count ( ) ) ;
	printf ( "mcr_Stage_count - OK\n" ) ;
}

void test_Stage_get_all ( )
{
	mcr_isme_fnc all [ kLen ] ;
	mcr_Stage_get_all ( all, kLen ) ;
	for ( unsigned int i = 0 ; i < kLen ; i++ )
	{
		// Function exists for all id's.
		assert ( all [ i ] != NULL ) ;
		// Function is the same as isme function for that id.
		assert ( all [ i ] == mcr_Stage_isme_for ( i ) ) ;
	}
	printf ( "mcr_Stage_isme_for - OK\n" ) ;
	printf ( "mcr_Stage_get_all - OK\n" ) ;
}

void test_Stage_isref ( )
{
	stage.intercept_pt = NULL ;
	stage.modifiers = MCR_ANY_MOD ;
	assert ( mcr_Stage_isref ( & stage, NULL, 42 ) ) ;
	stage.modifiers = 0 ;
	assert ( ! mcr_Stage_isref ( & stage, & sig, 42 ) ) ;
	assert ( mcr_Stage_isref ( & stage, & sig, 0 ) ) ;
	stage.intercept_pt = & sig ;
	assert ( mcr_Stage_isref ( & stage, & sig, 0 ) ) ;
	assert ( ! mcr_Stage_isref ( & stage, & sig, 42 ) ) ;
	assert ( ! mcr_Stage_isref ( & stage, ( void * ) 1, 0 ) ) ;
	stage.modifiers = MCR_ANY_MOD ;
	assert ( mcr_Stage_isref ( & stage, & sig, 0 ) ) ;
	assert ( mcr_Stage_isref ( & stage, & sig, 42 ) ) ;
	assert ( ! mcr_Stage_isref ( & stage, ( void * ) 1, 0 ) ) ;

	printf ( "mcr_Stage_isref - OK\n" ) ;
}

// alarm tested by mktime ( struct tm * )
void test_Stage_isalarm ( )
{
	mcr_Alarm alm, alm2 ;
	mcr_Signal sig2 = { & mcr_iAlarm, & alm2 } ;
	sig.data = & alm ;
	sig.type = & mcr_iAlarm ;
	time_t t1 = time ( NULL ) ;
	alm = * localtime ( & t1 ) ;
	time_t t2 = t1 - 42 ;
	alm2 = * localtime ( & t2 ) ;
	mcr_Stage_init_with ( & stage, 0, & sig, 42, MCR_ANY_MOD ) ;
	assert ( mcr_Stage_isalarm ( & stage, & sig2, 0 ) ) ;
	-- t2 ;
	alm2 = * localtime ( & t2 ) ;
	assert ( ! mcr_Stage_isalarm ( & stage, & sig2, 0 ) ) ;
	t2 = t1 + 42 ;
	alm2 = * localtime ( & t2 ) ;
	assert ( mcr_Stage_isalarm ( & stage, & sig2, 0 ) ) ;
	++ t2 ;
	alm2 = * localtime ( & t2 ) ;
	assert ( ! mcr_Stage_isalarm ( & stage, & sig2, 0 ) ) ;

	sig.data = NULL ;
	stage.modifiers = 42 ;
	assert ( mcr_Stage_isalarm ( & stage, & sig2, 42 ) ) ;
	assert ( ! mcr_Stage_isalarm ( & stage, & sig2, 0 ) ) ;

	printf ( "mcr_Stage_isalarm - OK\n" ) ;
}

void test_Stage_isecho ( )
{
	mcr_HIDEcho e1, e2 ;
	mcr_Echo_init_with ( & e1, MCR_ANY_MOD ) ;
	mcr_Echo_init_with ( & e2, 0 ) ;
	sig.type = & mcr_iHIDEcho ;
	sig.data = & e1 ;
	mcr_Signal sig2 = { & mcr_iHIDEcho, & e2 } ;
	stage.modifiers = MCR_ANY_MOD ;
	assert ( mcr_Stage_isecho ( & stage, & sig2, 0 ) ) ;
	assert ( mcr_Stage_isecho ( & stage, & sig2, 42 ) ) ;
	mcr_Echo_set ( & e1, 4 ) ;
	assert ( ! mcr_Stage_isecho ( & stage, & sig2, 0 ) ) ;
	mcr_Echo_set ( & e2, 4 ) ;
	assert ( mcr_Stage_isecho ( & stage, & sig2, 0 ) ) ;
	stage.modifiers = 42 ;
	assert ( mcr_Stage_isecho ( & stage, & sig2, 42 ) ) ;
	assert ( ! mcr_Stage_isecho ( & stage, & sig2, 0 ) ) ;

	printf ( "mcr_Stage_isecho - OK\n" ) ;
}

void test_Stage_iskey ( )
{
	mcr_Key k1, k2 ;
	mcr_Key_init_with ( & k1, MCR_ANY_KEY, MCR_ANY_KEY, MCR_BOTH ) ;
	mcr_Key_init_with ( & k2, 42, 42, MCR_DOWN ) ;
	sig.type = & mcr_iKey ;
	sig.data = & k1 ;
	mcr_Signal sig2 = { & mcr_iKey, & k2 } ;
	stage.modifiers = MCR_ANY_MOD ;
	assert ( mcr_Stage_iskey ( & stage, & sig2, 42 ) ) ;

	mcr_Key_init_with ( & k1, MCR_ANY_KEY, MCR_ANY_KEY, MCR_DOWN ) ;
	assert ( mcr_Stage_iskey ( & stage, & sig2, 42 ) ) ;
	mcr_Key_set_up_type ( & k2, MCR_UP ) ;
	assert ( ! mcr_Stage_iskey ( & stage, & sig2, 42 ) ) ;

	mcr_Key_init_with ( & k1, MCR_ANY_KEY, 42, MCR_BOTH ) ;
	assert ( mcr_Stage_iskey ( & stage, & sig2, 42 ) ) ;
	mcr_Key_set_scan ( & k2, 4 ) ;
	assert ( ! mcr_Stage_iskey ( & stage, & sig2, 42 ) ) ;

	mcr_Key_init_with ( & k1, 42, MCR_ANY_KEY, MCR_BOTH ) ;
	assert ( mcr_Stage_iskey ( & stage, & sig2, 42 ) ) ;
	mcr_Key_set ( & k2, 4 ) ;
	assert ( ! mcr_Stage_iskey ( & stage, & sig2, 42 ) ) ;

	mcr_Key_init_with ( & k1, MCR_ANY_KEY, MCR_ANY_KEY, MCR_BOTH ) ;
	stage.modifiers = 42 ;
	assert ( mcr_Stage_iskey ( & stage, & sig2, 42 ) ) ;
	assert ( ! mcr_Stage_iskey ( & stage, & sig2, 0 ) ) ;

	printf ( "mcr_Stage_iskey - OK\n" ) ;
}

void test_Stage_ismovecursor ( )
{
	mcr_SpacePosition p1 = { } ;
	mcr_MoveCursor mc1, mc2 ;
	mcr_MoveCursor_init_with ( & mc1, p1, 0 ) ;
	mcr_MoveCursor_init_with ( & mc2, p1, 0 ) ;
	sig.type = & mcr_iMoveCursor ;
	sig.data = & mc1 ;
	mcr_Signal sig2 = { & mcr_iMoveCursor, & mc2 } ;
	stage.modifiers = MCR_ANY_MOD ;
	stage.measurement_error = 42 ;

	// absolute
	for ( int i = MCR_DIMENSION_MIN ; i < MCR_DIMENSION_CNT ;
			i ++ )
	{
		mcr_MoveCursor_set_position ( & mc1, i, 42 ) ;
		mcr_MoveCursor_set_position ( & mc2, i, 0 ) ;
		assert ( mcr_Stage_ismovecursor ( & stage, & sig2, 42 ) ) ;
		mcr_MoveCursor_set_position ( & mc2, i, -1 ) ;
		assert ( ! mcr_Stage_ismovecursor ( & stage, & sig2, 42 ) ) ;

		mcr_MoveCursor_set_position ( & mc2, i, 42 + 42 ) ;
		assert ( mcr_Stage_ismovecursor ( & stage, & sig2, 42 ) ) ;
		mcr_MoveCursor_set_position ( & mc2, i, 42 + 42 + 1 ) ;
		assert ( ! mcr_Stage_ismovecursor ( & stage, & sig2, 42 ) ) ;

		mcr_MoveCursor_set_position ( & mc1, i, 0 ) ;
		mcr_MoveCursor_set_position ( & mc2, i, 0 ) ;
	}
	// Check bad justify.
	mcr_MoveCursor_enable_justify ( & mc1, 1 ) ;
	assert ( ! mcr_Stage_ismovecursor ( & stage, & sig2, 42 ) ) ;
	mcr_MoveCursor_enable_justify ( & mc2, 1 ) ;
	// justify
	for ( int i = MCR_DIMENSION_MIN ; i < MCR_DIMENSION_CNT ;
			i ++ )
	{
		mcr_MoveCursor_set_position ( & mc1, i, 42 ) ;
		mcr_MoveCursor_set_position ( & mc2, i, 1 ) ;
		assert ( mcr_Stage_ismovecursor ( & stage, & sig2, 42 ) ) ;
		mcr_MoveCursor_set_position ( & mc2, i, -1 ) ;
		assert ( ! mcr_Stage_ismovecursor ( & stage, & sig2, 42 ) ) ;

		mcr_MoveCursor_set_position ( & mc1, i, -42 ) ;
		assert ( mcr_Stage_ismovecursor ( & stage, & sig2, 42 ) ) ;
		mcr_MoveCursor_set_position ( & mc2, i, 42 ) ;
		assert ( ! mcr_Stage_ismovecursor ( & stage, & sig2, 42 ) ) ;

		mcr_MoveCursor_set_position ( & mc1, i, 0 ) ;
		mcr_MoveCursor_set_position ( & mc2, i, 0 ) ;
	}

	printf ( "mcr_Stage_ismovecursor - OK\n" ) ;
}

void test_Stage_isnoop ( )
{
	mcr_NoOp n1, n2 ;
	mcr_Signal sig2 = { & mcr_iNoOp, & n2 } ;
	sig.data = & n1 ;
	sig.type = & mcr_iNoOp ;
	n1.tv_sec = 42 ;
	n2.tv_sec = 0 ;
	mcr_Stage_init_with ( & stage, 0, & sig, 42, MCR_ANY_MOD ) ;
	assert ( mcr_Stage_isnoop ( & stage, & sig2, 0 ) ) ;
	-- n2.tv_sec ;
	assert ( ! mcr_Stage_isnoop ( & stage, & sig2, 0 ) ) ;
	n2.tv_sec = n1.tv_sec + 42 ;
	assert ( mcr_Stage_isnoop ( & stage, & sig2, 0 ) ) ;
	++ n2.tv_sec ;
	assert ( ! mcr_Stage_isnoop ( & stage, & sig2, 0 ) ) ;

	sig.data = NULL ;
	stage.modifiers = 42 ;
	assert ( mcr_Stage_isnoop ( & stage, & sig2, 42 ) ) ;
	assert ( ! mcr_Stage_isnoop ( & stage, & sig2, 0 ) ) ;

	printf ( "mcr_Stage_isnoop - OK\n" ) ;
}

void test_Stage_isscroll ( )
{
	mcr_Dimensions p1 = { } ;
	mcr_Scroll scr1, scr2 ;
	mcr_Scroll_init_with ( & scr1, p1 ) ;
	mcr_Scroll_init_with ( & scr2, p1 ) ;
	sig.type = & mcr_iScroll ;
	sig.data = & scr1 ;
	mcr_Signal sig2 = { & mcr_iScroll, & scr2 } ;
	stage.modifiers = MCR_ANY_MOD ;
	stage.measurement_error = 42 ;

	for ( int i = MCR_DIMENSION_MIN ; i < MCR_DIMENSION_CNT ;
			i ++ )
	{
		mcr_Scroll_set_dimension ( & scr1, i, 42 ) ;
		mcr_Scroll_set_dimension ( & scr2, i, 1 ) ;
		assert ( mcr_Stage_isscroll ( & stage, & sig2, 42 ) ) ;
		mcr_Scroll_set_dimension ( & scr2, i, -1 ) ;
		assert ( ! mcr_Stage_isscroll ( & stage, & sig2, 42 ) ) ;

		mcr_Scroll_set_dimension ( & scr1, i, -42 ) ;
		assert ( mcr_Stage_isscroll ( & stage, & sig2, 42 ) ) ;
		mcr_Scroll_set_dimension ( & scr2, i, 42 ) ;
		assert ( ! mcr_Stage_isscroll ( & stage, & sig2, 42 ) ) ;

		mcr_Scroll_set_dimension ( & scr1, i, 0 ) ;
		mcr_Scroll_set_dimension ( & scr2, i, 0 ) ;
	}

	printf ( "mcr_Stage_isscroll - OK\n" ) ;
}

int main ( void )
{
	setup ( ) ;
	test_Stage_init_with ( ) ;

	test_Stage_count ( ) ;
	test_Stage_get_all ( ) ;

	test_Stage_isref ( ) ;
	test_Stage_isalarm ( ) ;
	test_Stage_isecho ( ) ;
	test_Stage_iskey ( ) ;
	test_Stage_ismovecursor ( ) ;
	test_Stage_isnoop ( ) ;
	test_Stage_isscroll ( ) ;

	printf ( "Stage test completed without assertion.\n" ) ;
	return 0 ;
}
