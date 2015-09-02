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
