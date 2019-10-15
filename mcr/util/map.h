/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

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
 *  \brief \ref mcr_Map - Dynamic resizing map from one type to another for
 *  fast retrieval.
 *
 *  Array items are stored as the size of paired key-value sizes.
 *  \ref qsort
 *  \ref bsearch
 */

#ifndef MCR_UTIL_MAP_H_
#define MCR_UTIL_MAP_H_

#include "mcr/util/array.h"
#include "mcr/util/interface.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Map from one type to another.
 *
 *  The map should be sorted at all times.  All functions that add elements
 *  will also sort.  All functions that remove values assume the map will
 *  still be sorted after items are removed. */
struct mcr_Map {
	/*! Resizing set of all items
	 *
	 *  Size of one mapped element = \ref mcr_Map.key_size +
	 *  \ref mcr_Map.value_size
	 */
	struct mcr_Array set;
	/* Internal */
	/*! Size of mapping key elements */
	size_t key_size;
	/*! Size of mapped value elements */
	size_t value_size;
	/*! \ref opt Interface of key elements.
	 *
	 *  Elements will be initialized, freed, copied, and
	 *  compared with this interface */
	const struct mcr_Interface *key_interface;
	/*! \ref opt Interface of value elements
	 *
	 *  Elements will be initialized, freed, copied, and
	 *  compared with this interface */
	const struct mcr_Interface *value_interface;
};

/*! \ref mcr_Map ctor
 *
 *  \param mapPt \ref opt \ref mcr_Map *
 *  \return 0
 */
MCR_API int mcr_Map_init(void *mapPt);
/*! \ref mcr_Map_init and \ref mcr_Map_set_all
 *
 *  \param keySize \ref mcr_Map.key_size
 *  \param valueSize \ref mcr_Map.value_size
 *  \param compare \ref opt \ref mcr_Array.compare
 *  \param keyIface \ref opt \ref mcr_Map.key_interface, overrides keySize and
 *  compare
 *  \param valueIface \ref opt \ref mcr_Map.value_interface, overrides
 *  valueSize
 *  \return Empty map
 */
MCR_API struct mcr_Map mcr_Map_new(size_t keySize,
								   size_t valueSize, mcr_compare_fnc compare,
								   const struct mcr_Interface *keyIface,
								   const struct mcr_Interface *valueIface);
/*! \post The map will be available to use again with only the array changed.
 *  \brief \ref mcr_Map dtor
 *
 *  \param mapPt \ref opt \ref mcr_Map *
 *  \return 0
 */
MCR_API int mcr_Map_deinit(void *mapPt);
/*! \pre Size and comparison will be taken from interfaces if they are
 *  available, otherwize size and comparison must exist.
 *  \brief Set initial values
 *
 *  \param keySize \ref mcr_Map.key_size
 *  \param valueSize \ref mcr_Map.value_size
 *  \param compare \ref opt \ref mcr_Array.compare
 *  \param keyIface \ref opt \ref mcr_Map.key_interface, overrides keySize and
 *  compare
 *  \param valueIface \ref opt \ref mcr_Map.value_interface, overrides
 *  valueSize
 */
MCR_API int mcr_Map_set_all(struct mcr_Map *mapPt, size_t keySize,
							size_t valueSize, mcr_compare_fnc compare,
							const struct mcr_Interface *keyIface,
							const struct mcr_Interface *valueIface);

/* Allocation control */
/*! Set a minimum number of used elements and resize
 *  if needed.
 *
 *  \param minUsed Minimum number of used elements
 *  \return \ref reterr
 */
MCR_API int mcr_Map_minused(struct mcr_Map *mapPt, size_t minUsed);
/*! Set a minimum size, and resize if needed.
 *
 *  \param minSize Minimum number of allocated elements
 *  \return \ref reterr
 */
MCR_API int mcr_Map_minsize(struct mcr_Map *mapPt, size_t minSize);
/*! Apply a smart resizing algorithm for adding a number of
 *  elements.
 *
 *  \param increasingCount Number of elements to add
 *  \return \ref reterr
 */
MCR_API int mcr_Map_smartsize(struct mcr_Map *mapPt, size_t increasingCount);
/*! Minimize allocated space. */
MCR_API void mcr_Map_trim(struct mcr_Map *mapPt);
/*! Reallocate map to given size.
 *
 *  \param newSize New allocated number of elements
 *  \return \ref reterr
 */
MCR_API int mcr_Map_resize(struct mcr_Map *mapPt, size_t newSize);
/*! Remove all mapped key-value pairs.
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Map_clear(struct mcr_Map *mapPt);

/* Position */
/*! Convert to the value of a key-value element
 *
 *  \param mapPt \ref opt
 *  \param pairPt \ref opt Pointer to an element
 *  \return Value of pair, or null
 */
