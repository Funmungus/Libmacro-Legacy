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

int mcr_atob ( const char * value, int * retVal )
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


int mcr_String_insert ( mcr_Array * strPt, size_t index,
		const char * str, size_t len )
{
	dassert ( strPt ) ;
	dassert ( strPt->element_size == sizeof ( char ) ) ;
	if ( MCR_STR_EMPTY ( * strPt ) || index >= strPt->used - 1 )
	{
		return mcr_String_append ( strPt, str, len ) ;
	}
	size_t min = str ? strlen ( str ) : 0 ;
	if ( len < min ) min = len ;
	if ( ! mcr_Array_resize ( strPt, strPt->used + min ) )
	{
		dmsg ;
		return 0 ;
	}
	memmove ( strPt->array + index + min, strPt->array + index,
			strPt->used - index ) ;
	strPt->used = strPt->size ;
	memcpy ( strPt->array + index, str, min ) ;
	/* Assume null terminated already. */
	return 1 ;
}

int mcr_String_insert_char (mcr_Array * strPt, size_t index,
		const char c )
{
	dassert ( strPt ) ;
	dassert ( strPt->element_size == sizeof ( char ) ) ;
	if ( MCR_STR_EMPTY ( * strPt ) || index >= strPt->used - 1 )
	{
		return mcr_String_push ( strPt, c ) ;
	}
	return mcr_Array_insert ( strPt, index, & c ) ;
}

void mcr_String_remove (mcr_Array * strPt, size_t index,
		size_t count )
{
	dassert ( strPt ) ;
	dassert ( strPt->element_size == sizeof ( char ) ) ;
	// Index out of bounds.
	if ( index >= strPt->used - 1 )
	{
		dmsg ;
		return ;
	}
	// Nothing to move back, just null-terminate.
	else if ( index + count >= strPt->used - 1 )
	{
		strPt->array [ index ] = '\0' ;
		strPt->used = index + 1 ;
	}
	// Move into index, position at index + count, size is
	// array size - source position.
	else if ( index < strPt->used - 1 )
	{
		size_t moveCount = strPt->used - index - count ;
		memmove ( strPt->array + index, strPt->array + index + count,
				strPt->used - index - count ) ;
		strPt->used = index + moveCount ;
	}
}

int mcr_String_push (mcr_Array * strPt, const char c )
{
	dassert ( strPt ) ;
	dassert ( strPt->element_size == sizeof ( char ) ) ;
	char nc = '\0' ;
	if ( ! mcr_Array_push ( strPt, & nc ) )
	{
		dmsg ;
		return 0 ;
	}
	strPt->array [ strPt->used - 2 ] = c ;
	return 1 ;
}

char mcr_String_pop (mcr_Array * strPt )
{
	dassert ( strPt ) ;
	dassert ( strPt->element_size == sizeof ( char ) ) ;
	if ( MCR_STR_EMPTY ( * strPt ) )
		return '\0' ;
	char mem = strPt->array [ strPt->used - 2 ] ;
	-- strPt->used ;
	strPt->array [ strPt->used - 1 ] = '\0' ;
	return mem ;
}

int mcr_String_from_string (mcr_Array * strPt,
		const char * str )
{
	dassert ( strPt ) ;
	dassert ( strPt->element_size == sizeof ( char ) ) ;
	size_t min = str ? strlen ( str ) : 0 ;
	if ( ! mcr_Array_resize ( strPt, min + 1 ) )
	{
		dmsg ;
		return 0 ;
	}
	memcpy ( strPt->array, str, min ) ;
	strPt->array [ strPt->size - 1 ] = '\0' ;
	strPt->used = strPt->size ;
	return 1 ;
}

int mcr_String_nfrom_string (mcr_Array * strPt,
		const char * str, size_t len )
{
	dassert ( strPt ) ;
	dassert ( strPt->element_size == sizeof ( char ) ) ;
	size_t min = str ? strlen ( str ) : 0 ;
	if ( len < min ) min = len ;
	if ( ! mcr_Array_resize ( strPt, min + 1 ) )
	{
		dmsg ;
		return 0 ;
	}
	memcpy ( strPt->array, str, min ) ;
	strPt->array [ strPt->size - 1 ] = '\0' ;
	strPt->used = strPt->size ;
	return 1 ;
}

int mcr_String_append (mcr_Array * strPt,
		const char * str, size_t len )
{
	dassert ( strPt ) ;
	dassert ( strPt->element_size == sizeof ( char ) ) ;
	size_t min = str ? strlen ( str ) : 0 ;
	if ( len < min ) min = len ;
	if ( ! mcr_Array_resize ( strPt, min + 1 ) )
	{
		dmsg ;
		return 0 ;
	}
	memcpy ( strPt->array + strPt->used - 1, str, min ) ;
	strPt->array [ strPt->size - 1 ] = '\0' ;
	strPt->used = strPt->size ;
	return 1 ;
}

int mcr_String_compare ( const void * lhs, const void * rhs )
{
	dassert ( lhs ) ;
	dassert ( rhs ) ;
	const mcr_Array * lPt = lhs, * rPt = rhs ;
	if ( ! lPt->used || ! rPt->used )
		return lPt->used < rPt->used ? -1 : lPt->used > rPt->used ;
	return strcasecmp ( lPt->array, rPt->array ) ;
}
