/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */


# include "hotkey/hotkey.h"
# include <assert.h>

# define SIZE 0x10

int obj = 42 ;
mcr_Hot hotty ;
mcr_Signal sig ;
mcr_ISignal isig ;

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

void test_Hot_init ( )
{
	mcr_Hot nullSig ;
	memset ( & nullSig, 0, sizeof ( nullSig ) ) ;
	memset ( & hotty, 1, sizeof ( hotty ) ) ;
	mcr_Hot_init ( & hotty ) ;
	assert ( ! memcmp ( & hotty, & nullSig, sizeof ( hotty ) ) ) ;

	printf ( "mcr_Hot_init - OK\n" ) ;
}

int triggerCalled = 0 ;
mcr_Signal * trigSignal ;
unsigned int trigMods ;
void triggerFnc ( mcr_Hot * hotPt, mcr_Signal * sigPt,
		unsigned int mods )
{
	assert ( hotPt == & hotty ) ;
	assert ( sigPt == trigSignal ) ;
	assert ( mods == trigMods ) ;
	triggerCalled = 1 ;
}
void test_Hot_init_with ( )
{
	mcr_Hot_init_with ( & hotty, obj, triggerFnc, & obj ) ;
	assert ( hotty.block == obj ) ;
	assert ( hotty.trigger == triggerFnc ) ;
	assert ( hotty.data == & obj ) ;

	printf ( "mcr_Hot_init_with - OK\n" ) ;
}
void test_Hot_trigger ( )
{
	mcr_Hot_init_with ( & hotty, 0, triggerFnc, & obj ) ;
	triggerCalled = 0 ;
	trigSignal = NULL ;
	trigMods = 0 ;
	int block = mcr_Hot_trigger ( & hotty, trigSignal, trigMods ) ;
	assert ( ! block ) ;
	hotty.block = 1 ;
	trigMods = obj ;
	block = mcr_Hot_trigger ( & hotty, trigSignal, trigMods ) ;
	assert ( block ) ;
	trigSignal = & sig ;
	mcr_Hot_trigger ( & hotty, trigSignal, trigMods ) ;
	trigMods = 0 ;
	mcr_Hot_trigger ( & hotty, trigSignal, trigMods ) ;

	printf ( "mcr_Hot_trigger - OK\n" ) ;
}
void trigIncFnc ( mcr_Hot * hotPt, mcr_Signal * sigPt,
		unsigned int mods )
{
	++ triggerCalled ;
	UNUSED ( hotPt ) ;
	UNUSED ( sigPt ) ;
	UNUSED ( mods ) ;
}

void test_Hot_trigger_array ( )
{
	mcr_Hot hotset [ SIZE ] ;
	mcr_Hot * hotPtSet [ SIZE ] ;
	for ( int i = 0 ; i < SIZE ; i ++ )
	{
		mcr_Hot_init_with ( hotset + i, 0, trigIncFnc, NULL ) ;
		hotPtSet [ i ] = hotset + i ;
	}
	triggerCalled = 0 ;
	mcr_Hot_trigger_array ( hotPtSet, SIZE, trigSignal, trigMods ) ;
	assert ( triggerCalled == SIZE ) ;

	printf ( "mcr_Hot_trigger_array - OK\n" ) ;
}

int main ( void )
{
	setup ( ) ;

	test_Hot_init ( ) ;
	test_Hot_init_with ( ) ;
	test_Hot_trigger ( ) ;
	test_Hot_trigger_array ( ) ;

	printf ( "Test complete without assertion error.\n" ) ;

	return 0 ;
}
