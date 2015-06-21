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

mcr_Dispatch * dispPt ;
int specCalled = 0 ;

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
	dispPt = mcr_Dispatch_get ( mcr_iAlarm.interface.id ) ;
	assert ( dispPt ) ;

	fprintf ( mcr_stdout, "Setup - OK\n" ) ;
}

void test_Dispatch_get ( )
{
	assert ( mcr_Dispatch_get ( mcr_ISignal_count ( ) ) ==
			mcr_Dispatch_get ( -1 ) ) ;
	// generic dispatch
	assert ( mcr_Dispatch_get ( -1 ) ) ;

	mcr_Dispatch disp ;
	mcr_DispatchAlarm_init ( & disp ) ;
	assert ( ! memcmp ( & disp, dispPt, sizeof ( mcr_Dispatch ) ) ) ;

	fprintf ( mcr_stdout, "mcr_Dispatch_get - OK\n" ) ;
}

void test_Dispatch_add_unspecific ( )
{
	mcr_Hot obj ;
	assert ( ! dispPt->generics.used ) ;

	mcr_Dispatch_add_unspecific ( dispPt, & obj ) ;
	assert ( dispPt->generics.used == 1 ) ;
	assert ( * ( ( mcr_Hot ** ) mcr_Array_at ( & dispPt->generics, 0 ) )
			== & obj ) ;

	mcr_Dispatch_add_unspecific ( dispPt, & obj ) ;
	assert ( dispPt->generics.used == 1 ) ;
	assert ( * ( ( mcr_Hot ** ) mcr_Array_at ( & dispPt->generics, 0 ) )
			== & obj ) ;

	dispPt->generics.used = 0 ;

	fprintf ( mcr_stdout, "mcr_Dispatch_add_unspecific - OK\n" ) ;
}

void chk_add_specific ( mcr_Signal * c,
		unsigned int d, mcr_Hot * b )
{
	UNUSED ( b ) ; UNUSED ( c ) ; UNUSED ( d ) ;
	specCalled = 1 ;
}

void test_Dispatch_add_specific ( )
{
	mcr_Hot obj ;
	dispPt->add_specific = NULL ;
	mcr_Dispatch_add_specific ( dispPt, NULL, 0, & obj ) ;
	dispPt->add_specific = chk_add_specific ;
	specCalled = 0 ;
	mcr_Dispatch_add_specific ( dispPt, NULL, 0, & obj ) ;
	assert ( specCalled == 1 ) ;
	assert ( ! dispPt->generics.used ) ;

	fprintf ( mcr_stdout, "mcr_Dispatch_add_specific - OK\n" ) ;
}

int dispatch_func ( mcr_Signal * s )
{
	UNUSED ( s ) ;
	return 1 ;
}

void test_Dispatch_enable ( )
{
	mcr_Dispatch_enable ( & mcr_iAlarm, 1 ) ;
	assert ( mcr_iAlarm.dispatch == mcr_dispatch ) ;
	assert ( mcr_iAlarm.dispatch_object ) ;
	mcr_Dispatch_enable ( & mcr_iAlarm, 0 ) ;
	assert ( ! mcr_iAlarm.dispatch ) ;
	assert ( ! mcr_iAlarm.dispatch_object ) ;

	fprintf ( mcr_stdout, "mcr_Dispatch_enable - OK\n" ) ;
}

void test_Dispatch_is_enabled ( )
{
	mcr_Dispatch_enable ( & mcr_iAlarm, 0 ) ;
	assert ( ! mcr_Dispatch_is_enabled ( & mcr_iAlarm ) ) ;
	mcr_Dispatch_enable ( & mcr_iAlarm, 1 ) ;
	assert ( mcr_Dispatch_is_enabled ( & mcr_iAlarm ) ) ;

	fprintf ( mcr_stdout, "mcr_Dispatch_is_enabled - OK\n" ) ;
}

void chk_remove ( mcr_Hot * b )
{
	UNUSED ( b ) ;
	specCalled = 1 ;
}

void test_Dispatch_remove ( )
{
	mcr_Hot obj ;
	dispPt->generics.used = 0 ;
	dispPt->remove_specific = NULL ;
	mcr_Dispatch_add_unspecific ( dispPt, & obj ) ;
	assert ( * ( mcr_Hot ** ) mcr_Array_at ( & dispPt->generics, 0 )
			== & obj ) ;
	mcr_Dispatch_remove ( & mcr_iAlarm, & obj ) ;
	assert ( ! dispPt->generics.used ) ;
	dispPt->remove_specific = chk_remove ;
	specCalled = 0 ;
	mcr_Dispatch_remove ( & mcr_iAlarm, & obj ) ;
	assert ( specCalled ) ;

	fprintf ( mcr_stdout, "mcr_Dispatch_remove - OK\n" ) ;
}

void chk_disp ( )
{
	specCalled = 1 ;
}

