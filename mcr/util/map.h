/* Libmacro - A multi-platform, extendable macro and hotkey C library.
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

/*! \file
 * \brief Comparison and mapping from one type to another
 *
 * Array items are stored as the size of paired key-value size.
 * \ref qsort
 * \ref bsearch
 */

#ifndef MCR_MAP_H
#define MCR_MAP_H

#include "mcr/util/array.h"
#include "mcr/util/interface.h"

/*! \brief Dynamic resizing map */
struct mcr_Map {
	/*!
	 * \brief Resizing set of all items
	 *
	 * Size of one mapped element = \ref mcr_Map.key_size +
	 * \ref mcr_Map.value_size
	 */
	struct mcr_Array set;
	/* Internal */
	/*! \brief Size of mapping key elements */
	size_t key_size;
	/*! \brief Size of mapped value elements */
	size_t value_size;
	/*! \brief (opt) Data interface of key elements */
	const struct mcr_Interface *key_iface;
	/*! \brief (opt) Data interface of value elements */
	const struct mcr_Interface *value_iface;
};

/*!
 * ctor
 * Nothing will be allocated
 */
MCR_API void mcr_Map_init(void *dataPt);
/*! dtor: Element sizes are retained.
 */
MCR_API void mcr_Map_free(void *dataPt);
/*! \ref mcr_Map_free */
#define mcr_Map_free_foreach(mapPt, ignore) \
mcr_Map_free(mapPt)
/*!
 * \brief Set initial members to be useful.
 *
 * \param keySize \ref mcr_Map.key_size
 * \param valueSize \ref mcr_Map.value_size
 * \param compare \ref mcr_Array.compare
 * \param keyIface (opt) \ref mcr_Map.key_iface, overrides keySize and
 * compare
 * \param valueIface (opt) \ref mcr_Map.value_iface, overrides
 * valueSize
 */
MCR_API void mcr_Map_set_all(struct mcr_Map *mapPt, size_t keySize,
	size_t valueSize, mcr_compare_fnc compare,
	const struct mcr_Interface *keyIface,
	const struct mcr_Interface *valueIface);
/* Allocation control */
MCR_API int mcr_Map_minused(struct mcr_Map *mapPt, size_t minimumUsed);
MCR_API int mcr_Map_minsize(struct mcr_Map *mapPt, size_t minimumSize);
MCR_API int mcr_Map_smartsize(struct mcr_Map *mapPt, size_t increasingCount);
/*! \brief Minimize allocated space. */
MCR_API void mcr_Map_trim(struct mcr_Map *mapPt);
MCR_API int mcr_Map_resize(struct mcr_Map *mapPt, size_t newSize);
/*! \brief Remove all mapped key-value pairs. */
MCR_API void mcr_Map_clear(struct mcr_Map *mapPt);
/* Position */
MCR_API void *mcr_Map_valueof(const struct mcr_Map *mapPt, const void *pairPt);
MCR_API int mcr_Map_set_key(const struct mcr_Map *mapPt, void *pairPt,
	const void *copyKeyPt);
MCR_API int mcr_Map_set_valueof(const struct mcr_Map *mapPt, void *pairPt,
	const void *copyValuePt);
/*!
 * \brief Get a key-value pair, mapped from key pointer.
 *
 * \param keyPt Pointer to key to find from
 * \return Pointer to a key-value pair. or NULL if not found
 */
MCR_API void *mcr_Map_element(const struct mcr_Map *mapPt, const void *keyPt);
/*!
 * \brief Get a pointer to the value mapped from key pointer.
 *
 * \param keyPt Pointer to key to find from
 * \return Pointer to a value, or NULL if not found
 */
MCR_API void *mcr_Map_value(const struct mcr_Map *mapPt, const void *keyPt);
/*!
 * \brief Get a key-value pair. A backup value will be mapped if
 * key is not yet mapped.
 *
 * If the key is not previously found, both the key and value backup
 * will be mapped before this function returns.
 * \param keyPt Key to compare for search
 * \param Pointer to key-value pair, this is only NULL for errors.
 */
