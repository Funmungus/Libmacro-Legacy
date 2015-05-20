/* include/util/map.h - Use resizing arrays to map one type to another.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file util/map.h
 * \brief Map one type to another.
 *
 * Array items are stored as the size of paired key-value size.
 * \ref qsort
 * \ref bsearch
 * */

# ifndef MCR_MAP_H
# define MCR_MAP_H

# include "util/array.h"

/*!
 * \brief Dynamic resizing binary map.
 * */
typedef struct mcr_Map
{
	//! \brief Resizing set of all items.
	mcr_Array set ;
	//
	// Internal
	//
	/*! \brief Comparison function
	 * for binary search and sorting, based on key elements.
	 * If compare is NULL, sort and search cannot be performed.
	 **/
	int ( * compare ) ( const void * lh_first, const void * rh_first ) ;
	//! \brief Size of mapping key elements.
	size_t sizeof_first ;
	//! \brief Size of mapped value elements.
	size_t sizeof_second ;
} mcr_Map ;

/*! \brief ctor, Size of key and size of value are both required.
 *
 * Please also assign a comparison function, if you wish to use binary
 * search and sort.
 * \param sizeof_first Size of mapping key elements.
 * \param sizeof_second Size of mapped value elements.
 * */
MCR_API void mcr_Map_init ( mcr_Map * mapPt, size_t sizeofFirst,
		size_t sizeofSecond ) ;
/*! \brief dtor, Element sizes are retained.
 * */
MCR_API void mcr_Map_free ( mcr_Map * mapPt ) ;
/*! \brief Map from key to value.
 *
 * \param keyPt Pointer to key to copy from.
 * \param valuePt Pointer to value to copy from.
 * \return 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Map_map ( mcr_Map * mapPt, const void * keyPt,
		const void * valuePt ) ;
/*! \brief Map remove mapped value from one mapping, and remap
 * it for a different key.
 *
 * \param previousKeyPt Pointer to key to remove from.
 * \param newKeyPt Pointer to key to remap into.
 * \return 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Map_remap ( mcr_Map * mapPt, const void * previousKeyPt,
		const void * newKeyPt ) ;
/*! \brief Map key+value pair.
 *
 * \param mappingPair Pointer to key + value pair to
 * copy from.
 * \return 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Map_map_pair ( mcr_Map * mapPt, const void * mappingPair ) ;
/*! \brief Get a key-value pair, mapped from key pointer.
 *
 * \param keyPt Pointer to key to find from.
 * \return Pointer to key-value pair. NULL if not found.
 * */
MCR_API void * mcr_Map_get ( const mcr_Map * mapPt, const void * keyPt ) ;
/*! \brief Get a pointer to the value mapped from key pointer.
 *
 * \param keyPt Pointer to key to find from.
 * \return Pointer to a value, or NULL if not found.
 * */
MCR_API void * mcr_Map_get_value ( const mcr_Map * mapPt,
		const void * keyPt ) ;
/*!
 * \brief Get a key-value pair. Instead of binary
 * search this will be a memory comparison.
 *
 * \param keyPt Key to compare for search.
 * \return Pointer to key-value pair. NULL if not found.
 * */
MCR_API void * mcr_Map_get_slow ( const mcr_Map * mapPt,
		const void * keyPt ) ;
/*!
 * \brief Get a key-value pair. A backup value will be mapped if
 * key is not yet mapped.
 *
 * If the key is not previously found, both the key and value backup
 * will be mapped before this function returns.
 * \param keyPt Key to compare for search.
 * \param valueBackupPt Value to copy, if key is not
 * previously mapped.
 * \param Pointer to key-value pair. This is only NULL for errors.
 * */
MCR_API void * mcr_Map_get_ensured ( mcr_Map * mapPt, const void * keyPt,
		const void * valueBackupPt ) ;
/*! \brief Get the index of an item in the array. -1 if out of bounds.
 *
 * \param key Pointer to key to find from.
 * \return Index of an item.
 * */
MCR_API size_t mcr_Map_index_of ( const mcr_Map * mapPt,
		const void * keyPt ) ;