void test_Dispatch_clear ( )
{
	mcr_Hot hots [ SIZE ] ;
	for ( int i = 0 ; i < SIZE ; i++ )
	{
		mcr_Dispatch_add_unspecific ( dispPt, hots + i ) ;
	}
	assert ( dispPt->generics.used == SIZE ) ;
	mcr_Dispatch_clear ( & mcr_iAlarm ) ;
	assert ( ! dispPt->generics.used ) ;
	dispPt->clear_specific = chk_disp ;
	specCalled = 0 ;
	mcr_Dispatch_clear ( & mcr_iAlarm ) ;
	assert ( specCalled ) ;

	fprintf ( mcr_stdout, "mcr_Dispatch_clear - OK\n" ) ;
}

void chk_hot_disp ( mcr_Hot * a, mcr_Signal * b, unsigned int c )
{
	UNUSED ( a ) ; UNUSED ( b ) ;
	assert ( c == mcr_internalMods ) ;
	++ specCalled ;
}

int disp_spec ( mcr_Signal * b, unsigned int c )
{
	UNUSED ( b ) ;
	assert ( c == mcr_internalMods ) ;
	specCalled = 1 ;
	return 0 ;
}

unsigned int modify ( mcr_Signal * a, unsigned int c )
{
	assert ( a ) ;
	assert ( c == mcr_internalMods ) ;
	specCalled = 1 ;
	return c ;
}

void test_dispatch ( )
{
	mcr_Hot hots [ SIZE ] ;
	mcr_Signal sig ;
	sig.type = & mcr_iAlarm ;
	mcr_Dispatch * pt = mcr_Dispatch_get ( mcr_iAlarm.interface.id ) ;
	mcr_Dispatch_clear ( sig.type ) ;
//	pt->dispatch_specific = NULL ;
	pt->modifier = NULL ;
	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_Hot_init_with ( hots + i, & mcr_iHotkey, NULL, 0,
				0, chk_hot_disp, NULL ) ;
		mcr_Dispatch_add_unspecific ( pt, hots + i ) ;
	}
	specCalled = 0 ;
	mcr_dispatch ( & sig ) ;
	assert ( specCalled == SIZE ) ;
	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		hots [ i ].block = 1 ;
	}
	specCalled = 0 ;
	pt->modifier = modify ;
	mcr_dispatch ( & sig ) ;
	assert ( specCalled == 1 ) ;
	specCalled = 0 ;
	mcr_Dispatch_clear ( & mcr_iAlarm ) ;
	mcr_dispatch ( & sig ) ;
	assert ( specCalled == 1 ) ;

	pt->dispatch_specific = disp_spec ;
	specCalled = 0 ;
	mcr_dispatch ( & sig ) ;
	assert ( specCalled == 1 ) ;
	pt->modifier = NULL ;
	specCalled = 0 ;
	mcr_dispatch ( & sig ) ;
	assert ( specCalled == 1 ) ;

	fprintf ( mcr_stdout, "mcr_dispatch - OK\n" ) ;
}

void test_Dispatch_register ( )
{
	mcr_Dispatch disp ;
	memset ( & disp, 1, sizeof ( mcr_Dispatch ) ) ;
	mcr_Dispatch_register ( & disp, 0 ) ;
	mcr_Dispatch_register ( & disp, mcr_ISignal_count ( ) ) ;
	mcr_Dispatch_register ( & disp, mcr_ISignal_count ( ) << 1 ) ;
	assert ( ! memcmp ( mcr_Dispatch_get ( 0 ), & disp,
			sizeof ( mcr_Dispatch ) ) ) ;
	assert ( ! memcmp ( mcr_Dispatch_get ( mcr_ISignal_count ( ) ),
			& disp, sizeof ( mcr_Dispatch ) ) ) ;
	assert ( ! memcmp ( mcr_Dispatch_get ( mcr_ISignal_count ( ) << 1 ),
			& disp, sizeof ( mcr_Dispatch ) ) ) ;

	// Avoid problems with freeing dispatchers.
	mcr_Dispatch_init ( mcr_Dispatch_get ( 0 ) ) ;
	mcr_Dispatch_init ( mcr_Dispatch_get ( mcr_ISignal_count ( ) ) ) ;
	mcr_Dispatch_init ( mcr_Dispatch_get ( mcr_ISignal_count ( ) << 1 ) ) ;
	dispPt = mcr_Dispatch_get ( mcr_iAlarm.interface.id ) ;

	fprintf ( mcr_stdout, "mcr_Dispatch_register - OK\n" ) ;
}

mcr_Signal sig ;
unsigned int otherMods ;
int chk_disp_modified ( mcr_Signal * b,
		unsigned int c )
{
	assert ( b == & sig ) ;
	assert ( c == otherMods ) ;
	return 1 ;
}

void test_Dispatch_dispatch_modified ( )
{
	dispPt->dispatch_specific = chk_disp_modified ;
	mcr_Dispatch_dispatch_modified ( dispPt, & sig, & otherMods ) ;

	fprintf ( mcr_stdout, "mcr_Dispatch_dispatch_modified - OK\n" ) ;
}


