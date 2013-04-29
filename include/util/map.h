/*! \file util/map.h
 * \brief Map one type to another.
 *
 * Array items are stored as the size of paired key-value size.
 * \ref qsort ( void *, size_t, size_t, int ( * ) ( const void *, const void * )
 * \ref bsearch ( const void *, const void *, size_t, size_t,
 * int ( * ) ( const void *, const void * ) )
 * */

# ifndef MCR_MAP_H
# define MCR_MAP_H

# include "util/array.h"

/*!
 * \brief Dynamic resizing binary map.
 *
 * set - mcr_Array Resizing set of all items. <br>
 * sizeof_first - size_t Size of mapping key elements. <br>
 * sizeof_second - size_t Size of mapped value elements. <br>
 * compare - int ( * ) ( const void *, const void * ) Comparison function
 * for binary search and sorting, based on key elements.
 * If compare is NULL, sort and search cannot be performed.
 * */
typedef struct mcr_Map
{
	mcr_Array set ;
	size_t sizeof_first ;
	size_t sizeof_second ;
	int ( * compare ) ( const void * lh_first, const void * rh_first ) ;
} mcr_Map ;

/*! \brief ctor, Size of key and size of value are both required.
 *
 * Please also assign a comparison function, if you wish to use binary
 * search and sort.
 * \param mapPt mcr_Map *
 * \param sizeof_first size_t Size of mapping key elements.
 * \param sizeof_second size_t Size of mapped value elements.
 * */
MCR_API void mcr_Map_init ( mcr_Map * mapPt, size_t sizeofFirst,
		size_t sizeofSecond ) ;
/*! \brief dtor, Element sizes are retained.
 *
 * \param mapPt mcr_Map *
 * */
MCR_API void mcr_Map_free ( mcr_Map * mapPt ) ;
/*! \brief Print values from a map.
 *
 * \param mapPt mcr_Map *
 * */
MCR_API void mcr_Map_print ( mcr_Map * mapPt ) ;
/*! \brief Map from key to value.
 *
 * \param mapPt mcr_Map *
 * \param keyPt const void * Pointer to key to copy from.
 * \param valuePt const void * Pointer to value to copy from.
 * \return 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Map_map ( mcr_Map * mapPt, const void * keyPt,
		const void * valuePt ) ;
/*! \brief Map key+value pair.
 *
 * \param mapPt mcr_Map *
 * \param mappingPair const void * Pointer to key + value pair to
 * copy from.
 * \return 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Map_map_pair ( mcr_Map * mapPt, const void * mappingPair ) ;
/*! \brief \return Get pointer to a key-value pair, mapped from key.
 * NULL if not found.
 *
 * \param mapPt const mcr_Map *
 * \param key const void * Pointer to key to find from.
 * */
MCR_API void * mcr_Map_get ( const mcr_Map * mapPt, const void * keyPt ) ;
/*!
 * \brief \return Get pointer to a key-value pair. Instead of binary
 * search this will be a memory comparison. NULL if not found.
 *
 * \param mapPt const mcr_Map *
 * \param keyPt const void * Key to compare for search.
 * */
MCR_API void * mcr_Map_get_slow ( const mcr_Map * mapPt,
		const void * keyPt ) ;
/*!
 * \brief \return Get pointer to a key-value pair. This is never NULL.
 *
 * If the key is not previously found, both the key and value backup
 * will be mapped before this function returns.
 * \param mapPt mcr_Map *
 * \param keyPt const void * Key to compare for search.
 * \param valueBackupPt const void * Value to copy, if key is not
 * previously mapped.
 * */
MCR_API void * mcr_Map_get_ensured ( mcr_Map * mapPt, const void * keyPt,
		const void * valueBackupPt ) ;
/*! \brief \return Get index of item in array. -1 if out of bounds.
 *
 * \param mapPt mcr_Map *
 * \param key const void * Pointer to key to find from.
 * */
MCR_API size_t mcr_Map_index_of ( const mcr_Map * mapPt,
		const void * keyPt ) ;
/*! \brief For every member of the map, call the iteration
 * function with that member, and the va_list from variadic args.
 *
 * \param mapPt mcr_Map *
 * \param iterateFnc mcr_iterate_fnc
 * */
MCR_API void mcr_Map_for_each ( const mcr_Map * mapPt,
		mcr_iterate_fnc iterateFnc, ... ) ;
/*! \brief For every member of the map, call the iteration
 * function with the value of that member,
 * and the va_list from variadic args.
 *
 * \param mapPt mcr_Map *
 * \param iterateFnc mcr_iterate_fnc
 * */
MCR_API void mcr_Map_for_each_value ( const mcr_Map * mapPt,
		mcr_iterate_fnc iterateFnc,... ) ;
/*! \brief Remove mapping for given key.
 *
 * \param mapPt mcr_Map *
 * \param key const void * Pointer to key to find from.
 * */
MCR_API void mcr_Map_unmap ( mcr_Map * mapPt, const void * keyPt ) ;
/*! \brief \ref qsort.
 *
 * \param mapPt mcr_Map *
 * */
MCR_API void mcr_Map_sort ( mcr_Map * mapPt ) ;
/*! \brief Remove all mapped key-value pairs.
 *
 * \param mapPt mcr_Map *
 * */
MCR_API void mcr_Map_clear ( mcr_Map * mapPt ) ;
/*! \brief Minimize allocated space.
 *
 * \param mapPt mcr_Map *
 * */
MCR_API void mcr_Map_trim ( mcr_Map * mapPt ) ;

/*!
 * \brief \return Pointer to found key, or NULL.
 *
 * \param mapPt mcr_Map *
 * \param keyPt const void * key to find from.
 * */
# define MCR_MAP_GET( mapPt, keyPt ) \
	( mapPt )->set.used ? bsearch ( keyPt, ( mapPt )->set.array, \
			( mapPt )->set.used, ( mapPt )->set.element_size, \
			( mapPt )->compare ) : NULL

/*!
 * \brief \return Change address from key to value.
 *
 * \param mapPt mcr_Map *
 * \param pairPt Address to change from key to value.
 * */
# define MCR_MAP_VALUE( mapPt, pairPt ) \
	( void * ) ( pairPt ? ( unsigned char * ) ( pairPt ) \
			+ ( mapPt )->sizeof_first : NULL )

/*!
 * \brief If compare is available, qsort given map.
 *
 * \param mapPt mcr_Map *
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
 * \param mapPt mcr_Map *
 * \param iterateFnc void ( * ) ( void *,... ) This function must be able
 * to accept both the key-value pair member reference, and all variadic
 * parameters given to this macro.
 * */
# define MCR_MAP_FOR_EACH( mapPt, iterateFnc, ... ) \
	MCR_ARR_FOR_EACH ( & ( mapPt )->set, iterateFnc, __VA_ARGS__ )

/*!
 * \brief Use the iterateFnc for every value of the map set,
 * with all variadic arguments.
 *
 * In this case value means the second element of all key-value pairs.
 * This will not create va_list or do any type checking on variable
 * arguments.
 * \param mapPt mcr_Map *
 * \param iterateFnc void ( * ) ( void *,... ) This function must be able
 * to accept both the key-value pair member reference, and all variadic
 * parameters given to this macro.
 * */
# define MCR_MAP_FOR_EACH_VALUE( mapPt, iterateFnc, ... ) \
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