MCR_API void *mcr_Map_valueof(const struct mcr_Map *mapPt, void *pairPt);
/*! Change the key of a key-value element
 *
 *  \param mapPt \ref opt
 *  \param pairPt Pointer to an element
 *  \param copyKeyPt \ref opt New key to set
 *  \return \ref reterr
 */
MCR_API int mcr_Map_set_key(const struct mcr_Map *mapPt, void *pairPt,
							void *copyKeyPt);
/*! Change the value of a key-value element
 *
 *  \param mapPt \ref opt
 *  \param pairPt Pointer to an element
 *  \param copyKeyPt \ref opt New value to set
 *  \return \ref reterr
 */
MCR_API int mcr_Map_set_valueof(const struct mcr_Map *mapPt, void *pairPt,
								void *copyValuePt);
/*! Get a key-value pair, mapped from the key pointer.
 *
 *  \param mapPt \ref opt
 *  \param keyPt \ref opt Pointer to key to find from
 *  \return Pointer to a key-value pair, or null if not found
 */
MCR_API void *mcr_Map_element(const struct mcr_Map *mapPt, const void *keyPt);
/*! Get a pointer to the value mapped from key pointer.
 *
 *  \param mapPt \ref opt
 *  \param keyPt \ref opt Pointer to key to find from
 *  \return Pointer to a value, or null if not found
 */
MCR_API void *mcr_Map_value(const struct mcr_Map *mapPt, const void *keyPt);
/*! Get a key-value pair. A backup value will be mapped if
 *  key is not yet mapped.
 *
 *  \param keyPt Pointer to key to find from
 *  \return Pointer to key-value pair, this is only null for errors.
 */
MCR_API void *mcr_Map_element_ensured(struct mcr_Map *mapPt, void *keyPt);
/*! Get the index of an item in the array
 *
 *  \param keyPt \ref opt Pointer to key to find from
 *  \return \ref retind
 */
MCR_API size_t mcr_Map_index(const struct mcr_Map *mapPt, const void *keyPt);
/*! Create an iterator. \ref mcr_Arrray_iter
 *
 *  If iterator is null, end will be null, and vise versa.  Null checks
 *  are suggested for equality(e.g. !=) comparisons.
 *  \param mapPt \ref opt
 *  \param iterPt \ref opt Set to first element
 *  \param endPt \ref opt Set to one after last element
 *  \param bytesPt \ref opt \ref mcr_Map.set.element_size
 */
MCR_API void mcr_Map_iter(const struct mcr_Map *mapPt, char **iterPt,
						  char **endPt, size_t * bytesPt);
/*! Create a range iterator. \ref mcr_Array_iter_range
 *
 *  Depending on the indices used, the iterator or last element might be null.
 *  A null check is suggested, and a for-loop is not suggested.\n
 *  \param mapPt \ref opt
 *  \param iterPt \ref opt Set to first element in range
 *  \param lastPt \ref opt Set to last element in range
 *  \param bytesPt \ref opt \ref mcr_Map.set.element_size
 *  \param firstIndex Index of first element in range
 *  \param lastIndex Index of last element in range
 */
MCR_API void mcr_Map_iter_range(const struct mcr_Map *mapPt, char **iterPt,
								char **lastPt, size_t* bytesPt, size_t firstIndex,
								size_t lastIndex);

/* Set/remove mappings */
/*! Map from key to value.
 *
 *  \param keyPt Pointer to key to copy from
 *  \param valuePt \ref opt Pointer to value to copy from
 *  \return \ref reterr
 */
MCR_API int mcr_Map_map(struct mcr_Map *mapPt, void *keyPt, void *valuePt);
/*! Remove mapped value from one mapping, and remap
 *  it for a different key.
 *
 *  \param previousKeyPt \ref opt Pointer to key to remove from
 *  \param newKeyPt \ref opt Pointer to key to remap into
 *  \return \ref reterr
 */
MCR_API int mcr_Map_remap(struct mcr_Map *mapPt, const void *previousKeyPt,
						  void *newKeyPt);
/*! Map a key-value pair.
 *
 *  \param mappingPair Pointer to a key-value pair to
 *  copy from
 *  \return \ref reterr
 */
MCR_API int mcr_Map_map_pair(struct mcr_Map *mapPt, void *mappingPair);
/*! Remove mapping for given key.
 *
 *  \param keyPt \ref opt Pointer to key to remove
 *  \return \ref reterr
 */
