/* util/map.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "util/map.h"

void mcr_Map_init ( mcr_Map * mapPt, size_t sizeofFirst,
		size_t sizeofSecond )
{
	if ( ! mapPt )
	{
		dmsg ;
		return ;
	}
	mapPt->sizeof_first = sizeofFirst ;
	mapPt->sizeof_second = sizeofSecond ;
	mcr_Array_init ( & mapPt->set, sizeofFirst + sizeofSecond ) ;
}

void mcr_Map_free ( mcr_Map * mapPt )
{
	if ( ! mapPt )
	{
		dmsg ;
		return ;
	}
	mcr_Array_free ( & mapPt->set ) ;
}

void * mcr_Map_get_slow ( const mcr_Map * mapPt, const void * keyPt )
{
	dassert ( mapPt ) ;
	dassert ( keyPt ) ;
	if ( ! mapPt->set.used )
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
	dassert ( mapPt ) ;
	dassert ( keyPt ) ;
	char * mapping = mcr_Map_get ( mapPt, keyPt ) ;
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
		dmsg ;
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

int mcr_Map_remap ( mcr_Map * mapPt, const void * previousKeyPt,
		const void * newKeyPt )
{
	dassert ( mapPt ) ;
	if ( ! newKeyPt )
	{
		mcr_Map_unmap ( mapPt, previousKeyPt ) ;
		return 1 ;
	}
	void * oldPlace = previousKeyPt ?
			mcr_Map_get_value ( mapPt, previousKeyPt ) : NULL ;
	if ( ! oldPlace )
		return 1 ;
	void * newPlace = mcr_Map_get_value ( mapPt, newKeyPt ) ;
	// No new place to put, add a mapping if we have a key to use.
	if ( ! newPlace )
	{
		if ( ! mcr_Array_push ( & mapPt->set, NULL ) )
		{
			dmsg ;
			return 0 ;
		}
		newPlace = MCR_ARR_AT ( & mapPt->set, mapPt->set.used - 1 ) ;
		memcpy ( newPlace, newKeyPt, mapPt->sizeof_first ) ;
		newPlace = MCR_MAP_VALUE ( mapPt, newPlace ) ;
	}
	dassert ( newPlace ) ;
	memcpy ( newPlace, oldPlace, mapPt->sizeof_second ) ;
	mcr_Map_unmap ( mapPt, previousKeyPt ) ;
	return 1 ;
}

int mcr_Map_map_pair ( mcr_Map * mapPt, const void * mappingPair )
{
	dassert ( mapPt ) ;
	dassert ( mappingPair ) ;
	if ( mapPt->compare )
		return mcr_Array_add_sorted ( & mapPt->set, mappingPair,
			mapPt->compare ) ;
	else
		return mcr_Array_push_unique ( & mapPt->set, mappingPair ) ;
}

void * mcr_Map_get ( const mcr_Map * mapPt, const void * keyPt )
{
	dassert ( mapPt ) ;
	dassert ( keyPt ) ;
	return mapPt->compare ? MCR_MAP_GET ( mapPt, keyPt ) :
			mcr_Map_get_slow ( mapPt, keyPt ) ;
}

void * mcr_Map_get_value ( const mcr_Map * mapPt,
		const void * keyPt )
{
	dassert ( mapPt ) ;
	dassert ( keyPt ) ;
	return MCR_MAP_VALUE ( mapPt, mcr_Map_get ( mapPt, keyPt ) ) ;
}

void * mcr_Map_get_ensured ( mcr_Map * mapPt, const void * keyPt,
		const void * valueBackupPt )
{
	dassert ( mapPt ) ;
	dassert ( keyPt ) ;
	void * found = MCR_MAP_GET ( mapPt, keyPt ) ;
	if ( found )
	{
		return found ;
	}
	else
	{
		if ( ! mcr_Map_map ( mapPt, keyPt, valueBackupPt ) )
		{
			dmsg ;
			return NULL ;
		}
		return MCR_MAP_GET ( mapPt, keyPt ) ;
	}
}

size_t mcr_Map_index_of ( const mcr_Map * mapPt, const void * keyPt )
{
	dassert ( mapPt ) ;
	if ( ! mapPt->set.element_size )
		return -1 ;
	char * mapping = mcr_Map_get ( mapPt, keyPt ) ;
	if ( ! mapping )
		return -1 ;
	return MCR_ARR_INDEXOF ( & mapPt->set, mapping ) ;
}

void mcr_Map_for_each ( const mcr_Map * mapPt,
		mcr_iterate_fnc iterateFnc, ... )
{
	dassert ( mapPt ) ;
	if ( ! mapPt->set.used )
		return ;
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
		mcr_iterate_fnc iterateFnc, ... )
{
	dassert ( mapPt ) ;
	if ( ! mapPt->set.used ) return ;
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
	dassert ( mapPt ) ;
	dassert ( keyPt ) ;
	size_t index = mcr_Map_index_of ( mapPt, keyPt ) ;
	if ( index != ( size_t ) -1 )
	{
		mcr_Array_remove ( & mapPt->set, index ) ;
		MCR_MAP_SORT ( mapPt ) ;
	}
}

void mcr_Map_sort ( mcr_Map * mapPt )
{
	dassert ( mapPt ) ;
	if ( ! mapPt->compare )
	{
		dmsg ;
		return ;
	}
	MCR_MAP_SORT ( mapPt ) ;
}

void mcr_Map_clear ( mcr_Map * mapPt )
{
	dassert ( mapPt ) ;
	mapPt->set.used = 0 ;
}

void mcr_Map_trim ( mcr_Map * mapPt )
{
	dassert ( mapPt ) ;
	mcr_Array_trim ( & mapPt->set ) ;
}

void mcr_Map_print ( mcr_Map * mapPt )
{
	dassert ( mapPt ) ;
	fprintf ( mcr_stdout, "Map %p: first %llu, second %llu, "
			"compare %p.\n\tMember... ", mapPt,
			( long long unsigned ) mapPt->sizeof_first,
			( long long unsigned ) mapPt->sizeof_second,
			mapPt->compare ) ;
	mcr_Array_print ( & mapPt->set ) ;
}

int mcr_StringMap_map ( mcr_Map * mapPt, const char * key,
	const void * valuePt )
{
	dassert ( mapPt ) ;
	dassert ( key ) ;
	mcr_Array * found = mcr_StringMap_get ( mapPt, key ) ;
	if ( found )
	{
		mcr_String_from_string ( found, key ) ;
		void * foundVal = MCR_MAP_VALUE ( mapPt, found ) ;
		memcpy ( foundVal, valuePt,
				mapPt->sizeof_second ) ;
	}
	else
	{
		mcr_Array name ;
		mcr_String_init ( & name ) ;
		if ( ! mcr_String_from_string ( & name, key ) ||
				! mcr_Map_map ( mapPt, & name, valuePt ) )
		{
			dmsg ;
			mcr_Array_free ( & name ) ;
			return 0 ;
		}
	}
	return 1 ;
}

int mcr_StringMap_nmap ( mcr_Map * mapPt, const char * key,
		size_t count, const void * valuePt )
{
	dassert ( mapPt ) ;
	dassert ( key ) ;
	mcr_Array * found = mcr_StringMap_get ( mapPt, key ) ;
	if ( found )
	{
		mcr_String_nfrom_string ( found, key, count ) ;
		void * foundVal = MCR_MAP_VALUE ( mapPt, found ) ;
		memcpy ( foundVal, valuePt,
				mapPt->sizeof_second ) ;
	}
	else
	{
		mcr_Array name ;
		mcr_String_init ( & name ) ;
		if ( ! mcr_String_nfrom_string ( & name, key, count ) ||
		! mcr_Map_map ( mapPt, & name, valuePt ) )
		{
			dmsg ;
			mcr_Array_free ( & name ) ;
			return 0 ;
		}
	}
	return 1 ;
}

int mcr_StringMap_remap ( mcr_Map * mapPt, const char * key,
	const char * newKey )
{
	dassert ( mapPt ) ;
	dassert ( key ) ;
	mcr_Array * found = mcr_StringMap_get ( mapPt, key ) ;
	void * foundVal = MCR_MAP_VALUE ( mapPt, found ) ;
	if ( found )
	{
		if ( ! mcr_StringMap_map ( mapPt, newKey, foundVal ) )
		{
			dmsg ;
			return 0 ;
		}
		mcr_Array_free ( found ) ;
		mcr_Array_remove ( & mapPt->set, MCR_ARR_INDEXOF ( & mapPt->set,
				found ) ) ;
		MCR_MAP_SORT ( mapPt ) ;
	}
	return 1 ;
}

void * mcr_StringMap_get ( const mcr_Map * mapPt, const char * key )
{
	dassert ( mapPt ) ;
	dassert ( key ) ;
	return mcr_Map_get ( mapPt, & key ) ;
}

void * mcr_StringMap_get_value ( const mcr_Map * mapPt,
	const char * key )
{
	dassert ( mapPt ) ;
	dassert ( key ) ;
	return mcr_Map_get_value ( mapPt, & key ) ;
}

void mcr_StringMap_unmap ( mcr_Map * mapPt, const char * key )
{
	dassert ( mapPt ) ;
	dassert ( key ) ;
	mcr_Array * arrPt = mcr_StringMap_get ( mapPt, key ) ;
	if ( arrPt )
	{
		mcr_Array_free ( arrPt ) ;
		mcr_Array_remove ( & mapPt->set, MCR_ARR_INDEXOF ( & mapPt->set,
				arrPt ) ) ;
		MCR_MAP_SORT ( mapPt ) ;
	}
}

int mcr_name_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
	{
		return lhs < rhs ? -1 : 1 ;
	}
	return strcasecmp ( * ( const char * const * ) lhs,
						 * ( const char * const * ) rhs ) ;
}

int mcr_str_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
	{
		return lhs < rhs ? -1 : 1 ;
	}
	return strcmp ( * ( const char * const * ) lhs,
						 * ( const char * const * ) rhs ) ;
}

int mcr_int_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
	{
		return lhs < rhs ? -1 : 1 ;
	}
	return * ( const int * ) lhs < * ( const int * ) rhs ? -1 :
			 * ( const int * ) lhs > * ( const int * ) rhs ;
}

int mcr_unsigned_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
	{
		return lhs < rhs ? -1 : 1 ;
	}
	return * ( const unsigned int * ) lhs <
			 * ( const unsigned int * ) rhs ? -1 :
			 * ( const unsigned int * ) lhs >
			 * ( const unsigned int * ) rhs ;
}

int mcr_size_t_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
	{
		return lhs < rhs ? -1 : 1 ;
	}
	return * ( const size_t * ) lhs < * ( const size_t * ) rhs ? -1 :
			 * ( const size_t * ) lhs > * ( const size_t * ) rhs ;
}

int mcr_ref_compare ( const void * lhs, const void * rhs )
{
	if ( ! lhs || ! rhs )
	{
		return lhs < rhs ? -1 : 1 ;
	}
	return * ( void * const * ) lhs < * ( void * const * ) rhs ? -1 :
			 * ( void * const * ) lhs > * ( void * const * ) rhs ;
}
