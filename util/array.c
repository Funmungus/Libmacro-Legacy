
# include "util/array.h"

void mcr_Array_init ( mcr_Array * arrPt, size_t elementSize )
{
	if ( ! arrPt ) return ;
	memset ( arrPt, 0, sizeof ( mcr_Array ) ) ;
	arrPt->element_size = elementSize ;
}

void mcr_Array_free ( mcr_Array * arrPt )
{
	if ( ! arrPt ) return ;
	if ( arrPt->array ) free ( arrPt->array ) ;
	mcr_Array_init ( arrPt, arrPt->element_size ) ;
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
			dmsg ( "%s", "push." ) ;
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
			dmsg ( "%s", "insert." ) ;
			return 0 ;
		}
	}
	// Insert in-between, but need more room.
	else if ( arrPt->used == arrPt->size )
	{
		if ( ! mcr_Array_resize ( arrPt, arrPt->size ?
				arrPt->size << 1 : 2 ) )
		{
			dmsg ( "%s", "insert." ) ;
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
			dmsg ( "%s", "Trouble finding memmove positions." )
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
		dmsg ( "%s", "insert_filled." ) ;
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
		dmsg ( "%s\n", "from_array." ) ;
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
			dmsg ( "%s\n", "Append." ) ;
			return 0 ;
		}
	}
	size_t index = arrPt->used ;
	arrPt->used += count ;
	void * it = MCR_ARR_AT ( arrPt, index ) ;
	if ( it )
		memcpy ( it, arraySource, count *
				arrPt->element_size ) ;
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

void * mcr_Array_at ( mcr_Array * arrPt, size_t pos )
{
	dassert ( arrPt ) ;
	return MCR_ARR_AT ( arrPt, pos ) ;
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
		mcr_iterate_fnc iterateFnc,... )
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

	unsigned char * newSet = ( unsigned char * ) realloc
			( arrPt->array, newSize * arrPt->element_size ) ;
	// If freeing all memory, only need to assure 0 count.
	// If freeing all, then realloc will have freed,
	// we just set to null set {0}
	if ( ! newSize || ! arrPt->element_size )
	{
		mcr_Array_init ( arrPt, arrPt->element_size ) ;
		return 1 ;
	}

	// Check for errors. If errors, do not change structure.
	if ( ! newSet )
	{
		dmsg ( "%s", "resize allocation error." ) ;
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
	printf ( "Array %p: used %llu, size %llu, element size %llu, "
			"array: 0x...\n", arrPt, ( long long unsigned ) arrPt->used,
			( long long unsigned ) arrPt->size,
			( long long unsigned ) arrPt->element_size ) ;
	const void * end = MCR_ARR_END ( arrPt ) ;
	for ( const unsigned char * i = MCR_ARR_AT ( arrPt, 0 ) ;
			( void * ) i < end ; i++ )
	{
		printf ( "%x%s", * i, i + 1 == end ? "" : " " ) ;
	}
	printf ( "\n" ) ;
}
