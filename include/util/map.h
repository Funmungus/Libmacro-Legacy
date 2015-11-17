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

/*! \file util/map.h
 * \brief Map one type to another.
 *
 * Array items are stored as the size of paired key-value size.
 * \ref qsort
 * \ref bsearch
 * */

# ifndef MCR_MAP_H
# define MCR_MAP_H

# include "util/mcrstrings.h"

/*!
 * \brief Dynamic resizing binary map
 * */
typedef struct mcr_Map
{
	//! \brief Resizing set of all items
	mcr_Array set ;
	//
	// Internal
	//
	/*! \brief Comparison function for binary search and sorting,
	 * based on key elements.
	 *
	 * If compare is NULL, sort and search cannot be performed.
	 * */
	mcr_compare_fnc compare ;
	//! \brief Size of mapping key elements
	size_t sizeof_first ;
	//! \brief Size of mapped value elements
	size_t sizeof_second ;
} mcr_Map ;

/*! \brief ctor: The size of key and value are both required.
 *
 * Please also assign a comparison function, if you wish to use binary
 * search and sort.
 * \param sizeof_first Size of mapping key elements
 * \param sizeof_second Size of mapped value elements
 * */
MCR_API void mcr_Map_init ( mcr_Map * mapPt, size_t sizeofFirst,
		size_t sizeofSecond ) ;
/*! \brief Initialize a map to map from a string.
 *
 * \param sizeofSecond size_t
 * */
# define mcr_StringMap_init( mapPt, sizeofSecond ) \
mcr_Map_init ( mapPt, sizeof ( mcr_Array ), sizeofSecond ) ; \
( mapPt )->compare = mcr_name_compare ;
/*! \brief dtor: Element sizes are retained.
 * */
MCR_API void mcr_Map_free ( mcr_Map * mapPt ) ;
//! \brief Variadic \ref mcr_Map_free, unused extra arguments
# define mcr_Map_free_foreach( mapPt, ... ) mcr_Map_free ( arrPt )
/*! \brief Map from key to value.
 *
 * \param keyPt Pointer to key to copy from
 * \param valuePt Pointer to value to copy from
 * \return 0 on failure, otherwise successful
 * */
MCR_API int mcr_Map_map ( mcr_Map * mapPt, const void * keyPt,
		const void * valuePt ) ;
/*! \brief Remove mapped value from one mapping, and remap
 * it for a different key.
 *
 * \param previousKeyPt Pointer to key to remove from
 * \param newKeyPt Pointer to key to remap into
 * \return 0 on failure, otherwise successful
 * */
MCR_API int mcr_Map_remap ( mcr_Map * mapPt, const void * previousKeyPt,
		const void * newKeyPt ) ;
/*! \brief Map key+value pair.
 *
 * \param mappingPair Pointer to key + value pair to
 * copy from
 * \return 0 on failure, otherwise successful
 * */
MCR_API int mcr_Map_map_pair ( mcr_Map * mapPt, const void * mappingPair ) ;
/*! \brief Get a key-value pair, mapped from key pointer.
 *
 * \param keyPt Pointer to key to find from
 * \return Pointer to a key-value pair. or NULL if not found
 * */
MCR_API void * mcr_Map_get ( const mcr_Map * mapPt, const void * keyPt ) ;
/*! \brief Get a pointer to the value mapped from key pointer.
 *
 * \param keyPt Pointer to key to find from
 * \return Pointer to a value, or NULL if not found
 * */
MCR_API void * mcr_Map_get_value ( const mcr_Map * mapPt,
		const void * keyPt ) ;
/*! \brief Get a key-value pair. Instead of binary
 * search this will be a memory comparison.
 *
 * \param keyPt Key to compare for search
 * \return Pointer to a key-value pair. or NULL if not found
 * */
MCR_API void * mcr_Map_get_slow ( const mcr_Map * mapPt,
		const void * keyPt ) ;
/*! \brief Get a key-value pair. A backup value will be mapped if
 * key is not yet mapped.
 *
 * If the key is not previously found, both the key and value backup
 * will be mapped before this function returns.
 * \param keyPt Key to compare for search
 * \param valueBackupPt Value to copy, if key is not
 * previously mapped.
 * \param Pointer to key-value pair, this is only NULL for errors.
 * */
MCR_API void * mcr_Map_get_ensured ( mcr_Map * mapPt, const void * keyPt,
		const void * valueBackupPt ) ;