MCR_API void *mcr_Map_element_ensured(struct mcr_Map *mapPt, const void *keyPt);
/*!
 * \brief Get the index of an item in the array. -1 if out of bounds
 *
 * \param key Pointer to key to find from
 * \return \ref retind
 */
MCR_API size_t mcr_Map_index(const struct mcr_Map *mapPt, const void *keyPt);
/* Set/remove mappings */
/*!
 * \brief Map from key to value.
 *
 * \param keyPt Pointer to key to copy from
 * \param valuePt (opt) Pointer to value to copy from.  If NULL,
 * mapped value will be zero.
 * \return \ref reterr
 */
MCR_API int mcr_Map_map(struct mcr_Map *mapPt, const void *keyPt,
	const void *valuePt);
/*!
 * \brief Remove mapped value from one mapping, and remap
 * it for a different key.
 *
 * \param previousKeyPt Pointer to key to remove from
 * \param newKeyPt (opt) Pointer to key to remap into
 * \return \ref reterr
 */
MCR_API int mcr_Map_remap(struct mcr_Map *mapPt, const void *previousKeyPt,
	const void *newKeyPt);
/*!
 * \brief Map key+value pair.
 *
 * \param mappingPair Pointer to key + value pair to
 * copy from
 * \return \ref reterr
 */
MCR_API int mcr_Map_map_pair(struct mcr_Map *mapPt, const void *mappingPair);
/*!
 * \brief Remove mapping for given key.
 *
 * \param key Pointer to key to find from
 */
MCR_API void mcr_Map_unmap(struct mcr_Map *mapPt, const void *keyPt);
MCR_API void mcr_Map_unmap_value(struct mcr_Map *mapPt, const void *valuePt);
MCR_API int mcr_Map_fill(struct mcr_Map *mapPt, const void *keyArray,
	size_t keyCount, const void *valuePt);
MCR_API int mcr_Map_graph(struct mcr_Map *mapPt, const void *keyArray,
	const void *valueArray, size_t sourceArrayLen);
/* Sorting */
/*!
 * \brief Sort with \ref qsort.
 *
 * The qsort function is the map compare function. qsort element
 * size is the size of both key and value.
 */
MCR_API void mcr_Map_sort(struct mcr_Map *mapPt);
/* comparison */
/*!
 * \brief Compare chars
 *
 * \param lhs const char *
 * \param rhs const char *
 * \return \ref retcmp
 */
MCR_API int mcr_char_compare(const void *lhs, const void *rhs);
/*!
 * \brief Compare const c-strings referenced by each pointer,
 * case insensitive.
 *
 * \param lhs const char ** or mcr_Array *
 * \param rhs const char ** or mcr_Array *
 * \return \ref retcmp
 */
MCR_API int mcr_name_compare(const void *lhs, const void *rhs);
/*!
 * \brief Compare const c-strings referenced by each pointer,
 * case sensitive.
 *
 * \param lhs const char ** or mcr_Array *
 * \param rhs const char ** or mcr_Array *
 * \return \ref retcmp
 */
MCR_API int mcr_str_compare(const void *lhs, const void *rhs);
/*!
 * \brief Compare integers referenced by each pointer.
 *
 * \param lhs int *
 * \param rhs int *
 * \return \ref retcmp
 */
MCR_API int mcr_int_compare(const void *lhs, const void *rhs);
/*!
 * \brief Compare unsigned integers referenced by each pointer.
 *
 * \param lhs unsigned int *
 * \param rhs unsigned int *
 * \return \ref retcmp
 */
MCR_API int mcr_unsigned_compare(const void *lhs, const void *rhs);
/*!
 * \brief Compare size_t referenced by each pointer.
 *
 * \param lhs size_t *
 * \param rhs size_t *
 * \return \ref retcmp
 */
