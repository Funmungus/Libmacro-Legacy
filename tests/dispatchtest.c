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

mcr_Dispatch disp ;
int specCalled = 0 ;

void onComplete ( void )
{
	mcr_Dispatch_free ( & disp ) ;
	mcr_hotkey_cleanup ( ) ;
	mcr_signal_cleanup ( ) ;
}

void setup ( )
{
	mcr_reg_cleanup_filed ( onComplete, __FILE__ ) ;
	mcr_signal_initialize ( ) ;
	mcr_hotkey_initialize ( ) ;

	printf ( "Setup - OK\n" ) ;
}

void test_Dispatch_get ( )
{
	assert ( ! mcr_Dispatch_get ( mcr_ISignal_count ( ) ) ) ;
	// generic dispatch
	assert ( mcr_Dispatch_get ( -1 ) ) ;

	mcr_Dispatch * pt ;
	mcr_DispatchAlarm_init ( & disp ) ;
	pt = mcr_Dispatch_get ( mcr_iAlarm.id ) ;
	assert ( pt ) ;
	assert ( ! memcmp ( & disp, pt, sizeof ( mcr_Dispatch ) ) ) ;

	printf ( "mcr_Dispatch_get - OK\n" ) ;
}

void test_Dispatch_add_unspecific ( )
{
	mcr_Hot obj ;
	assert ( ! disp.generics.used ) ;

	mcr_Dispatch_add_unspecific ( & disp, & obj ) ;
	assert ( disp.generics.used == 1 ) ;
	assert ( * ( ( mcr_Hot ** ) mcr_Array_at ( & disp.generics, 0 ) )
			== & obj ) ;

	mcr_Dispatch_add_unspecific ( & disp, & obj ) ;
	assert ( disp.generics.used == 1 ) ;
	assert ( * ( ( mcr_Hot ** ) mcr_Array_at ( & disp.generics, 0 ) )
			== & obj ) ;

	mcr_Dispatch_add_unspecific ( & disp, NULL ) ;
	assert ( disp.generics.used == 1 ) ;
	assert ( * ( ( mcr_Hot ** ) mcr_Array_at ( & disp.generics, 0 ) )
			== & obj ) ;

	mcr_Dispatch_clear ( & disp ) ;

	printf ( "mcr_Dispatch_add_unspecific - OK\n" ) ;
}

void chk_add_specific ( mcr_Dispatch * a, mcr_Hot * b, mcr_Signal * c,
		unsigned int d )
{
	UNUSED ( a ) ; UNUSED ( b ) ; UNUSED ( c ) ; UNUSED ( d ) ;
	specCalled = 1 ;
}

void test_Dispatch_add_specific ( )
{
	mcr_Hot obj ;
	disp.add_specific = NULL ;
	mcr_Dispatch_add_specific ( & disp, & obj, NULL, 0 ) ;
	disp.add_specific = chk_add_specific ;
	specCalled = 0 ;
	mcr_Dispatch_add_specific ( & disp, & obj, NULL, 0 ) ;
	assert ( specCalled == 1 ) ;
	assert ( ! disp.generics.used ) ;

	printf ( "mcr_Dispatch_add_specific - OK\n" ) ;
}

void test_Dispatch_enable_auto ( )
{
	mcr_signal_fnc dPt = NULL ;
	disp.dispatcher_pt = & dPt ;
	disp.enable_specific = disp.enable_unspecific = 1 ;
	mcr_Dispatch_enable_auto ( & disp ) ;
	assert ( dPt == mcr_dispatch ) ;

	dPt = NULL ;
	disp.enable_specific = 0 ;
	mcr_Dispatch_enable_auto ( & disp ) ;
	assert ( dPt == mcr_dispatch ) ;

	dPt = mcr_dispatch ;
	disp.enable_unspecific = 0 ;
	mcr_Dispatch_enable_auto ( & disp ) ;
	assert ( ! dPt ) ;

	dPt = NULL ;
	disp.enable_specific = 1 ;
	mcr_Dispatch_enable_auto ( & disp ) ;
	assert ( dPt == mcr_dispatch ) ;

	printf ( "mcr_Dispatch_auto - OK\n" ) ;
}

int dispatch_func ( mcr_Signal * s )
{
	UNUSED ( s ) ;
	return 1 ;
}

