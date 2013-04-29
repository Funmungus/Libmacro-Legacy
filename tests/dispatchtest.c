
# include "hotkey/hotkey.h"
# include <assert.h>

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

	printf ( "Setup - OK\n" ) ;
}

mcr_Dispatch disp ;
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

	printf ( "mcr_Dispatch - OK\n" ) ;
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

void test_Dispatch_is_enabled ( )
{
	mcr_signal_fnc dPt = NULL ;
	disp.dispatcher_pt = & dPt ;
	disp.enable_specific = disp.enable_unspecific = 1 ;
	assert ( ! mcr_Dispatch_is_enabled ( & disp ) ) ;
	dPt = mcr_dispatch ;
	assert ( mcr_Dispatch_is_enabled ( & disp ) ) ;
	dPt = ( void * ) 1 ;
	assert ( mcr_Dispatch_is_enabled ( & disp ) ) ;

	dPt = mcr_dispatch ;
	disp.enable_specific = 0 ;
	assert ( mcr_Dispatch_is_enabled ( & disp ) ) ;
	disp.enable_unspecific = 0 ;
	assert ( ! mcr_Dispatch_is_enabled ( & disp ) ) ;
	disp.enable_specific = 1 ;
	assert ( mcr_Dispatch_is_enabled ( & disp ) ) ;

	printf ( "mcr_Dispatch_is_enabled - OK\n" ) ;
}
int dispCalled = 0 ;
int dispFnc ( mcr_Dispatch * obj, mcr_Signal * sig, unsigned int * mods )
{
	UNUSED ( obj ) ;
	UNUSED ( sig ) ;
	UNUSED ( mods ) ;
}
void test_Dispatch_register ( )
{
	int sigCount = mcr_ISignal_count ( ) ;
	disp.dispatch_specific
	mcr_Dispatch_register ( & disp, sigCount ) ;
}
int freeCalled = 0 ;
void freeFnc ( mcr_Dispatch * obj )
{
	freeCalled = 1 ;
}
void test_Dispatch_free ( )
{
	disp.free = freeFnc ;
	mcr_Dispatch_free ( & disp ) ;
	assert ( freeCalled ) ;

	printf ( "mcr_Dispatch_free - OK\n" ) ;
}
void test_dispatch ( )
{
	printf ( "mcr_dispatch - OK\n" ) ;
}
void test_Dispatch_reset ( )
{
	printf ( "mcr_Dispatch_reset - OK\n" ) ;
}
void test_Dispatch_release ( )
{
	printf ( "mcr_Dispatch_release - OK\n" ) ;
}
void test_Dispatch_add ( )
{
	printf ( "mcr_Dispatch_add - OK\n" ) ;
}
void test_Dispatch_add_specific ( )
{
	printf ( "mcr_Dispatch_add_specific - OK\n" ) ;
}
void test_Dispatch_remove ( )
{
	printf ( "mcr_Dispatch_remove - OK\n" ) ;
}
void test_Dispatch_dispatch_modified ( )
{
	printf ( "mcr_Dispatch_dispatch_modified - OK\n" ) ;
}

void test_DispatchGeneric_free ( )
{
	printf ( "mcr_Dispatch_free - OK\n" ) ;
}
void test_DispatchGeneric_dispatch_specific ( )
{
	printf ( "mcr_Dispatch_dispatch_specific - OK\n" ) ;
}
void test_DispatchGeneric_add_specific ( )
{
	printf ( "mcr_Dispatch_add_specific - OK\n" ) ;
}
void test_DispatchGeneric_remove_specific ( )
{
	printf ( "mcr_Dispatch_remove_specific - OK\n" ) ;
}
void test_DispatchGeneric_release ( )
{
	printf ( "mcr_Dispatch_release - OK\n" ) ;
}


int main ( void )
{
	setup ( ) ;

	test_Dispatch_enable_auto ( ) ;
	test_Dispatch_enable ( ) ;
	test_Dispatch_is_enabled ( ) ;
	test_Dispatch_register ( ) ;
	test_Dispatch_free ( ) ;
	test_dispatch ( ) ;
	test_Dispatch_reset ( ) ;
	test_Dispatch_release ( ) ;
	test_Dispatch_add ( ) ;
	test_Dispatch_add_specific ( ) ;
	test_Dispatch_remove ( ) ;
	test_Dispatch_dispatch_modified ( ) ;

	test_DispatchGeneric_free ( ) ;
	test_DispatchGeneric_dispatch_specific ( ) ;
	test_DispatchGeneric_add_specific ( ) ;
	test_DispatchGeneric_remove_specific ( ) ;
	test_DispatchGeneric_release ( ) ;

	printf ( "Test complete without assertion error.\n" ) ;

	return 0 ;
}