/*! \brief Get the index of an item in the array. -1 if out of bounds
 *
 * \param key Pointer to key to find from
 * \return Index of an item
 * */
MCR_API size_t mcr_Map_index_of ( const mcr_Map * mapPt,
		const void * keyPt ) ;
/*! \brief For every member of the map, call the iteration
 * function with that member, and the va_list from variadic args.
 *
 * \param iterateFnc Function to call for every pointer to element
 * in the array, also with all variadic parameters
 * */
MCR_API void mcr_Map_for_each ( const mcr_Map * mapPt,
		mcr_iterate_fnc iterateFnc, ... ) ;
/*! \brief For every member of the map, call the iteration
 * function with the value of that member,
 * and the va_list from variadic args.
 *
 * \param iterateFnc Function to call for every pointer to value
 * in all key-value pairs, also with all variadic parameters.
 * */
MCR_API void mcr_Map_for_each_value ( const mcr_Map * mapPt,
		mcr_iterate_fnc iterateFnc, ... ) ;
/*! \brief Remove mapping for given key.
 *
 * \param key Pointer to key to find from
 * */
MCR_API void mcr_Map_unmap ( mcr_Map * mapPt, const void * keyPt ) ;
/*! \brief Sort with \ref qsort.
 *
 * The qsort function is the map compare function. qsort element
 * size is the size of both key and value.
 * */
MCR_API void mcr_Map_sort ( mcr_Map * mapPt ) ;
/*! \brief Remove all mapped key-value pairs.
 * */
MCR_API void mcr_Map_clear ( mcr_Map * mapPt ) ;
/*! \brief Minimize allocated space.
 * */
MCR_API void mcr_Map_trim ( mcr_Map * mapPt ) ;
/*! \brief Print values from a map.
 * */
MCR_API void mcr_Map_print ( mcr_Map * mapPt ) ;

/*! \brief Map a string to given value.
 *
 * \param key String to map from
 * \param valuePt Value to map to
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_StringMap_map ( mcr_Map * mapPt, const char * key,
		const void * valuePt ) ;
/*! \brief Map a string to a set of values.
 *
 * \param key String to map from
 * \param count Number of values to map to
 * \param valuePt Array of values to map to
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_StringMap_nmap ( mcr_Map * mapPt, const char * key,
		size_t count, const void * valuePt ) ;
/*! \brief Get a reference to a mapped key+value pair.
 *
 * \param key String mapped to pair
 * \return Pointer to mapped to pair
 * */
MCR_API void * mcr_StringMap_get ( const mcr_Map * mapPt, const char * key ) ;
/*! \brief Get a reference to a mapped value.
 *
 * \param key String mapped to value
 * \return Pointer to mapped value
 * */
MCR_API void * mcr_StringMap_get_value ( const mcr_Map * mapPt,
		const char * key ) ;
/*! \brief Remove mapping from one string, and map to another.
 *
 * \param key String currently mapped to value
 * \param newKey String to remap value from
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_StringMap_remap ( mcr_Map * mapPt, const char * key,
		const char * newKey ) ;
/*! \brief Remove mapping from given string.
 *
 * \param key String to remove mapping from
 * */
MCR_API void mcr_StringMap_unmap ( mcr_Map * mapPt, const char * key ) ;

//
// Comparisons
//
/*! \brief Compare const c-strings referenced by each pointer,
 * case insensitive.
 *
 * \param lhs const char ** or mcr_Array **
 * \param rhs const char ** or mcr_Array **
 * \return < 0 if lhs < rhs, 0 if lhs == rhs,
 * > 0 if lhs > rhs
 * */
MCR_API int mcr_name_compare ( const void * lhs, const void * rhs ) ;
/*! \brief Compare const c-strings referenced by each pointer,
 * case sensitive.
 *
 * \param lhs const char ** or mcr_Array **
 * \param rhs const char ** or mcr_Array **
 * \return < 0 if lhs < rhs, 0 if lhs == rhs,
 * > 0 if lhs > rhs
 * */
MCR_API int mcr_str_compare ( const void * lhs, const void * rhs ) ;
/*! \brief Compare integers referenced by each pointer.
 *
 * \param lhs int *
 * \param rhs int *
 * \return < 0 if lhs < rhs, 0 if lhs == rhs,
 * > 0 if lhs > rhs
 * */