/*! \brief For every member of the map, call the iteration
 * function with that member, and the va_list from variadic args.
 *
 * \param iterateFnc Function to call for every pointer to element
 * in the array, also with all variadic parameters.
 * */
MCR_API void mcr_Map_for_each ( const mcr_Map * mapPt,
		mcr_iterate_fnc iterateFnc,... ) ;
/*! \brief For every member of the map, call the iteration
 * function with the value of that member,
 * and the va_list from variadic args.
 *
 * \param iterateFnc Function to call for every pointer to value
 * in all key-value pairs, also with all variadic parameters.
 * */
MCR_API void mcr_Map_for_each_value ( const mcr_Map * mapPt,
		mcr_iterate_fnc iterateFnc,... ) ;
/*! \brief Remove mapping for given key.
 *
 * \param key Pointer to key to find from.
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

/*!
 * \brief Get a pointer to found key, or NULL.
 *
 * \param mapPt \ref mcr_Map *
 * \param keyPt const void * key to find from.
 * \return void *
 * */
# define MCR_MAP_GET( mapPt, keyPt ) \
	( mapPt )->set.used ? bsearch ( keyPt, ( mapPt )->set.array, \
			( mapPt )->set.used, ( mapPt )->set.element_size, \
			( mapPt )->compare ) : NULL
/*!
 * \brief \return Change address from key to value.
 *
 * \param mapPt \ref mcr_Map *
 * \param pairPt Address to change from key to value.
 * */
# define MCR_MAP_VALUE( mapPt, pairPt ) \
	( void * ) ( pairPt ? ( unsigned char * ) ( pairPt ) \
			+ ( mapPt )->sizeof_first : NULL )

/*!
 * \brief Get a pointer to found value, or NULL.
 *
 * \param mapPt \ref mcr_Map *
 * \param keyPt const void * key to find from.
 * \return void *
 * */
# define MCR_MAP_GET_VALUE( mapPt, keyPt ) \
	MCR_MAP_VALUE ( mapPt, ( MCR_MAP_GET ( mapPt, keyPt ) ) )

/*!
 * \brief If compare is available, qsort given map.
 *
 * \param mapPt \ref mcr_Map *
 * */
# define MCR_MAP_SORT( mapPt ) \
	if ( ( mapPt )->compare ) \
	{ \
		qsort ( ( mapPt )->set.array, ( mapPt )->set.used, \
				( mapPt )->set.element_size, ( mapPt )->compare ) ; \
	}

/*!
 * \brief Use the iterateFnc for every element of the map set,
 * with all variadic arguments.
 *
 * This will not create va_list or do any type checking on variable
 * arguments.
 * \param mapPt \ref mcr_Map *
 * \param iterateFnc void ( * ) ( void *,... ) This function must be able
 * to accept both the key-value pair member reference, and all variadic
 * parameters given to this macro.
 * */
# define MCR_MAP_FOR_EACH( mapPt, iterateFnc,... ) \
	MCR_ARR_FOR_EACH ( & ( mapPt )->set, iterateFnc, __VA_ARGS__ )

/*!
 * \brief Use the iterateFnc for every value of the map set,
 * with all variadic arguments.
 *
 * In this case value means the second element of all key-value pairs.
 * This will not create va_list or do any type checking on variable
 * arguments.
 * \param mapPt \ref mcr_Map *
 * \param iterateFnc void ( * ) ( void *,... ) This function must be able
 * to accept both the key-value pair member reference, and all variadic
 * parameters given to this macro.
 * */
# define MCR_MAP_FOR_EACH_VALUE( mapPt, iterateFnc,... ) \
{ \
	void * _itPt_ = MCR_ARR_AT ( & ( mapPt )->set, 0 ) ; \
	_itPt_ = MCR_MAP_VALUE ( mapPt, _itPt_ ) ; \
	void * _endPt_ = MCR_ARR_END ( & ( mapPt )->set ) ; \
	while ( _itPt_ < _endPt_ ) \
	{ \
		iterateFnc ( _itPt_, __VA_ARGS__ ) ; \
		_itPt_ = MCR_ARR_NEXT ( & ( mapPt )->set, _itPt_ ) ; \
	} \
}

# endif // MCR_MAP_H
