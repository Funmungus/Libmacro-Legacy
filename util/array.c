/* util/array.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "util/array.h"

void mcr_Array_init ( mcr_Array * arrPt, size_t elementSize )
{
	if ( ! arrPt )
	{
		dmsg ;
		return ;
	}
	memset ( arrPt, 0, sizeof ( mcr_Array ) ) ;
	arrPt->element_size = elementSize ;
}

void mcr_Array_free ( mcr_Array * arrPt )
{
	if ( ! arrPt )
	{
		dmsg ;
		return ;
	}
	free ( arrPt->array ) ;
	mcr_Array_init ( arrPt, arrPt->element_size ) ;
}

void mcr_Array_free_foreach ( mcr_Array * arrPt, ... )
{
	mcr_Array_free ( arrPt ) ;
}

void mcr_Array_sort ( mcr_Array * arrPt, mcr_compare_fnc cmp )
{
	dassert ( arrPt ) ;
	dassert ( cmp ) ;
	MCR_ARR_SORT ( arrPt, cmp ) ;
}

int mcr_Array_push ( mcr_Array * arrPt, const void * elementPt )
{
	dassert ( arrPt ) ;
	// All full.
	if ( arrPt->used == arrPt->size )
	{
		if ( ! mcr_Array_resize ( arrPt, arrPt->size ?
				arrPt->size << 1 : 2 ) )
		{
			dmsg ;
			return 0 ;
		}
	}
	++ arrPt->used ;
	void * arrPos = MCR_ARR_AT ( arrPt, arrPt->used - 1 ) ;
	if ( elementPt )
		memcpy ( arrPos, elementPt, arrPt->element_size ) ;
	else
		memset ( arrPos, 0, arrPt->element_size ) ;
	return 1 ;
}

int mcr_Array_push_unique ( mcr_Array * arrPt, const void * elementPt )
{
	dassert ( arrPt ) ;
	dassert ( elementPt ) ;
	void * it = MCR_ARR_AT ( arrPt, 0 ) ;
	if ( it )
	{
		void * end = MCR_ARR_END ( arrPt ) ;
		while ( it < end )
		{
			// Exists, do not repeat.
			if ( ! memcmp ( it, elementPt, arrPt->element_size ) )
			{
				return 1 ;
			}
			it = MCR_ARR_NEXT ( arrPt, it ) ;
		}
	}
	return mcr_Array_push ( arrPt, elementPt ) ;
}

int mcr_Array_add_sorted ( mcr_Array * arrPt, const void * elementPt,
		mcr_compare_fnc cmp )
{
	dassert ( arrPt ) ;
	dassert ( elementPt ) ;
	dassert ( cmp ) ;
	if ( arrPt->used )
	{
		void * found = bsearch ( elementPt, arrPt->array, arrPt->used,
				arrPt->element_size, cmp ) ;
		if ( found )
			return 1 ;
	}
	if ( ! mcr_Array_push ( arrPt, elementPt ) )
	{
		dmsg ;
		return 0 ;
	}
	mcr_Array_sort ( arrPt, cmp ) ;
	return 1 ;
}

int mcr_Array_set ( mcr_Array * arrPt, size_t pos, const void * elementPt )
{
	dassert ( arrPt ) ;
	void * found = MCR_ARR_AT ( arrPt, pos ) ;
	if ( found )
	{
		if ( elementPt )
			memcpy ( found, elementPt, arrPt->element_size ) ;
		else
			memset ( found, 0, arrPt->element_size ) ;
	}
	else
	{
		return mcr_Array_insert ( arrPt, pos, elementPt ) ;
	}
	return 1 ;
}

int mcr_Array_insert ( mcr_Array * arrPt, size_t pos,
		const void * elementPt )
{
	dassert ( arrPt ) ;
	void * p1, * p2 ;
	// Resize up to pos
	if ( pos >= arrPt->size )
	{
		// allocation failure
		if ( ! mcr_Array_resize ( arrPt, pos + 1 ) )
		{
			dmsg ;
			return 0 ;
		}
	}
	// Insert in-between, but need more room.
	else if ( arrPt->used == arrPt->size )
	{
		if ( ! mcr_Array_resize ( arrPt, arrPt->size ?
				arrPt->size << 1 : 2 ) )
		{
			dmsg ;
			return 0 ;
		}
	}
	// Insert after used
	if ( pos >= arrPt->used )
	{
		// # elements to fill is position - old used
		size_t bytes = ( pos - arrPt->used ) * arrPt->element_size ;
		p1 = MCR_ARR_AT ( arrPt, arrPt->used ) ;
		if ( p1 )
			memset ( p1, 0, bytes ) ;
		arrPt->used = pos + 1 ;
	}
	// Insert in-between
	else
	{
		p1 = MCR_ARR_AT ( arrPt, pos ) ;
		p2 = MCR_ARR_AT ( arrPt, pos + 1 ) ;
		if ( p1 && p2 )
		{
			// # elements to move is previous used count - insert index
			size_t bytes = ( ( arrPt->used ++ ) - pos )
					 * arrPt->element_size ;
			memmove ( p2, p1, bytes ) ;
		}
		else
		{
			dmsg ;
		}
	}
	/* We are now either copying into pos beyond previous end,
	 * or copying into new empty space
	 * ensured array size > pos
	 **/
	void * arrPos = MCR_ARR_AT ( arrPt, pos ) ;
	if ( elementPt )
	{
		memcpy ( arrPos, elementPt, arrPt->element_size ) ;
	}
	else
	{
		memset ( arrPos, 0, arrPt->element_size ) ;
	}
	return 1 ;
}