MCR_API int mcr_int_compare ( const void * lhs, const void * rhs ) ;
/*! \brief Compare unsigned integers referenced by each pointer.
 *
 * \param lhs int *
 * \param rhs int *
 * \return < 0 if lhs < rhs, 0 if lhs == rhs,
 * > 0 if lhs > rhs
 * */
MCR_API int mcr_unsigned_compare ( const void * lhs, const void * rhs ) ;
/*! \brief Compare size_t referenced by each pointer.
 *
 * \param lhs size_t *
 * \param rhs size_t *
 * \return < 0 if lhs < rhs, 0 if lhs == rhs,
 * > 0 if lhs > rhs
 * */
MCR_API int mcr_size_t_compare ( const void * lhs, const void * rhs ) ;
/*! \brief Compare void * referenced by each pointer.
 *
 * \param lhs void **
 * \param rhs void **
 * \return < 0 if lhs < rhs, 0 if lhs == rhs,
 * > 0 if lhs > rhs
 * */
MCR_API int mcr_ref_compare ( const void * lhs, const void * rhs ) ;

/*! \brief \ref mcr_Map_get
 *
 * \param map \ref mcr_Map
 * \param keyPt const void *
 * \return void *
 * */
# define MCR_MAP_GET( map, keyPt ) \
MCR_ARR_FIND ( ( map ).set, keyPt, ( map ).compare )
/*! \brief \ref mcr_StringMap_get
 *
 * \param map \ref mcr_Map
 * \param key const char * or const mcr_Array
 * \return void *
 * */
# define MCR_STRINGMAP_GET( map, key ) \
MCR_MAP_GET ( map, & key )
/*! \brief Change address from key to value.
 *
 * \param map \ref mcr_Map
 * \param pairPt Address to change from key to value
 * \return void *
 * */
# define MCR_MAP_VALUE( map, pairPt ) \
( ( void * ) ( pairPt ? ( char * ) ( pairPt ) \
		+ ( map ).sizeof_first : NULL ) )

/*! \brief \ref mcr_Map_get_value
 *
 * \param map \ref mcr_Map
 * \param keyPt const void *
 * \return void *
 * */
# define MCR_MAP_GET_VALUE( map, keyPt ) \
MCR_MAP_VALUE ( map, ( MCR_MAP_GET ( map, keyPt ) ) )
/*! \brief \ref mcr_StringMap_get_value
 *
 * \param map \ref mcr_Map
 * \param key const char * or mcr_Array
 * */
# define MCR_STRINGMAP_GET_VALUE( map, key ) \
MCR_MAP_GET_VALUE ( map, & key )
/*! \brief If compare is available, qsort given map.
 *
 * \param map \ref mcr_Map
 * */
# define MCR_MAP_SORT( map ) \
if ( ( map ).compare ) \
{ \
	MCR_ARR_SORT ( ( map ).set, ( map ).compare ) ; \
}

/*! \brief Use the iterateFnc for every element of the map set,
 * with all variadic arguments.
 *
 * This will not create va_list or do any type checking on variable
 * arguments.
 * \param map \ref mcr_Map
 * \param iterateFnc void ( * ) ( void *, ... ) This function must be able
 * to accept both the key-value pair member reference, and all variadic
 * parameters given to this macro.
 * */
# define MCR_MAP_FOR_EACH( map, iterateFnc, ... ) \
MCR_ARR_FOR_EACH ( ( map ).set, iterateFnc, __VA_ARGS__ )

/*! \brief Use the iterateFnc for every value of the map set,
 * with all variadic arguments.
 *
 * In this case value means the second element of all key-value pairs.
 * This will not create va_list or do any type checking on variable
 * arguments.
 * \param map \ref mcr_Map
 * \param iterateFnc void ( * ) ( void *, ... ) This function must be able
 * to accept both the key-value pair member reference, and all variadic
 * parameters given to this macro.
 * */
# define MCR_MAP_FOR_EACH_VALUE( map, iterateFnc, ... ) \
{ \
	void * _itPt_ = MCR_ARR_AT ( ( map ).set, 0 ) ; \
	_itPt_ = MCR_MAP_VALUE ( map, _itPt_ ) ; \
	void * _endPt_ = MCR_ARR_END ( ( map ).set ) ; \
	while ( _itPt_ < _endPt_ ) \
	{ \
		iterateFnc ( _itPt_, __VA_ARGS__ ) ; \
		_itPt_ = MCR_ARR_NEXT ( ( map ).set, _itPt_ ) ; \
	} \
}

# endif // MCR_MAP_H
