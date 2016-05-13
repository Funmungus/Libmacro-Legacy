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
 */

#ifndef MCR_STRINGMAP_H
#define MCR_STRINGMAP_H

#include "mcr/util/mcrstring.h"
#include "mcr/util/map.h"

/*! \brief \ref mcr_Map designed to map from \ref mcr_String keys. */
typedef struct mcr_Map mcr_StringMap;

/*!
 * \brief Initialize a map to map from a string.
 *
 * \param dataPt \ref mcr_StringMap
 */
MCR_API void mcr_StringMap_init(void *dataPt);
/*!
 * \brief Free both the contained strings, and the map.
 *
 * \param mapPt
 */
#define mcr_StringMap_free mcr_Map_free
/*! \ref mcr_StringMap_free */
#define mcr_StringMap_free_foreach(mapPt, ignore) \
mcr_StringMap_free(mapPt)
/*!
 * \brief Set value elements
 *
 * \param mapPt \ref mcr_StringMap
 * \param valueSize \ref mcr_Map.value_size
 * \param valueIface \ref mcr_Map.value_iface
 */
MCR_API void mcr_StringMap_set_all(mcr_StringMap * mapPt,
	size_t valueSize, const struct mcr_Interface *valueIface);
/* */
/* Allocation control */
/* */
/*! \ref mcr_Map_minused, also initializing any new strings */
#define mcr_StringMap_minused mcr_Map_minused
/*! \ref mcr_Map_minsize */
#define mcr_StringMap_minsize mcr_Map_minsize
/*! \ref mcr_Map_smartsize */
#define mcr_StringMap_smartsize mcr_Map_smartsize
/*! \ref mcr_Map_trim */
#define mcr_StringMap_trim mcr_Map_trim
/*! \ref mcr_Map_resize */
#define mcr_StringMap_resize mcr_Map_resize
/*! \ref mcr_Map_clear, also freeing string keys */
#define mcr_StringMap_clear mcr_Map_clear
/* */
/* Position */
/* */
/*! \ref mcr_Map_valueof */
#define mcr_StringMap_valueof mcr_Map_valueof
/*! \ref mcr_Map_set_key */
MCR_API int mcr_StringMap_set_key(const mcr_StringMap * mapPt,
	void *pairPt, const char *keyStr);
/*! \ref mcr_Map_set_valueof */
#define mcr_StringMap_set_valueof mcr_Map_set_valueof
/*!
 * \brief Get a reference to a mapped key+value pair.
 *
 * \param keyStr String mapped to pair
 * \return Pointer to mapped pair
 */
MCR_API mcr_String *mcr_StringMap_element(const mcr_StringMap * mapPt,
	const char *keyStr);
/*!
 * \brief Get a reference to a mapped value.
 *
 * \param keyStr String mapped to value
 * \return Pointer to mapped value
 */
MCR_API void *mcr_StringMap_value(const mcr_StringMap * mapPt,
	const char *keyStr);
/*! \ref mcr_Map_element_ensured */
MCR_API mcr_String *mcr_StringMap_element_ensured(mcr_StringMap * mapPt,
	const char *keyStr);
/*! \ref mcr_Map_index */
MCR_API size_t mcr_StringMap_index(const mcr_StringMap * mapPt,
	const char *keyStr);
/*! \ref mcr_Map_for_each */
#define mcr_StringMap_for_each mcr_Map_for_each
/*! \ref mcr_Map_for_each_value */
#define mcr_StringMap_for_each_value mcr_Map_for_each_value
/* */
/* Set/remove mappings */
/* */
/*!
 * \brief Map a string to given value.
 *
 * \param mapPt \ref mcr_StringMap *
 * \param strKey const char * String to map from
 * \param valuePt (opt) Value to map to
 * \return \ref reterr
 */
MCR_API int mcr_StringMap_map(mcr_StringMap * mapPt, const char *strKey,
	const void *valuePt);
/*!
 * \brief Map a string to a set of values.
 *
 * \param key String to map from
 * \param count Number of values to map to
 * \param valuePt (opt) Array of values to map to
 * \return \ref reterr
 */
MCR_API int mcr_StringMap_nmap(mcr_StringMap * mapPt, const char *strKey,
	size_t strLen, const void *valuePt);
/*! Remove mapping from one string, and map to another.
 *
 * \param key String currently mapped to value
 * \param newKey (opt) String to remap value from
 * \return \ref reterr
 */
MCR_API int mcr_StringMap_remap(mcr_StringMap * mapPt, const char *key,
	const char *newKey);
/*! \ref mcr_Map_map_pair */
#define mcr_StringMap_map_pair mcr_Map_map_pair
/*!
 * \brief Remove mapping from given string.
 *
 * \param key String to remove mapping from
 */
MCR_API void mcr_StringMap_unmap(mcr_StringMap * mapPt, const char *key);
#define mcr_StringMap_unmap_value mcr_Map_unmap_value
MCR_API int mcr_StringMap_fill(mcr_StringMap * mapPt,
	const char **keyStrArray, size_t keyCount, const void *valuePt);
MCR_API int mcr_StringMap_graph(mcr_StringMap * mapPt,
	const char **keyStrArray, const void *valueArray,
	size_t sourceArrayLen);
/* */
/* Sorting */
/* */
/*! \ref mcr_Map_sort
 */
#define mcr_StringMap_sort mcr_Map_sort

#define MCR_STRINGMAP_VALUEOF(map, pairPt) \
((void *)(pairPt ? ((char *)(pairPt)) \
		+ sizeof (mcr_String) : NULL))
/*! \ref mcr_StringMap_element
 *
 * \param map \ref mcr_StringMap
 * \param key const char * or const mcr_String
 * \return void *
 */
#define MCR_STRINGMAP_ELEMENT(map, key) \
((mcr_String *)(key ? MCR_MAP_ELEMENT (map, \
		(void *) &(key)) : NULL))
#define MCR_STRINGMAP_INDEXOF(map, elPt) \
MCR_MAP_INDEXOF (map, elPt)
#define MCR_STRINGMAP_SORT(map) MCR_MAP_SORT (map)
#define MCR_STRINGMAP_ITER(map, itHolder, endHolder, bytesHolder) \
MCR_MAP_ITER (map, itHolder, endHolder, bytesHolder)
#define MCR_STRINGMAP_FOR_EACH(map, iterateFnc, ...) \
MCR_MAP_FOR_EACH (map, iterateFnc, __VA_ARGS__)
#define MCR_STRINGMAP_FOR_EACH_VALUE(map, iterateFnc, ...) \
MCR_MAP_FOR_EACH_VALUE (map, iterateFnc, __VA_ARGS__)
#define MCR_STRINGMAP_RANGE(map, firstIndex, lastIndex, \
		iterateFnc, ...) \
MCR_MAP_RANGE (map, firstIndex, lastIndex, iterateFnc, __VA_ARGS__)
#define MCR_STRINGMAP_RANGE_VALUE(map, firstIndex, lastIndex, \
		iterateFnc, ...) \
MCR_MAP_RANGE_VALUE (map, firstIndex, lastIndex, \
		iterateFnc, __VA_ARGS__)

#endif
