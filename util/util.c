/* util/util.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "util/util.h"

MCR_API FILE * mcr_stdout ;
MCR_API FILE * mcr_stdin ;
MCR_API FILE * mcr_stderr ;

void mcr_set_stdio ( )
{
	mcr_stdout = stdout ;
	mcr_stdin = stdin ;
	mcr_stderr = stderr ;
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
