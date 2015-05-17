/* util/lnx/lnxmcrstrings.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "util/mcrstrings.h"

// case implemented on lnx
int stricmp ( const char * lhs, const char * rhs )
{
	return strcasecmp ( lhs, rhs ) ;
}

int strnicmp ( const char * lhs, const char * rhs, size_t count )
{
	return strncasecmp ( lhs, rhs, count ) ;
}
