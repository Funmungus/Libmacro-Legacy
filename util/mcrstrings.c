
# include "util/mcrstrings.h"

int mcr_convert_bool ( const char * value, int * retVal )
{
	if ( value == NULL || * value == '\0' )
	{
		 * retVal = 0 ;
		return 0 ;
	}
	if ( strcasecmp ( value, "true" ) )
	{
		 * retVal = 1 ;
		return 1 ;
	}
	if ( strcasecmp ( value, "false" ) )
	{
		 * retVal = 0 ;
		return 1 ;
	}

	const char * pt = value ;
	while ( isspace ( * pt ) ) ++pt ;

	if ( * pt == '0' || * pt == '\0' )
		 * retVal = 0 ;
	else
		 * retVal = 1 ;

	return 1 ;
}
