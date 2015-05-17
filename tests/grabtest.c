/* grabtest.c - Test Linux input event grabbing.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "intercept/intercept.h"
# include "intercept/lnx/intercept.h"
# include <assert.h>

# define SIZE 64
char buffer [ SIZE ] ;
# define SAFESCAN(_buff_) scanf ( "%63s", _buff_ )

const char * myDev = "/dev/input/event4" ;
mcr_Hot mHot ;

void onComplete ( void )
{
	mcr_intercept_cleanup ( ) ;
	mcr_hotkey_cleanup ( ) ;
	mcr_signal_cleanup ( ) ;
}

void mTrigger ( mcr_Hot *, mcr_Signal *, unsigned int ) ;

void setup ( )
{
	mcr_reg_cleanup_filed ( onComplete, __FILE__ ) ;
	mcr_signal_initialize ( ) ;
	mcr_hotkey_initialize ( ) ;
	mcr_intercept_initialize ( ) ;

	mcr_Hot_init_with ( & mHot, 0, mTrigger, NULL ) ;

	printf ( "Setup - OK\n" ) ;
}

int mSelect ( ) ;

int main ( )
{
	setup ( ) ;

	mcr_Dispatch_add_unspecific ( mcr_Dispatch_get ( -1 ), & mHot ) ;
	mcr_intercept_grab ( myDev ) ;

	for ( int i = mSelect ( ) ;
		i != -1 ; i = mSelect ( ) )
	{
	}

	printf ( "Test complete without assertion error.\n" ) ;

	return 0 ;
}

void mTrigger ( mcr_Hot * hotPt, mcr_Signal * sigPt, unsigned int mods )
{
	assert ( hotPt == & mHot ) ;
	UNUSED ( mods ) ;
	printf ( "Triggered signal %s.\n", sigPt->type->name ) ;
}

int mSelect ( )
{
	printf ( "\nPlease make an integer selection.\n" ) ;
	printf ( "%7s%30s\n %7s%30s\n ",
		"-1 ) ", "exit",
		"0 ) ", "nothing" ) ;
	printf ( "Your selection : " ) ;

	int i = -1 ;
	SAFESCAN ( buffer ) ;
	i = atoi ( buffer ) ;
	return i ;
}
