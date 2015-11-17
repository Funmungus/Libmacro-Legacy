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

# include "macro.h"
# include <assert.h>

# define SIZE 64
char buffer [ SIZE ] ;
char argbuffer [ SIZE ] ;
char * argset [ 2 ] = { argbuffer, NULL } ;
# define SAFESCAN(_buff_) fscanf ( mcr_in, "%63s", _buff_ )

mcr_Command cmd ;
mcr_Signal cmdSig ;

void onComplete ( void )
{
}

void setup ( )
{
	mcr_set_stdio ( ) ;
	mcr_reg_cleanup ( onComplete ) ;
	mcr_library_initialize ( 1 ) ;
//	mcr_standard_enable ( 1 ) ;

	mcr_Command_init ( & cmd ) ;
	mcr_Signal_init_with ( & cmdSig, & mcr_iCommand, & cmd, 0, 0 ) ;

	fprintf ( mcr_out, "Setup - OK\n" ) ;
}

void mSelect ( ) ;

int main ( int argc, char ** argv )
{
	setup ( ) ;

	mSelect ( ) ;
	while ( buffer [ 0 ] != '\0' && buffer [ 0 ] != '\r' && buffer [ 0 ] != '\n' &&
			 strncmp ( buffer, "-1", 2 ) )
	{
		fprintf ( mcr_out, "Set file: %s\n", buffer ) ;
		mcr_Command_set_file ( & cmd, buffer, 0 ) ;
		fprintf ( mcr_out, "Set args: %s\n", argbuffer ) ;
		mcr_Command_set_args ( & cmd, argset, 0 ) ;
		fprintf ( mcr_out, "Executing: %s %s\n", buffer, argbuffer ) ;
		mcr_send ( & cmdSig ) ;
		mSelect ( ) ;
	}

	fprintf ( mcr_out, "Test complete without assertion error.\n" ) ;

	return 0 ;
}

void mSelect ( )
{
	fprintf ( mcr_out, "\nPlease enter a command.\n" ) ;
	fprintf ( mcr_out, "$:" ) ;
	SAFESCAN ( buffer ) ;
	fprintf ( mcr_out, "\nPlease enter an argument.\n" ) ;
	fprintf ( mcr_out, "$:" ) ;
	SAFESCAN ( argbuffer ) ;
}