void test_Dispatch_enable_auto_to ( )
{
	mcr_signal_fnc dPt = NULL ;
	disp.dispatcher_pt = & dPt ;
	disp.enable_specific = disp.enable_unspecific = 1 ;
	mcr_Dispatch_enable_auto_to ( & disp, dispatch_func ) ;
	assert ( dPt == dispatch_func ) ;

	dPt = NULL ;
	disp.enable_specific = 0 ;
	mcr_Dispatch_enable_auto_to ( & disp, dispatch_func ) ;
	assert ( dPt == dispatch_func ) ;

	dPt = dispatch_func ;
	disp.enable_unspecific = 0 ;
	mcr_Dispatch_enable_auto_to ( & disp, dispatch_func ) ;
	assert ( ! dPt ) ;

	dPt = NULL ;
	disp.enable_specific = 1 ;
	mcr_Dispatch_enable_auto_to ( & disp, dispatch_func ) ;
	assert ( dPt == dispatch_func ) ;

	printf ( "mcr_Dispatch_auto_to - OK\n" ) ;
}

void test_Dispatch_enable ( )
{
	mcr_signal_fnc dPt = NULL ;
	disp.dispatcher_pt = & dPt ;
	mcr_Dispatch_enable ( & disp, 1 ) ;
	assert ( dPt == mcr_dispatch ) ;
	mcr_Dispatch_enable ( & disp, 0 ) ;
	assert ( ! dPt ) ;

	printf ( "mcr_Dispatch_enable - OK\n" ) ;
}

void test_Dispatch_enable_to ( )
{
	mcr_signal_fnc dPt = NULL ;
	disp.dispatcher_pt = & dPt ;
	mcr_Dispatch_enable_to ( & disp, 1, dispatch_func ) ;
	assert ( dPt == dispatch_func ) ;
	mcr_Dispatch_enable_to ( & disp, 0, dispatch_func ) ;
	assert ( ! dPt ) ;

	printf ( "mcr_Dispatch_enable_to - OK\n" ) ;
}

void test_Dispatch_is_enabled ( )
{
	mcr_signal_fnc dPt = NULL ;
	disp.dispatcher_pt = & dPt ;
	disp.enable_specific = disp.enable_unspecific = 1 ;
	assert ( ! mcr_Dispatch_is_enabled ( & disp ) ) ;
	dPt = mcr_dispatch ;
	assert ( mcr_Dispatch_is_enabled ( & disp ) ) ;
	dPt = ( void * ) 1 ;
	assert ( mcr_Dispatch_is_enabled ( & disp ) < 0 ) ;

	dPt = mcr_dispatch ;
	disp.enable_specific = 0 ;
	assert ( mcr_Dispatch_is_enabled ( & disp ) ) ;
	disp.enable_unspecific = 0 ;
	assert ( ! mcr_Dispatch_is_enabled ( & disp ) ) ;
	disp.enable_specific = 1 ;
	assert ( mcr_Dispatch_is_enabled ( & disp ) ) ;

	printf ( "mcr_Dispatch_is_enabled - OK\n" ) ;
}

void test_Dispatch_is_enabled_to ( )
{
	mcr_signal_fnc dPt = NULL ;
	disp.dispatcher_pt = & dPt ;
	disp.enable_specific = disp.enable_unspecific = 1 ;
	assert ( ! mcr_Dispatch_is_enabled_to ( & disp, dispatch_func ) ) ;
	dPt = dispatch_func ;
	assert ( mcr_Dispatch_is_enabled_to ( & disp, dispatch_func ) ) ;
	dPt = mcr_dispatch ;
	assert ( mcr_Dispatch_is_enabled_to ( & disp, dispatch_func ) < 0 ) ;

	dPt = dispatch_func ;
	disp.enable_specific = 0 ;
	assert ( mcr_Dispatch_is_enabled_to ( & disp, dispatch_func ) ) ;
	disp.enable_unspecific = 0 ;
	assert ( ! mcr_Dispatch_is_enabled_to ( & disp, dispatch_func ) ) ;
	disp.enable_specific = 1 ;
	assert ( mcr_Dispatch_is_enabled_to ( & disp, dispatch_func ) ) ;

	printf ( "mcr_Dispatch_enabled_to - OK\n" ) ;
}

void chk_remove ( mcr_Dispatch * a, mcr_Hot * b )
{
	UNUSED ( a ) ; UNUSED ( b ) ;
	specCalled = 1 ;
}

void test_Dispatch_remove ( )
{
	mcr_Hot obj ;
	disp.generics.used = 0 ;
	disp.remove_specific = NULL ;
	mcr_Dispatch_add_unspecific ( & disp, & obj ) ;
	assert ( * ( mcr_Hot ** ) mcr_Array_at ( & disp.generics, 0 )
			== & obj ) ;
	mcr_Dispatch_remove ( & disp, & obj ) ;
	assert ( ! disp.generics.used ) ;
	disp.remove_specific = chk_remove ;
	specCalled = 0 ;
	mcr_Dispatch_remove ( & disp, & obj ) ;
	assert ( specCalled ) ;

	printf ( "mcr_Dispatch_remove - OK\n" ) ;
}

