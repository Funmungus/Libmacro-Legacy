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

# include "util/util.h"

MCR_API FILE * mcr_out ;
MCR_API FILE * mcr_in ;
MCR_API FILE * mcr_err ;

void mcr_set_stdio ( )
{
	mcr_out = stdout ;
	mcr_in = stdin ;
	mcr_err = stderr ;
}

void mcr_reg_cleanup ( void ( * cleaner ) ( void ) )
{
	if ( atexit ( cleaner ) )
	{
		dmsg ;
		exit ( 2 ) ;
	}
}

unsigned int mcr_bit_index ( unsigned int bitval )
{
	if ( bitval == 0 ) return -1 ; /* No on bit gives no index information.
			If not 0, there will definitely be some bit value contained */
	unsigned int index = 0 ;
	while ( ( bitval & 0x01 ) == 0 )
	{
		++ index ;
		bitval >>= 1 ;
	}
	return index ;
}

unsigned int mcr_index_bit ( const unsigned int index )
{
	return 1 << index ;
}

const char * mcr_timestring ( )
{
	time_t curtime ;
	time ( & curtime ) ;
	return ctime ( & curtime ) ;
}
