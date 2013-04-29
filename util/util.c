
# include "util/util.h"

void mcr_reg_cleanup ( void ( * cleaner ) ( void ) )
{
	if ( atexit ( cleaner ) )
	{
		DMSG ( "%s\n", "Unable to register cleanup." ) ;
		exit ( 2 ) ;
	}
}

void mcr_reg_cleanup_filed ( void ( * cleaner ) ( void ),
		const char * fileName )
{
	if ( atexit ( cleaner ) )
	{
		DMSG ( "%s: %s\n", fileName, "Unable to register cleanup." ) ;
		exit ( 2 ) ;
	}
}

unsigned int mcr_bit_index ( unsigned int bitval )
{
	if ( bitval == 0 ) return -1 ; /* No on bit gives no index information.
			If not 0, there will definitely be some bit value contained */
	unsigned int index = 0 ;
	while ( ( bitval & 1 ) == 0 )
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