void test_Dispatch_reset ( )
{
	mcr_Hot hots [ SIZE ] ;
	mcr_signal_fnc dPt = NULL ;
	disp.dispatcher_pt = & dPt ;
	disp.generics.used = 0 ;
	disp.enable_specific = 1 ;
	for ( int i = 0 ; i < SIZE ; i++ )
	{
		mcr_Dispatch_add_unspecific ( & disp, hots + i ) ;
	}
	assert ( disp.generics.used == SIZE ) ;
	mcr_Dispatch_reset ( & disp ) ;
	assert ( ! disp.generics.used ) ;
	assert ( dPt == mcr_dispatch ) ;
	disp.enable_specific = 0 ;
	disp.enable_unspecific = 0 ;
	mcr_Dispatch_reset ( & disp ) ;
	assert ( ! disp.generics.used ) ;
	assert ( dPt == NULL ) ;

	printf ( "mcr_Dispatch_reset - OK\n" ) ;
}

void chk_disp ( mcr_Dispatch * a )
{
	UNUSED ( a ) ;
	specCalled = 1 ;
}

void test_Dispatch_clear ( )
{
	mcr_Hot hots [ SIZE ] ;
	for ( int i = 0 ; i < SIZE ; i++ )
	{
		mcr_Dispatch_add_unspecific ( & disp, hots + i ) ;
	}
	assert ( disp.generics.used == SIZE ) ;
	mcr_Dispatch_clear ( & disp ) ;
	assert ( ! disp.generics.used ) ;
	disp.clear_specific = chk_disp ;
	specCalled = 0 ;
	mcr_Dispatch_clear ( & disp ) ;
	assert ( specCalled ) ;

	printf ( "mcr_Dispatch_clear - OK\n" ) ;
}

void chk_hot_disp ( mcr_Hot * a, mcr_Signal * b, unsigned int c )
{
	UNUSED ( a ) ; UNUSED ( b ) ;
	assert ( c == mcr_internalMods ) ;
	++ specCalled ;
}

int disp_spec ( mcr_Dispatch * a, mcr_Signal * b, unsigned int c )
{
	UNUSED ( a ) ; UNUSED ( b ) ;
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
	mcr_Dispatch * pt = mcr_Dispatch_get ( mcr_iAlarm.id ) ;
	mcr_Dispatch_clear ( pt ) ;
	pt->dispatch_specific = NULL ;
	pt->modifier = NULL ;
	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_Hot_init_with ( hots + i, 0, chk_hot_disp, NULL ) ;
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
	mcr_Dispatch_clear ( pt ) ;
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

	printf ( "mcr_dispatch - OK\n" ) ;
}

void test_Dispatch_register ( )
{
	mcr_Dispatch_free ( & disp ) ;
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
	mcr_Dispatch_init ( & disp ) ;
	mcr_Dispatch_register ( & disp, 0 ) ;
	mcr_Dispatch_register ( & disp, mcr_ISignal_count ( ) ) ;
	mcr_Dispatch_register ( & disp, mcr_ISignal_count ( ) << 1 ) ;

	printf ( "mcr_Dispatch_register - OK\n" ) ;
}

mcr_Signal sig ;
unsigned int otherMods ;
int chk_disp_modified ( mcr_Dispatch * a, mcr_Signal * b,
		unsigned int c )
{
	assert ( a == & disp ) ;
	assert ( b == & sig ) ;
	assert ( c == otherMods ) ;
	return 1 ;
}

void test_Dispatch_dispatch_modified ( )
{
	disp.dispatch_specific = chk_disp_modified ;
	mcr_Dispatch_dispatch_modified ( & disp, & sig, & otherMods ) ;

	printf ( "mcr_Dispatch_dispatch_modified - OK\n" ) ;
}


