/* util/mcrstrings.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "util/mcrstrings.h"

int mcr_convert_bool ( const char * value, int * retVal )
{
	dassert ( retVal ) ;
	if ( value == NULL )
	{
		* retVal = 0 ;
		return 0 ;
	}
	while ( isspace ( * value ) ) ++ value ;
	if ( * value == '\0' )
	{
		* retVal = 0 ;
		return 0 ;
	}

	if ( isdigit ( * value ) )
	{
		if ( * value == '0' )
			* retVal = 0 ;
		else
			* retVal = 1 ;
		return 1 ;
	}

	if ( value [ 1 ] == '\0' )
	{
		if ( * value == 't' || * value == 'T' ||
				* value == 'y' || * value == 'Y' )
		{
			* retVal = 1 ;
		}
		else if ( * value == 'f' || * value == 'F' ||
				* value == 'n' || * value == 'N' )
		{
			* retVal = 0 ;
		}
	}
	else
	{
		if ( ! strcasecmp ( value, "true" ) ||
				! strcasecmp ( value, "yes" ) )
		{
			* retVal = 1 ;
		}
		else if ( ! strcasecmp ( value, "false" ) ||
				! strcasecmp ( value, "no" ) )
		{
			* retVal = 0 ;
		}
	}
	return 1 ;
}
