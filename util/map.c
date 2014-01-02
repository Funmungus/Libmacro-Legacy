
# include "util/map.h"

void mcr_Map_init ( mcr_Map * mapPt, size_t sizeofFirst,
		size_t sizeofSecond )
{
	if ( ! mapPt ) return ;
	mapPt->sizeof_first = sizeofFirst ;
	mapPt->sizeof_second = sizeofSecond ;
	mcr_Array_init ( & mapPt->set, sizeofFirst + sizeofSecond ) ;
}
void mcr_Map_free ( mcr_Map * mapPt )
{
	if ( ! mapPt ) return ;
	mcr_Array_free ( & mapPt->set ) ;
}

void * mcr_Map_get_slow ( const mcr_Map * mapPt, const void * keyPt )
{
	if ( ! mapPt || ! mapPt->set.used )
		return NULL ;
	void * end = MCR_ARR_END ( & mapPt->set ) ;
	for ( void * i = MCR_ARR_AT ( & mapPt->set, 0 ) ;
			i < end ; i = MCR_ARR_NEXT ( & mapPt->set, i ) )
	{
		if ( ! memcmp ( i, keyPt, mapPt->sizeof_first ) )
			return i ;
	}
	return NULL ;
}

int mcr_Map_map ( mcr_Map * mapPt, const void * keyPt,
		const void * valuePt )
{
	if ( ! mapPt || ! keyPt ) return 0 ;
	unsigned char * mapping = mcr_Map_get ( mapPt, keyPt ) ;
	// Found. Copy and sort.
	if ( mapping )
	{
		// key equal, skip to writing value
		mapping += mapPt->sizeof_first ;
		if ( valuePt )
			memcpy ( mapping, valuePt, mapPt->sizeof_second ) ;
		else
			memset ( mapping, 0, mapPt->sizeof_second ) ;
		return 1 ;
	}
	if ( ! mcr_Array_push ( & mapPt->set, NULL ) )
	{
		DMSG ( "%s\n", "map." ) ;
		return 0 ;
	}
	// Point to pushed member.
	mapping = MCR_ARR_AT ( & mapPt->set, mapPt->set.used - 1 ) ;

	memcpy ( mapping, keyPt, mapPt->sizeof_first ) ;
	if ( valuePt )
	{
		mapping += mapPt->sizeof_first ;
		memcpy ( mapping, valuePt, mapPt->sizeof_second ) ;
	}

	MCR_MAP_SORT ( mapPt ) ;

	return 1 ;
}
int mcr_Map_map_pair ( mcr_Map * mapPt, const void * mappingPair )
{
	if ( ! mapPt || ! mappingPair ) return 0 ;
	void * mapping = mcr_Map_get ( mapPt, mappingPair ) ;
	// Found. Copy and sort.
	if ( mapping )
	{
		memcpy ( mapping, mappingPair, mapPt->set.element_size ) ;
		MCR_MAP_SORT ( mapPt ) ;
		return 1 ;
	}
	if ( ! mcr_Array_push ( & mapPt->set, mappingPair ) )
	{
		DMSG ( "%s\n", "map_pair." ) ;
		return 0 ;
	}
	MCR_MAP_SORT ( mapPt ) ;
	return 1 ;
}
void * mcr_Map_get ( const mcr_Map * mapPt, const void * keyPt )
{
	if ( ! mapPt || ! keyPt )
		return NULL ;
	return mapPt->compare ? MCR_MAP_GET ( mapPt, keyPt ) :
			mcr_Map_get_slow ( mapPt, keyPt ) ;
}
void * mcr_Map_get_ensured ( mcr_Map * mapPt, const void * keyPt,
		const void * valueBackupPt )
{
	if ( ! mapPt || ! keyPt )
		return NULL ;
	void * found = mcr_Map_get ( mapPt, keyPt ) ;
	if ( found )
	{
		return found ;
	}
	else
	{
		if ( ! mcr_Map_map ( mapPt, keyPt, valueBackupPt ) )
		{
			DMSG ( "%s\n", "get_ensured allocation error." ) ;
			return NULL ;
		}
		return MCR_MAP_GET ( mapPt, keyPt ) ;
	}
}
size_t mcr_Map_index_of ( const mcr_Map * mapPt, const void * keyPt )
{
	if ( ! mapPt || ! mapPt->set.element_size )
		return -1 ;
	unsigned char * mapping = mcr_Map_get ( mapPt, keyPt ) ;
	if ( ! mapping )
		return -1 ;
	size_t diff = mapping - mapPt->set.array ;
	return diff / mapPt->set.element_size ;
}
void mcr_Map_for_each ( const mcr_Map * mapPt,
		mcr_iterate_fnc iterateFnc,... )
{
	if ( ! mapPt || ! mapPt->set.used ) return ;
	va_list lst ;
	void * it = MCR_ARR_AT ( & mapPt->set, 0 ) ;
	if ( ! it ) return ;
	void * end = MCR_ARR_END ( & mapPt->set ) ;
	va_start ( lst, iterateFnc ) ;
	while ( it < end )
	{
		va_list cpy ;
		va_copy ( cpy, lst ) ;
		iterateFnc ( it, cpy ) ;
		va_end ( cpy ) ;
		it = MCR_ARR_NEXT ( & mapPt->set, it ) ;
	}
	va_end ( lst ) ;
}
void mcr_Map_for_each_value ( const mcr_Map * mapPt,
		mcr_iterate_fnc iterateFnc,... )
{
	if ( ! mapPt || ! mapPt->set.used ) return ;
	va_list lst ;
	void * it = MCR_ARR_AT ( & mapPt->set, 0 ) ;
	if ( ! it ) return ;
	it = MCR_MAP_VALUE ( mapPt, it ) ;
	void * end = MCR_ARR_END ( & mapPt->set ) ;
	va_start ( lst, iterateFnc ) ;
	while ( it < end )
	{
		va_list cpy ;
		va_copy ( cpy, lst ) ;
		iterateFnc ( it, cpy ) ;
		va_end ( cpy ) ;
		it = MCR_ARR_NEXT ( & mapPt->set, it ) ;
	}
	va_end ( lst ) ;
}
void mcr_Map_unmap ( mcr_Map * mapPt, const void * keyPt )
{
	if ( ! mapPt || ! keyPt )
		return ;
	size_t index = mcr_Map_index_of ( mapPt, keyPt ) ;
	if ( index != ( size_t ) -1 )
	{
		mcr_Array_remove ( & mapPt->set, index ) ;
		MCR_MAP_SORT ( mapPt ) ;
	}
}
void mcr_Map_sort ( mcr_Map * mapPt )
{
	if ( ! mapPt || ! mapPt->compare )
		return ;
	MCR_MAP_SORT ( mapPt ) ;
}
void mcr_Map_clear ( mcr_Map * mapPt )
{
	mapPt->set.used = 0 ;
}
void mcr_Map_trim ( mcr_Map * mapPt )
{
	mcr_Array_trim ( & mapPt->set ) ;
}

void mcr_Map_print ( mcr_Map * mapPt )
{
	if ( ! mapPt ) return ;
	printf ( "Map %p: first %llu, second %llu, compare %p.\n\tMember... ",
			mapPt, ( long long unsigned ) mapPt->sizeof_first,
			( long long unsigned ) mapPt->sizeof_second, mapPt->compare ) ;
	mcr_Array_print ( & mapPt->set ) ;
}
