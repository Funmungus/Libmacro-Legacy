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

# include "intercept/intercept.h"
# include "intercept/lnx/intercept.h"
# include <assert.h>

# define SIZE 64
char buffer [ SIZE ] ;
# define SAFESCAN(_buff_) fscanf ( mcr_stdin, "%63s", _buff_ )

const char * myDev = NULL ;
mcr_Hot mHot ;

void onComplete ( void )
{
	mcr_Hot_free ( & mHot ) ;
	mcr_intercept_cleanup ( ) ;
	mcr_hotkey_cleanup ( ) ;
	mcr_signal_cleanup ( ) ;
}

void mTrigger ( mcr_Hot *, mcr_Signal *, unsigned int ) ;

void setup ( )
{
	mcr_set_stdio ( ) ;
	mcr_reg_cleanup ( onComplete ) ;
	mcr_signal_initialize ( ) ;
	mcr_standard_enable ( 1 ) ;
	mcr_hotkey_initialize ( ) ;
	mcr_intercept_initialize ( ) ;
	mcr_intercept_add_grab ( myDev ) ;
	mcr_intercept_enable ( 1 ) ;
	mcr_iAlarm.dispatch = mcr_iHIDEcho.dispatch =
	mcr_iKey.dispatch = mcr_iMoveCursor.dispatch =
	mcr_iNoOp.dispatch = mcr_iScroll.dispatch = mcr_dispatch ;

	mcr_Hot_init_with ( & mHot, & mcr_iHot, 0, 0, 0, mTrigger, NULL ) ;

	fprintf ( mcr_out, "Setup - OK\n" ) ;
}

int mSelect ( ) ;

int main ( int argc, char ** argv )
{
	if ( argc < 2 )
		return 1 ;
	myDev = argv [ 1 ] ;
	setup ( ) ;

	mcr_Dispatch_add_unspecific ( mcr_Dispatch_get ( -1 ), & mHot ) ;
	mcr_Dispatch_get ( -1 )->enable_unspecific = 1 ;

	for ( int i = mSelect ( ) ;
		i != -1 ; i = mSelect ( ) )
	{
	}

	fprintf ( mcr_out, "Test complete without assertion error.\n" ) ;

	return 0 ;
}

void mTrigger ( mcr_Hot * hotPt, mcr_Signal * sigPt, unsigned int mods )
{
	assert ( hotPt == & mHot ) ;
	UNUSED ( mods ) ;
	const char * name = mcr_ISignal_get_name
			( sigPt->type->interface.id ) ;
	fprintf ( mcr_out, "Triggered signal %llu:%s.\n",
			( long long unsigned ) sigPt->type->interface.id,
			name ? name : "NULL" ) ;
}

int mSelect ( )
{
	fprintf ( mcr_out, "\nPlease make an integer selection.\n" ) ;
	fprintf ( mcr_out, "%7s%30s\n %7s%30s\n ",
		"-1 ) ", "exit",
		"0 ) ", "nothing" ) ;
	fprintf ( mcr_out, "Your selection : " ) ;

	int i = -1 ;
	SAFESCAN ( buffer ) ;
	i = atoi ( buffer ) ;
	return i ;
}