int mcr_Array_insert_filled ( mcr_Array * arrPt, size_t pos,
		const void * elementPt, const void * fillerElementPt )
{
	dassert ( arrPt ) ;
	size_t prevUsed = arrPt->used ;
	// Structure unchanged, do not fill.
	if ( ! mcr_Array_insert ( arrPt, pos, elementPt ) )
	{
		dmsg ;
		return 0 ;
	}
	// Fill only items from arr [ prevUsed ] to arr [ curUsed-2 ] inclusive
	// arr [ curUsed-1 ] is the newly inserted item.
	void * arrPos = MCR_ARR_AT ( arrPt, prevUsed ) ;
	void * elPos = MCR_ARR_AT ( arrPt, pos ) ;
	// If prevUsed >= insert position, the following will not happen.
	if ( fillerElementPt )
	{
		for ( ; arrPos < elPos ;
				arrPos = MCR_ARR_NEXT ( arrPt, arrPos ) )
		{
			memcpy ( arrPos, fillerElementPt, arrPt->element_size ) ;
		}
	}
	else
	{
		for ( ; arrPos < elPos ;
				arrPos = MCR_ARR_NEXT ( arrPt, arrPos ) )
		{
			memset ( arrPos, 0, arrPt->element_size ) ;
		}
	}
	return 1 ;
}

int mcr_Array_from_array ( mcr_Array * arrPt, const void * arraySource,
		size_t count )
{
	dassert ( arrPt ) ;
	dassert ( arraySource ) ;
	if ( ! mcr_Array_resize ( arrPt, count ) )
	{
		dmsg ;
		return 0 ;
	}
	memcpy ( arrPt->array, arraySource,
			count * arrPt->element_size ) ;
	arrPt->used = count ;
	return 1 ;
}

int mcr_Array_append ( mcr_Array * arrPt, const void * arraySource,
		size_t count )
{
	dassert ( arrPt ) ;
	dassert ( arraySource ) ;
	if ( arrPt->used + count > arrPt->size )
	{
		// If resizing, one additional to allow additional push if needed.
		if ( ! mcr_Array_resize ( arrPt, arrPt->used + count + 1 ) )
		{
			dmsg ;
			return 0 ;
		}
	}
	size_t index = arrPt->used ;
	arrPt->used += count ;
	void * it = MCR_ARR_AT ( arrPt, index ) ;
	if ( it )
		memcpy ( it, arraySource, count * arrPt->element_size ) ;
	return 1 ;
}

void * mcr_Array_pop ( mcr_Array * arrPt )
{
	dassert ( arrPt ) ;
	if ( ! arrPt->used )
		return NULL ;
	return arrPt->array + ( --arrPt->used * arrPt->element_size ) ;
}

void mcr_Array_remove ( mcr_Array * arrPt, size_t pos )
{
	dassert ( arrPt ) ;
	unsigned char * found = MCR_ARR_AT ( arrPt, pos ) ;
	if ( ! found )
		return ;
	unsigned char * end = mcr_Array_pop ( arrPt ) ;
	size_t byteNum = end - found ;
	memmove ( found, found + arrPt->element_size, byteNum ) ;
}

void mcr_Array_remove_all ( mcr_Array * arrPt,
		const void * removeElementPt )
{
	dassert ( arrPt ) ;
	dassert ( removeElementPt ) ;
	void * it = MCR_ARR_AT ( arrPt, 0 ) ;
	size_t index = 0 ;
	while ( index < arrPt->used )
	{
		if ( ! memcmp ( it, removeElementPt, arrPt->element_size ) )
		{
			mcr_Array_remove ( arrPt, index ) ;
		}
		// Only increase index when not removing current index.
		else
		{
			it = MCR_ARR_NEXT ( arrPt, it ) ;
			++index ; // next
		}
	}
}