void test_DispatchGeneric_add_specific_dispatch_specific ( )
{
	mcr_Dispatch * pt = mcr_Dispatch_get ( -1 ) ;
	assert ( pt ) ;
	mcr_Signal otherSig ;
	mcr_Hot obj ;
	mcr_Hot_init_with ( & obj, 0, chk_hot_disp, NULL ) ;

	assert ( ! pt->generics.used ) ;
	mcr_DispatchGeneric_add_specific ( pt, & obj, NULL, -1 ) ;
	assert ( * ( mcr_Hot ** ) mcr_Array_at ( & pt->generics, 0 )
			== & obj ) ;
	mcr_Dispatch_clear ( pt ) ;

	mcr_internalMods = MCR_ANY_MOD ;
	mcr_DispatchGeneric_add_specific ( pt, & obj, & sig, mcr_internalMods ) ;
	specCalled = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( pt, & sig, mcr_internalMods ) ;
	assert ( specCalled ) ;
	specCalled = 0 ;
	mcr_internalMods = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( pt, & sig, mcr_internalMods ) ;
	assert ( specCalled ) ;
	specCalled = 0 ;
	mcr_internalMods = 42 ;
	mcr_DispatchGeneric_dispatch_specific ( pt, & sig, mcr_internalMods ) ;
	assert ( specCalled ) ;
	specCalled = 0 ;
	mcr_internalMods = MCR_ANY_MOD ;
	mcr_DispatchGeneric_dispatch_specific ( pt, NULL, mcr_internalMods ) ;
	assert ( ! specCalled ) ;
	specCalled = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( pt, & otherSig, mcr_internalMods ) ;
	assert ( ! specCalled ) ;

	// Make sure hotkey removed for next test.
	mcr_Dispatch_clear ( pt ) ;
	specCalled = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( pt, & sig, mcr_internalMods ) ;
	assert ( ! specCalled ) ;
	mcr_internalMods = 42 ;
	mcr_DispatchGeneric_add_specific ( pt, & obj, NULL, mcr_internalMods ) ;
	mcr_DispatchGeneric_dispatch_specific ( pt, & sig, mcr_internalMods ) ;
	assert ( specCalled ) ;
	specCalled = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( pt, NULL, mcr_internalMods ) ;
	assert ( specCalled ) ;
	specCalled = 0 ;
	mcr_internalMods = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( pt, NULL, mcr_internalMods ) ;
	assert ( ! specCalled ) ;
	mcr_internalMods = MCR_ANY_MOD ;
	mcr_DispatchGeneric_dispatch_specific ( pt, NULL, mcr_internalMods ) ;
	assert ( ! specCalled ) ;

	mcr_Dispatch_clear ( pt ) ;

	printf ( "mcr_DispatchGeneric_add_specific - OK\n" ) ;
	printf ( "mcr_DispatchGeneric_dispatch_specific - OK\n" ) ;
}

void test_DispatchGeneric_remove_specific ( )
{
	mcr_Dispatch * pt = mcr_Dispatch_get ( -1 ) ;
	assert ( pt ) ;
	mcr_Hot obj ;
	mcr_Hot_init_with ( & obj, 0, chk_hot_disp, NULL ) ;

	mcr_Dispatch_clear ( pt ) ;

	mcr_internalMods = 42 ;
	mcr_DispatchGeneric_add_specific ( pt, & obj, & sig, 42 ) ;
	mcr_DispatchGeneric_add_specific ( pt, & obj, NULL, 42 ) ;
	mcr_DispatchGeneric_add_specific ( pt, & obj, & sig, MCR_ANY_MOD ) ;

	specCalled = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( pt, & sig,
			mcr_internalMods ) ;
	assert ( specCalled ) ;
	mcr_DispatchGeneric_remove_specific ( pt, & obj ) ;
	specCalled = 0 ;
	mcr_DispatchGeneric_dispatch_specific ( pt, & sig,
			mcr_internalMods ) ;
	assert ( ! specCalled ) ;

	printf ( "mcr_Dispatch_remove_specific - OK\n" ) ;
}

void test_DispatchGeneric_clear ( )
{
	// clear tested above.
	printf ( "mcr_Dispatch_clear - OK\n" ) ;
}

// Dispatch, 25 functions. Not testing init, free, initialize,
// or cleanup. Not testing init or free from DispatchGeneric
int main ( void )
{
	setup ( ) ;

	test_Dispatch_get ( ) ;
	test_Dispatch_add_unspecific ( ) ;
	test_Dispatch_add_specific ( ) ;
	test_Dispatch_enable_auto ( ) ;
	test_Dispatch_enable_auto_to ( ) ;
	test_Dispatch_enable ( ) ;
	test_Dispatch_enable_to ( ) ;
	test_Dispatch_is_enabled ( ) ;
	test_Dispatch_is_enabled_to ( ) ;
	test_Dispatch_remove ( ) ;
	test_Dispatch_reset ( ) ;
	test_Dispatch_clear ( ) ;
	test_dispatch ( ) ;
	test_Dispatch_register ( ) ;
	test_Dispatch_dispatch_modified ( ) ;

	test_DispatchGeneric_add_specific_dispatch_specific ( ) ;
	test_DispatchGeneric_remove_specific ( ) ;
	test_DispatchGeneric_clear ( ) ;

	printf ( "Test complete without assertion error.\n" ) ;

	return 0 ;
}
