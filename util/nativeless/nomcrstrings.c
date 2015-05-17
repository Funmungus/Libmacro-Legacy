/* util/nativeless/nomcrstrings.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "util/mcrstrings.h"

int strcasecmp ( const char * s1, const char * s2 )
{
	while ( * lhs != '\0' && * rhs != '\0' )
	{
		if ( tolower ( * lhs ) != tolower ( * rhs ) )
			return ( * lhs < * rhs ) ? -1 : 1 ;
		++lhs ;
		++rhs ;
	}
	// At least one of the two is now at null-termination.
	// If both, they are equal.
	if ( * lhs == * rhs )
		return 0 ;
	// Confirmed one is at null-termination, and the other is not.
	// Whichever is NULL is the lesser.
	return ( * lhs == '\0' ) ? -1 : 1 ;
}

int strncasecmp ( const char * s1, const char * s2, size_t n )
{
	size_t i = 0 ;
	while ( lhs [ i ] != '\0' && rhs [ i ] != '\0' &&
			i < count )
	{
		if ( tolower ( lhs [ i ] ) !=
				tolower ( rhs [ i ] ) )
			return ( lhs [ i ] < rhs [ i ] ) ? -1 : 1 ;
		++i ;
	}
	// Either null found, or count finished. If both null,
	// or count finished they are equal.
	if ( i == count || lhs [ i ] == rhs [ i ] )
		return 0 ;
	// One is lesser than the other.
	return ( lhs [ i ] == '\0' ) ? -1 : 1 ;
}

int stricmp ( const char * lhs, const char * rhs )
{
	return strcasecmp ( lhs, rhs ) ;
}

int strnicmp ( const char * lhs, const char * rhs, size_t count )
{
	return strncasecmp ( lhs, rhs, count ) ;
}