void mcr_Array_remove_sorted ( mcr_Array * arrPt,
		const void * removeElementPt, mcr_compare_fnc cmp )
{
	dassert ( arrPt ) ;
	dassert ( removeElementPt ) ;
	dassert ( cmp ) ;
	void * found = mcr_Array_find ( arrPt, removeElementPt, cmp ) ;
	if ( found )
	{
		mcr_Array_remove ( arrPt, MCR_ARR_INDEXOF ( arrPt, found ) ) ;
		MCR_ARR_SORT ( arrPt, cmp ) ;
	}
}

void * mcr_Array_at ( mcr_Array * arrPt, size_t pos )
{
	dassert ( arrPt ) ;
	return MCR_ARR_AT ( arrPt, pos ) ;
}

void * mcr_Array_find ( mcr_Array * arrPt, const void * elementPt,
		mcr_compare_fnc cmp )
{
	dassert ( arrPt ) ;
	dassert ( elementPt ) ;
	dassert ( cmp ) ;
	return MCR_ARR_FIND ( arrPt, elementPt, cmp ) ;
}

void * mcr_Array_next ( mcr_Array * arrPt, void * posPt )
{
	dassert ( arrPt ) ;
	return MCR_ARR_NEXT ( arrPt, posPt ) ;
}

void * mcr_Array_prev ( mcr_Array * arrPt, void * posPt )
{
	dassert ( arrPt ) ;
	return MCR_ARR_PREV ( arrPt, posPt ) ;
}

void * mcr_Array_end ( mcr_Array * arrPt )
{
	dassert ( arrPt ) ;
	return MCR_ARR_END ( arrPt ) ;
}

void mcr_Array_for_each ( const mcr_Array * arrPt,
		mcr_iterate_fnc iterateFnc, ... )
{
	dassert ( arrPt ) ;
	if ( ! arrPt->used ) return ;
	va_list lst ;
	void * it = MCR_ARR_AT ( arrPt, 0 ) ;
	if ( ! it ) return ;
	void * end = MCR_ARR_END ( arrPt ) ;
	va_start ( lst, iterateFnc ) ;
	while ( it < end )
	{
		va_list cpy ;
		va_copy ( cpy, lst ) ;
		iterateFnc ( it, cpy ) ;
		va_end ( cpy ) ;
		it = MCR_ARR_NEXT ( arrPt, it ) ;
	}
	va_end ( lst ) ;
}

void mcr_Array_trim ( mcr_Array * arrPt )
{
	dassert ( arrPt ) ;
	mcr_Array_resize ( arrPt, arrPt->used ) ;
}

int mcr_Array_resize ( mcr_Array * arrPt, size_t newSize )
{
	dassert ( arrPt ) ;
	// No change, do nothing.
	if ( arrPt->size == newSize )
		return 1 ;

	if ( ! newSize || ! arrPt->element_size )
	{
		mcr_Array_free ( arrPt ) ;
		return 1 ;
	}
	char * newSet = NULL ;
	if ( arrPt->array )
		newSet = realloc ( arrPt->array, newSize * arrPt->element_size ) ;
	else
	{
		newSet = malloc ( newSize * arrPt->element_size ) ;
		arrPt->array = newSet ;
		if ( arrPt->array )
		{
			memset ( arrPt->array, 0, newSize * arrPt->element_size ) ;
			arrPt->size = newSize ;
		}
		return arrPt->array != NULL ;
	}

	// Check for errors. If errors, do not change structure.
	if ( ! newSet )
	{
		dmsg ;
		return 0 ;
	}

	arrPt->array = newSet ;
	arrPt->size = newSize ;
	if ( arrPt->used > arrPt->size )
		arrPt->used = arrPt->size ;
	return 1 ;
}

void mcr_Array_print ( const mcr_Array * arrPt )
{
	dassert ( arrPt ) ;
	fprintf ( mcr_stdout, "Array %p: used %llu, size %llu, "
			"element size %llu, array: 0x...\n", arrPt,
			( long long unsigned ) arrPt->used,
			( long long unsigned ) arrPt->size,
			( long long unsigned ) arrPt->element_size ) ;
	const void * end = MCR_ARR_END ( arrPt ) ;
	for ( const unsigned char * i = MCR_ARR_AT ( arrPt, 0 ) ;
			( void * ) i < end ; i++ )
	{
		fprintf ( mcr_stdout, "%x%s", * i, i + 1 == end ? "" : " " ) ;
	}
	fprintf ( mcr_stdout, "\n" ) ;
}

