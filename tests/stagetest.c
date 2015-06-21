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
	mcr_Stage_free ( & stage ) ;
	mcr_hotkey_cleanup ( ) ;
	mcr_signal_cleanup ( ) ;
}

void setup ( )
{
	mcr_set_stdio ( ) ;
	mcr_reg_cleanup ( cleanup ) ;
	mcr_signal_initialize ( ) ;
	mcr_hotkey_initialize ( ) ;
	assert ( mcr_Stage_ismeGeneric == mcr_Stage_isref ) ;
	assert ( mcr_Stage_resembleGeneric == mcr_Stage_resemble_ref ) ;

	fprintf ( mcr_stdout, "test setup - OK\n" ) ;
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

mcr_isme_fnc resembles [ ] =
{
	NULL, mcr_Stage_resemble_echo,
	mcr_Stage_resemble_key,
	mcr_Stage_resemble_movecursor, NULL,
	mcr_Stage_resemble_scroll,
	mcr_Stage_resemble_mod
} ;

const size_t kLen = sizeof ( knowns ) / sizeof ( mcr_ISignal * ) ;

void reset ( )
{
	mcr_Stage_free ( & stage ) ;
	mcr_Stage_init ( & stage ) ;
}

void test_Stage_init_with ( )
{
	mcr_Signal nulSig = { 0, { 0, 0 } } ;
	mcr_Stage_init_with ( & stage, 0, NULL, 5, 42 ) ;
	assert ( stage.activated == 0 ) ;
	assert ( stage.blocking == 0 ) ;
	assert ( ! memcmp ( & stage.intercept, & nulSig,
			sizeof ( mcr_Signal ) ) ) ;
	assert ( stage.isme == mcr_Stage_ismeGeneric ) ;
	assert ( stage.resembles == mcr_Stage_resembleGeneric ) ;
	assert ( stage.measurement_error == 5 ) ;
	assert ( stage.modifiers == 42 ) ;
	stage.isme = stage.resembles = NULL ;
	stage.modifiers = 0 ;
	mcr_Stage_set ( & stage, NULL, 42 ) ;
	assert ( stage.isme == mcr_Stage_ismeGeneric ) ;
	assert ( stage.resembles == mcr_Stage_resembleGeneric ) ;
	assert ( stage.modifiers == 42 ) ;
	stage.isme = stage.resembles = NULL ;
	stage.modifiers = 0 ;
	mcr_Stage_init_with ( & stage, 0, & sig, 5, 42 ) ;
	assert ( stage.activated == 0 ) ;
	assert ( stage.blocking == 0 ) ;
	assert ( stage.intercept.data.data == & sig ) ;
	assert ( stage.isme == mcr_Stage_ismeGeneric ) ;
	assert ( stage.resembles == mcr_Stage_resembleGeneric ) ;
	assert ( stage.measurement_error == 5 ) ;
	assert ( stage.modifiers == 42 ) ;
	stage.isme = stage.resembles = NULL ;
	stage.modifiers = 0 ;
	mcr_Stage_set ( & stage, & sig, 42 ) ;
	assert ( stage.isme == mcr_Stage_ismeGeneric ) ;
	assert ( stage.resembles == mcr_Stage_resembleGeneric ) ;
	assert ( stage.modifiers == 42 ) ;
	for ( unsigned int i = 0 ; i < kLen ; i ++ )
	{
		unsigned int chker = i % 2 ;
		sig.type = knowns [ i ] ;
		mcr_Stage_init_with ( & stage, ( int ) chker, & sig,
				chker, chker ) ;
		assert ( stage.activated == 0 ) ;
		assert ( stage.blocking == ( int ) chker ) ;
		assert ( ! mcr_Signal_compare ( & stage.intercept, & sig ) ) ;
		assert ( stage.isme == fncs [ i ] ) ;
		assert ( stage.resembles == resembles [ i ] ) ;
		assert ( stage.measurement_error == chker ) ;
		assert ( stage.modifiers == chker ) ;
		stage.isme = stage.resembles = NULL ;
		stage.modifiers = 42 ;
		mcr_Stage_set ( & stage, & sig, chker ) ;
		assert ( stage.blocking == ( int ) chker ) ;
		assert ( stage.isme == fncs [ i ] ) ;
		assert ( stage.resembles == resembles [ i ] ) ;
		assert ( stage.modifiers == chker ) ;
	}

	fprintf ( mcr_stdout, "mcr_Stage_init_with - OK\n" ) ;
	fprintf ( mcr_stdout, "mcr_Stage_set - OK\n" ) ;
}

void test_Stage_count ( )
{
	assert ( mcr_Stage_count ( ) == mcr_ISignal_count ( ) ) ;
	fprintf ( mcr_stdout, "mcr_Stage_count - OK\n" ) ;
}

void test_Stage_get_all ( )
{
	mcr_isme_fnc all [ kLen ] ;
	mcr_isme_fnc all_resembles [ kLen ] ;
	mcr_Stage_get_all ( all, all_resembles, kLen ) ;
	for ( unsigned int i = 0 ; i < kLen ; i++ )
	{
		// Function is the same as isme function for that id.
		assert ( all [ i ] == mcr_Stage_isme_for ( i ) ) ;
		assert ( all_resembles [ i ] == mcr_Stage_resemble_for ( i ) ) ;
	}
	fprintf ( mcr_stdout, "mcr_Stage_isme_for - OK\n" ) ;
	fprintf ( mcr_stdout, "mcr_Stage_get_all - OK\n" ) ;
}

void test_Stage_isref ( )
{
	mcr_Signal_init ( & stage.intercept, NULL ) ;
	reset ( ) ;
	stage.modifiers = MCR_ANY_MOD ;
	assert ( mcr_Stage_isref ( & stage, NULL, 42 ) ) ;
	stage.modifiers = 0 ;
	assert ( ! mcr_Stage_isref ( & stage, & sig, 42 ) ) ;
	assert ( mcr_Stage_isref ( & stage, & sig, 0 ) ) ;
	stage.intercept.data.data = & sig ;
	assert ( mcr_Stage_isref ( & stage, & sig, 0 ) ) ;
	assert ( ! mcr_Stage_isref ( & stage, & sig, 42 ) ) ;
	stage.modifiers = MCR_ANY_MOD ;
	assert ( mcr_Stage_isref ( & stage, & sig, 0 ) ) ;
	assert ( mcr_Stage_isref ( & stage, & sig, 42 ) ) ;

	fprintf ( mcr_stdout, "mcr_Stage_isref - OK\n" ) ;
}

// alarm tested by mktime ( struct tm * )
void test_Stage_isalarm ( )
{
	mcr_Alarm alm ;
	sig.data.data = & alm ;
	sig.type = & mcr_iAlarm ;
	time_t original = time ( NULL ) ;
	time_t t1 = original ;
	alm = * localtime ( & t1 ) ;
	mcr_Stage_init_with ( & stage, 0, & sig, 42, MCR_ANY_MOD ) ;
	t1 -= 42 ;
	alm = * localtime ( & t1 ) ;
	assert ( mcr_Stage_isalarm ( & stage, & sig, 0 ) ) ;
	-- t1 ;
	alm = * localtime ( & t1 ) ;
	assert ( ! mcr_Stage_isalarm ( & stage, & sig, 0 ) ) ;
	t1 = original + 42 ;
	alm = * localtime ( & t1 ) ;
	assert ( mcr_Stage_isalarm ( & stage, & sig, 0 ) ) ;
	++ t1 ;
	alm = * localtime ( & t1 ) ;
	assert ( ! mcr_Stage_isalarm ( & stage, & sig, 0 ) ) ;

	t1 = original ;
	alm = * localtime ( & t1 ) ;
	stage.modifiers = 42 ;
	assert ( mcr_Stage_isalarm ( & stage, & sig, 42 ) ) ;
	assert ( ! mcr_Stage_isalarm ( & stage, & sig, 0 ) ) ;

	fprintf ( mcr_stdout, "mcr_Stage_isalarm - OK\n" ) ;
}

void test_Stage_isecho ( )
{
	mcr_HIDEcho echo ;
	mcr_Echo_init_with ( & echo, MCR_ANY_MOD ) ;
	sig.type = & mcr_iHIDEcho ;
	sig.data.data = & echo ;
	mcr_Stage_set ( & stage, & sig, MCR_ANY_MOD ) ;
	mcr_Echo_set ( & echo, 0 ) ;
	assert ( mcr_Stage_isecho ( & stage, & sig, 0 ) ) ;
	assert ( mcr_Stage_isecho ( & stage, & sig, 42 ) ) ;
	mcr_Echo_set ( stage.intercept.data.data, 4 ) ;
	assert ( ! mcr_Stage_isecho ( & stage, & sig, 0 ) ) ;
	mcr_Echo_set ( & echo, 4 ) ;
	assert ( mcr_Stage_isecho ( & stage, & sig, 0 ) ) ;
	stage.modifiers = 42 ;
	assert ( mcr_Stage_isecho ( & stage, & sig, 42 ) ) ;
	assert ( ! mcr_Stage_isecho ( & stage, & sig, 0 ) ) ;

	fprintf ( mcr_stdout, "mcr_Stage_isecho - OK\n" ) ;
}

void test_Stage_iskey ( )
{
	mcr_Key key ;
	mcr_Key_init_with ( & key, MCR_ANY_KEY, MCR_ANY_KEY, MCR_BOTH ) ;
	sig.type = & mcr_iKey ;
	sig.data.data = & key ;
	mcr_Stage_set ( & stage, & sig, MCR_ANY_MOD ) ;
	mcr_Key_init_with ( & key, 42, 42, MCR_DOWN ) ;
	assert ( mcr_Stage_iskey ( & stage, & sig, 42 ) ) ;

	mcr_Key_init_with ( stage.intercept.data.data, MCR_ANY_KEY,
			MCR_ANY_KEY, MCR_DOWN ) ;
	assert ( mcr_Stage_iskey ( & stage, & sig, 42 ) ) ;
	mcr_Key_set_up_type ( & key, MCR_UP ) ;
	assert ( ! mcr_Stage_iskey ( & stage, & sig, 42 ) ) ;

	mcr_Key_init_with ( stage.intercept.data.data, MCR_ANY_KEY,
			42, MCR_BOTH ) ;
	assert ( mcr_Stage_iskey ( & stage, & sig, 42 ) ) ;
	mcr_Key_set_scan ( & key, 4 ) ;
	assert ( ! mcr_Stage_iskey ( & stage, & sig, 42 ) ) ;

	mcr_Key_init_with ( stage.intercept.data.data, 42,
			MCR_ANY_KEY, MCR_BOTH ) ;
	assert ( mcr_Stage_iskey ( & stage, & sig, 42 ) ) ;
	mcr_Key_set ( & key, 4 ) ;
	assert ( ! mcr_Stage_iskey ( & stage, & sig, 42 ) ) ;

	mcr_Key_init_with ( stage.intercept.data.data, MCR_ANY_KEY,
			MCR_ANY_KEY, MCR_BOTH ) ;
	stage.modifiers = 42 ;
	assert ( mcr_Stage_iskey ( & stage, & sig, 42 ) ) ;
	assert ( ! mcr_Stage_iskey ( & stage, & sig, 0 ) ) ;

	fprintf ( mcr_stdout, "mcr_Stage_iskey - OK\n" ) ;
}

void test_Stage_ismovecursor ( )
{
	mcr_SpacePosition p1 = { } ;
	mcr_MoveCursor mc ;
	mcr_MoveCursor_init_with ( & mc, p1, 0 ) ;
	sig.type = & mcr_iMoveCursor ;
	sig.data.data = & mc ;
	mcr_Stage_set ( & stage, & sig, MCR_ANY_MOD ) ;
	stage.measurement_error = 42 ;

	// absolute
	for ( int i = MCR_DIMENSION_MIN ; i < MCR_DIMENSION_CNT ;
			i ++ )
	{
		mcr_MoveCursor_set_position ( stage.intercept.data.data, i, 42 ) ;
		mcr_MoveCursor_set_position ( & mc, i, 0 ) ;
		assert ( mcr_Stage_ismovecursor ( & stage, & sig, 42 ) ) ;
		mcr_MoveCursor_set_position ( & mc, i, -1 ) ;
		assert ( ! mcr_Stage_ismovecursor ( & stage, & sig, 42 ) ) ;

		mcr_MoveCursor_set_position ( & mc, i, 42 + 42 ) ;
		assert ( mcr_Stage_ismovecursor ( & stage, & sig, 42 ) ) ;
		mcr_MoveCursor_set_position ( & mc, i, 42 + 42 + 1 ) ;
		assert ( ! mcr_Stage_ismovecursor ( & stage, & sig, 42 ) ) ;

		mcr_MoveCursor_set_position ( stage.intercept.data.data, i, 0 ) ;
		mcr_MoveCursor_set_position ( & mc, i, 0 ) ;
	}
	// Check bad justify.
	mcr_MoveCursor_enable_justify ( & mc, 1 ) ;
	assert ( ! mcr_Stage_ismovecursor ( & stage, & sig, 42 ) ) ;
	mcr_MoveCursor_enable_justify ( stage.intercept.data.data, 1 ) ;
	// justify
	for ( int i = MCR_DIMENSION_MIN ; i < MCR_DIMENSION_CNT ;
			i ++ )
	{
		mcr_MoveCursor_set_position ( stage.intercept.data.data, i, 42 ) ;
		mcr_MoveCursor_set_position ( & mc, i, 1 ) ;
		assert ( mcr_Stage_ismovecursor ( & stage, & sig, 42 ) ) ;
		mcr_MoveCursor_set_position ( & mc, i, -1 ) ;
		assert ( ! mcr_Stage_ismovecursor ( & stage, & sig, 42 ) ) ;

		mcr_MoveCursor_set_position ( stage.intercept.data.data, i,
				-42 ) ;
		assert ( mcr_Stage_ismovecursor ( & stage, & sig, 42 ) ) ;
		mcr_MoveCursor_set_position ( & mc, i, 42 ) ;
		assert ( ! mcr_Stage_ismovecursor ( & stage, & sig, 42 ) ) ;

		mcr_MoveCursor_set_position ( & mc, i, 0 ) ;
		mcr_MoveCursor_set_position ( & mc, i, 0 ) ;
	}

	fprintf ( mcr_stdout, "mcr_Stage_ismovecursor - OK\n" ) ;
}

void test_Stage_isnoop ( )
{
	mcr_NoOp noop = { 43, 0 } ;
	sig.data.data = & noop ;
	sig.type = & mcr_iNoOp ;
	mcr_Stage_set ( & stage, & sig, MCR_ANY_MOD ) ;
	stage.measurement_error = 42 ;
	noop.tv_sec = 1 ;
	assert ( mcr_Stage_isnoop ( & stage, & sig, 0 ) ) ;
	-- noop.tv_sec ;
	assert ( ! mcr_Stage_isnoop ( & stage, & sig, 0 ) ) ;
	noop.tv_sec = 43 + 42 ;
	assert ( mcr_Stage_isnoop ( & stage, & sig, 0 ) ) ;
	++ noop.tv_sec ;
	assert ( ! mcr_Stage_isnoop ( & stage, & sig, 0 ) ) ;

	noop.tv_sec = 42 ;
	stage.modifiers = 42 ;
	assert ( mcr_Stage_isnoop ( & stage, & sig, 42 ) ) ;
	assert ( ! mcr_Stage_isnoop ( & stage, & sig, 0 ) ) ;

	fprintf ( mcr_stdout, "mcr_Stage_isnoop - OK\n" ) ;
}

void test_Stage_isscroll ( )
{
	mcr_Dimensions p1 = { } ;
	mcr_Scroll scr ;
	mcr_Scroll_init_with ( & scr, p1 ) ;
	sig.type = & mcr_iScroll ;
	sig.data.data = & scr ;
	mcr_Stage_set ( & stage, & sig, MCR_ANY_MOD ) ;
	stage.measurement_error = 42 ;

	for ( int i = MCR_DIMENSION_MIN ; i < MCR_DIMENSION_CNT ;
			i ++ )
	{
		mcr_Scroll_set_dimension ( stage.intercept.data.data, i, 42 ) ;
		mcr_Scroll_set_dimension ( & scr, i, 1 ) ;
		assert ( mcr_Stage_isscroll ( & stage, & sig, 42 ) ) ;
		mcr_Scroll_set_dimension ( & scr, i, -1 ) ;
		assert ( ! mcr_Stage_isscroll ( & stage, & sig, 42 ) ) ;

		mcr_Scroll_set_dimension ( stage.intercept.data.data, i, -42 ) ;
		assert ( mcr_Stage_isscroll ( & stage, & sig, 42 ) ) ;
		mcr_Scroll_set_dimension ( & scr, i, 42 ) ;
		assert ( ! mcr_Stage_isscroll ( & stage, & sig, 42 ) ) ;

		mcr_Scroll_set_dimension ( stage.intercept.data.data, i, 0 ) ;
		mcr_Scroll_set_dimension ( & scr, i, 0 ) ;
	}

	fprintf ( mcr_stdout, "mcr_Stage_isscroll - OK\n" ) ;
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

	fprintf ( mcr_stdout, "Stage test completed without assertion.\n" ) ;
	return 0 ;
}