void test_DispatchGeneric_add_specific_dispatch_specific ( )
{
	mcr_Dispatch * pt = mcr_Dispatch_get ( -1 ) ;
	assert ( pt ) ;
	mcr_Signal otherSig ;
	mcr_Hot obj ;
	mcr_Hot_init_with ( & obj, & mcr_iHotkey, NULL, 0,
			0, chk_hot_disp, NULL ) ;

	assert ( ! pt->generics.used ) ;
	mcr_DispatchGeneric_add_specific ( NULL, -1, & obj ) ;
	assert ( * ( mcr_Hot ** ) mcr_Array_at ( & pt->generics, 0 )
			== & obj ) ;
	mcr_Dispatch_clear ( NULL ) ;

	mcr_internalMods = MCR_ANY_MOD ;
	mcr_DispatchGeneric_add_specific ( & sig, mcr_internalMods, & obj ) ;
	specCalled = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( & sig, mcr_internalMods ) ;
	assert ( specCalled ) ;
	specCalled = 0 ;
	mcr_internalMods = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( & sig, mcr_internalMods ) ;
	assert ( specCalled ) ;
	specCalled = 0 ;
	mcr_internalMods = 42 ;
	mcr_DispatchGeneric_dispatch_specific ( & sig, mcr_internalMods ) ;
	assert ( specCalled ) ;
	specCalled = 0 ;
	mcr_internalMods = MCR_ANY_MOD ;
	mcr_DispatchGeneric_dispatch_specific ( NULL, mcr_internalMods ) ;
	assert ( ! specCalled ) ;
	specCalled = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( & otherSig, mcr_internalMods ) ;
	assert ( ! specCalled ) ;

	// Make sure hotkey removed for next test.
	mcr_Dispatch_clear ( NULL ) ;
	specCalled = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( & sig, mcr_internalMods ) ;
	assert ( ! specCalled ) ;
	mcr_internalMods = 42 ;
	mcr_DispatchGeneric_add_specific ( NULL, mcr_internalMods, & obj ) ;
	mcr_DispatchGeneric_dispatch_specific ( & sig, mcr_internalMods ) ;
	assert ( specCalled ) ;
	specCalled = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( NULL, mcr_internalMods ) ;
	assert ( specCalled ) ;
	specCalled = 0 ;
	mcr_internalMods = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( NULL, mcr_internalMods ) ;
	assert ( ! specCalled ) ;
	mcr_internalMods = MCR_ANY_MOD ;
	mcr_DispatchGeneric_dispatch_specific ( NULL, mcr_internalMods ) ;
	assert ( ! specCalled ) ;

	mcr_Dispatch_clear ( NULL ) ;

	fprintf ( mcr_stdout, "mcr_DispatchGeneric_add_specific - OK\n" ) ;
	fprintf ( mcr_stdout, "mcr_DispatchGeneric_dispatch_specific - OK\n" ) ;
}

void test_DispatchGeneric_remove_specific ( )
{
	mcr_Dispatch * pt = mcr_Dispatch_get ( -1 ) ;
	assert ( pt ) ;
	mcr_Hot obj ;
	mcr_Hot_init_with ( & obj, & mcr_iHotkey, NULL, 0, 0,
			chk_hot_disp, NULL ) ;

	mcr_Dispatch_clear ( NULL ) ;

	mcr_internalMods = 42 ;
	mcr_DispatchGeneric_add_specific ( & sig, 42, & obj ) ;
	mcr_DispatchGeneric_add_specific ( NULL, 42, & obj ) ;
	mcr_DispatchGeneric_add_specific ( & sig, MCR_ANY_MOD, & obj ) ;

	specCalled = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( & sig,
			mcr_internalMods ) ;
	assert ( specCalled ) ;
	mcr_DispatchGeneric_remove_specific ( & obj ) ;
	specCalled = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( & sig,
			mcr_internalMods ) ;
	assert ( ! specCalled ) ;

	fprintf ( mcr_stdout, "mcr_Dispatch_remove_specific - OK\n" ) ;
}

void test_DispatchGeneric_clear ( )
{
	// clear tested above.
	fprintf ( mcr_stdout, "mcr_Dispatch_clear - OK\n" ) ;
}

// Dispatch, 25 functions. Not testing init, free, initialize,
// or cleanup. Not testing init or free from DispatchGeneric
int main ( void )
{
	setup ( ) ;

	test_Dispatch_get ( ) ;
	test_Dispatch_add_unspecific ( ) ;
	test_Dispatch_add_specific ( ) ;
	test_Dispatch_enable ( ) ;
	test_Dispatch_is_enabled ( ) ;
	test_Dispatch_remove ( ) ;
	test_Dispatch_clear ( ) ;
	test_dispatch ( ) ;
	test_Dispatch_register ( ) ;
	test_Dispatch_dispatch_modified ( ) ;

	test_DispatchGeneric_add_specific_dispatch_specific ( ) ;
	test_DispatchGeneric_remove_specific ( ) ;
	test_DispatchGeneric_clear ( ) ;

	fprintf ( mcr_stdout, "Test complete without assertion error.\n" ) ;

	return 0 ;
}