int mcr_String_insert ( mcr_Array * arrPt, size_t index,
		const char * str, size_t len )
{
	dassert ( arrPt ) ;
	dassert ( arrPt->element_size == sizeof ( char ) ) ;
	if ( MCR_STR_ISEMPTY ( arrPt ) || index >= arrPt->used - 1 )
	{
		return mcr_String_append ( arrPt, str, len ) ;
	}
	size_t min = strlen ( str ) ;
	if ( len < min ) min = len ;
	if ( ! mcr_Array_resize ( arrPt, arrPt->used + min ) )
	{
		dmsg ;
		return 0 ;
	}
	memmove ( arrPt->array + index + min, arrPt->array + index,
			arrPt->used - index ) ;
	arrPt->used = arrPt->size ;
	memcpy ( arrPt->array + index, str, min ) ;
	/* Assume null terminated already. */
	return 1 ;
}

int mcr_String_insert_char ( mcr_Array * arrPt, size_t index,
		const char c )
{
	dassert ( arrPt ) ;
	dassert ( arrPt->element_size == sizeof ( char ) ) ;
	if ( MCR_STR_ISEMPTY ( arrPt ) || index >= arrPt->used - 1 )
	{
		return mcr_String_push ( arrPt, c ) ;
	}
	return mcr_Array_insert ( arrPt, index, & c ) ;
}

void mcr_String_remove ( mcr_Array * arrPt, size_t index,
		size_t count )
{
	dassert ( arrPt ) ;
	dassert ( arrPt->element_size == sizeof ( char ) ) ;
	// Index out of bounds.
	if ( index >= arrPt->used - 1 )
	{
		dmsg ;
		return ;
	}
	// Nothing to move back, just null-terminate.
	else if ( index + count >= arrPt->used - 1 )
	{
		arrPt->array [ index ] = '\0' ;
		arrPt->used = index + 1 ;
	}
	// Move into index, position at index + count, size is
	// array size - source position.
	else if ( index < arrPt->used - 1 )
	{
		size_t moveCount = arrPt->used - index - count ;
		memmove ( arrPt->array + index, arrPt->array + index + count,
				arrPt->used - index - count ) ;
		arrPt->used = index + moveCount ;
	}
}

int mcr_String_push ( mcr_Array * arrPt, const char c )
{
	dassert ( arrPt ) ;
	dassert ( arrPt->element_size == sizeof ( char ) ) ;
	char nc = '\0' ;
	if ( ! mcr_Array_push ( arrPt, & nc ) )
	{
		dmsg ;
		return 0 ;
	}
	arrPt->array [ arrPt->used - 2 ] = c ;
	return 1 ;
}

char mcr_String_pop ( mcr_Array * arrPt )
{
	dassert ( arrPt ) ;
	dassert ( arrPt->element_size == sizeof ( char ) ) ;
	if ( MCR_STR_ISEMPTY ( arrPt ) )
		return '\0' ;
	char mem = arrPt->array [ arrPt->used - 2 ] ;
	-- arrPt->used ;
	arrPt->array [ arrPt->used - 1 ] = '\0' ;
	return mem ;
}

int mcr_String_from_string ( mcr_Array * arrPt,
		const char * str )
{
	dassert ( arrPt ) ;
	dassert ( arrPt->element_size == sizeof ( char ) ) ;
	size_t min = strlen ( str ) ;
	if ( ! mcr_Array_resize ( arrPt, min + 1 ) )
	{
		dmsg ;
		return 0 ;
	}
	memcpy ( arrPt->array, str, min ) ;
	arrPt->array [ arrPt->size - 1 ] = '\0' ;
	arrPt->used = arrPt->size ;
	return 1 ;
}

int mcr_String_nfrom_string ( mcr_Array * arrPt,
		const char * str, size_t len )
{
	dassert ( arrPt ) ;
	dassert ( arrPt->element_size == sizeof ( char ) ) ;
	size_t min = strlen ( str ) ;
	if ( len < min ) min = len ;
	if ( ! mcr_Array_resize ( arrPt, min + 1 ) )
	{
		dmsg ;
		return 0 ;
	}
	memcpy ( arrPt->array, str, min ) ;
	arrPt->array [ arrPt->size - 1 ] = '\0' ;
	arrPt->used = arrPt->size ;
	return 1 ;
}

int mcr_String_append ( mcr_Array * arrPt,
		const char * str, size_t len )
{
	dassert ( arrPt ) ;
	dassert ( arrPt->element_size == sizeof ( char ) ) ;
	size_t min = strlen ( str ) ;
	if ( len < min ) min = len ;
	if ( ! mcr_Array_resize ( arrPt, min + 1 ) )
	{
		dmsg ;
		return 0 ;
	}
	memcpy ( arrPt->array + arrPt->used - 1, str, min ) ;
	arrPt->array [ arrPt->size - 1 ] = '\0' ;
	arrPt->used = arrPt->size ;
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