MCR_API int mcr_Map_unmap(struct mcr_Map *mapPt, const void *keyPt);
/*! Remove all mappings for a value
 *
 *  \param valuePt \ref opt Pointer to value to remove
 *  \return \ref reterr
 */
MCR_API int mcr_Map_unmap_value(struct mcr_Map *mapPt, const void *valuePt);
/*! Map a set of keys to the same value
 *
 *  \param keyArray \ref opt A set of keys to map
 *  \param keyCount Number of keys to map
 *  \param valuePt \ref opt Pointer to value to map into
 *  \return \ref reterr
 */
MCR_API int mcr_Map_fill(struct mcr_Map *mapPt, void *keyArray,
						 size_t keyCount, void *valuePt);
/*! Map a set of keys to a set of values
 *
 *  \param keyArray \ref opt A set of keys to map
 *  \param valueArray \ref opt A set of values to map into
 *  \param sourceArrayLen Number of keys and values to map
 *  \return \ref reterr
 */
MCR_API int mcr_Map_graph(struct mcr_Map *mapPt, void *keyArray,
						  void *valueArray, size_t sourceArrayLen);

/* Sorting */
/*! \pre Map may be sorted or not
 *  \post Map will be sorted
 *  \brief Sort, usually done automatically
 */
MCR_API void mcr_Map_sort(struct mcr_Map *mapPt);

/* Some common comparison functions */
/*! Compare chars
 *
 *  \param lhs \ref opt const char *
 *  \param rhs \ref opt const char *
 *  \return \ref retcmp
 */
MCR_API int mcr_char_compare(const void *lhs, const void *rhs);
/*! Compare const c-strings referenced by each pointer,
 *  case insensitive.
 *
 *  \param lhs \ref opt const char * const* or mcr_Array *
 *  \param rhs \ref opt const char * const* or mcr_Array *
 *  \return \ref retcmp
 */
MCR_API int mcr_name_compare(const void *lhs, const void *rhs);
/*! Compare const c-strings referenced by each pointer,
 *  case sensitive.
 *
 *  \param lhs \ref opt const char * const* or mcr_Array *
 *  \param rhs \ref opt const char * const* or mcr_Array *
 *  \return \ref retcmp
 */
MCR_API int mcr_str_compare(const void *lhs, const void *rhs);
/*! Compare integers referenced by each pointer.
 *
 *  \param lhs \ref opt int *
 *  \param rhs \ref opt int *
 *  \return \ref retcmp
 */
MCR_API int mcr_int_compare(const void *lhs, const void *rhs);
/*! Compare unsigned integers referenced by each pointer.
 *
 *  \param lhs \ref opt unsigned int *
 *  \param rhs \ref opt unsigned int *
 *  \return \ref retcmp
 */
MCR_API int mcr_unsigned_compare(const void *lhs, const void *rhs);
/*! Compare void * referenced by each pointer.
 *
 *  \param lhs \ref opt void **
 *  \param rhs \ref opt void **
 *  \return \ref retcmp
 */
MCR_API int mcr_ref_compare(const void *lhs, const void *rhs);

/*! \ref mcr_Map_valueof */
#define MCR_MAP_VALUEOF(map, pairPt) \
mcr_castpt(void, pairPt ? mcr_castpt(char, pairPt) \
		+ (map).key_size : mcr_null)

/*! \ref mcr_Map_element */
#define MCR_MAP_ELEMENT(map, keyPt) \
mcr_Array_find(&(map).set, keyPt)

/*! \ref mcr_Map_index */
#define MCR_MAP_INDEX(map, elPt) \
MCR_ARR_INDEX((map).set, elPt)

/*! If compare is available, qsort given map. */
#define MCR_MAP_SORT(map) mcr_Array_sort(&(map).set)

/*! \ref MCR_ARR_FOR_EACH For each element */
#define MCR_MAP_FOR_EACH(map, iterateFnc) \
MCR_ARR_FOR_EACH((map).set, iterateFnc)

/*! \ref MCR_ARR_FOR_EACH For each value */
#define MCR_MAP_FOR_EACH_VALUE(map, iterateFnc) \
{ \
	char *local_it, *local_end; \
	size_t local_bytes; \
	mcr_Map_iter(&(map), &local_it, &local_end, \
			&local_bytes); \
	local_it = mcr_castpt(char, MCR_MAP_VALUEOF(map, local_it)); \
	while (local_it < local_end) { \
		iterateFnc(mcr_castpt(void, local_it)); \
		local_it += local_bytes; \
	} \
}

#ifdef __cplusplus
}
#endif

#endif