MCR_API int mcr_size_t_compare(const void *lhs, const void *rhs);
/*!
 * \brief Compare void * referenced by each pointer.
 *
 * \param lhs void **
 * \param rhs void **
 * \return \ref retcmp
 */
MCR_API int mcr_ref_compare(const void *lhs, const void *rhs);

/*!
 * \brief Change address from key to value.
 *
 * \param map \ref mcr_Map
 * \param pairPt Address to change from key to value
 * \return void *
 */
#define MCR_MAP_VALUEOF(map, pairPt) \
((void *)(pairPt ? ((char *)(pairPt)) \
		+ (map).key_size : NULL))
/*! \ref mcr_Map_get
 *
 * \param map \ref mcr_Map
 * \param keyPt const void *
 * \return void *
 */
#define MCR_MAP_ELEMENT(map, keyPt) \
MCR_ARR_FIND((map).set, keyPt)
#define MCR_MAP_INDEXOF(map, elPt) \
MCR_ARR_INDEX((map).set, elPt)
/*!
 * \brief If compare is available, qsort given map.
 *
 * \param map \ref mcr_Map
 */
#define MCR_MAP_SORT(map) MCR_ARR_SORT ((map).set)
#define MCR_MAP_ITER(map, itHolder, endHolder, bytesHolder) \
MCR_ARR_ITER((map).set, itHolder, endHolder, bytesHolder)
#define MCR_MAP_ITER_RANGE(map, itHolder, endHolder, bytesHolder, \
		firstIndex, lastIndex) \
MCR_ARR_ITER_RANGE((map).set, itHolder, endHolder, bytesHolder, \
		firstIndex, lastIndex)

/*!
 * \brief Use the iterateFnc for every element of the map set,
 * with all variadic arguments.
 *
 * This will not create va_list or do any type casting on arguments.
 * \param map \ref mcr_Map
 * \param iterateFnc void (*) (void *, ...) This function must be able
 * to accept both the key-value pair member reference, and all variadic
 * parameters given to this macro.
 */
#define MCR_MAP_FOR_EACH(map, iterateFnc, ...) \
MCR_ARR_FOR_EACH((map).set, iterateFnc, __VA_ARGS__)
/*!
 * \brief Use the iterateFnc for every value of the map set,
 * with all variadic arguments.
 *
 * In this case value means the second element of all key-value pairs.
 * This will not create va_list or do any type checking on variable
 * arguments.
 * \param map \ref mcr_Map
 * \param iterateFnc void (*) (void *, ...) This function must be able
 * to accept both the key-value pair member reference, and all variadic
 * parameters given to this macro.
 */
#define MCR_MAP_FOR_EACH_VALUE(map, iterateFnc, ...) \
{ \
	MCR_MAP_ITER (map, char *local_it, char *local_end, \
			size_t local_bytes); \
	local_it = MCR_MAP_VALUEOF (map, local_it); \
	while (local_it < local_end) { \
		iterateFnc ((void *)local_it, __VA_ARGS__); \
		local_it += local_bytes; \
	} \
}
#define MCR_MAP_RANGE(map, firstIndex, lastIndex, iterateFnc, ...) \
MCR_ARR_RANGE ((map).set, firstIndex, lastIndex, iterateFnc, \
		__VA_ARGS__)
#define MCR_MAP_RANGE_VALUE(map, firstIndex, lastIndex, \
		iterateFnc, ...) \
{ \
	MCR_MAP_ITER_RANGE (map, char *local_it, char *local_end, \
		size_t local_bytes, firstIndex, lastIndex); \
	local_it = MCR_MAP_VALUEOF (map, local_it); \
	while (local_it < local_end) { \
		iterateFnc ((void *)local_it, __VA_ARGS__); \
		local_it += local_bytes; \
	} \
}

#endif
