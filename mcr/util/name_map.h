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

#ifndef MCR_NAMEMAP_H
#define MCR_NAMEMAP_H

#include "mcr/util/mcrstring.h"
#include "mcr/util/map.h"

/*!
 * \brief \ref mcr_Map designed to map from value keys to
 * \ref mcr_String names
 */
typedef struct mcr_Map mcr_NameMap;

MCR_API void mcr_NameMap_init(void *dataPt);
#define mcr_NameMap_free mcr_Map_free
#define mcr_NameMap_free_foreach(mapPt, ignore) mcr_Map_free(mapPt)
MCR_API void mcr_NameMap_set_all(mcr_NameMap * mapPt, size_t keySize,
	mcr_compare_fnc compareFnc, const struct mcr_Interface *keyIface);
/* Allocation control */
/*! \ref mcr_Map_minused, also initializing any new strings */
#define mcr_NameMap_minused mcr_Map_minused
/*! \ref mcr_Map_minsize */
#define mcr_NameMap_minsize mcr_Map_minsize
/*! \ref mcr_Map_smartsize */
#define mcr_NameMap_smartsize mcr_Map_smartsize
/*! \ref mcr_Map_trim */
#define mcr_NameMap_trim mcr_Map_trim
/*! \ref mcr_Map_resize */
#define mcr_NameMap_resize mcr_Map_resize
/*! \ref mcr_Map_clear, also freeing string values */
#define mcr_NameMap_clear mcr_Map_clear
/* Position */
/*! \ref mcr_Map_valueof */
MCR_API mcr_String *mcr_NameMap_valueof(const mcr_NameMap * mapPt,
	const void *pairPt);
/*! \ref mcr_Map_set_key */
#define mcr_NameMap_set_key mcr_Map_set_key
/*! \ref mcr_Map_set_valueof */
MCR_API void mcr_NameMap_set_valueof(const mcr_NameMap * mapPt,
	void *pairPt, const char *strValue);
#define mcr_NameMap_element mcr_Map_element
MCR_API mcr_String *mcr_NameMap_value(const mcr_NameMap * mapPt,
	const void *keyPt);
MCR_API const char *mcr_NameMap_name(const mcr_NameMap * mapPt,
	const void *keyPt);
/*! \ref mcr_Map_element_ensured */
#define mcr_NameMap_element_ensured mcr_Map_element_ensured
/*! \ref mcr_Map_index */
#define mcr_NameMap_index mcr_Map_index
/*! \ref mcr_Map_for_each */
#define mcr_NameMap_for_each mcr_Map_for_each
/*! \ref mcr_Map_for_each_value */
#define mcr_NameMap_for_each_value mcr_Map_for_each_value
/* Set/remove mappings */
MCR_API int mcr_NameMap_map(mcr_NameMap * mapPt, const void *keyPt,
	const char *valueStr);
MCR_API int mcr_NameMap_nmap(mcr_NameMap * mapPt, const void *keyPt,
	const char *valueStr, size_t valueLen);
#define mcr_NameMap_remap mcr_Map_remap
/*! \ref mcr_Map_map_pair */
#define mcr_NameMap_map_pair mcr_Map_map_pair
#define mcr_NameMap_unmap mcr_Map_unmap
MCR_API void mcr_NameMap_unmap_value(mcr_NameMap * mapPt, const char *valueStr);
MCR_API int mcr_NameMap_fill(mcr_NameMap * mapPt,
	const void *keyArray, size_t keyCount, const char *valueStr);
MCR_API int mcr_NameMap_graph(mcr_NameMap * mapPt,
	const void *keyArray, const char **valueStrArray,
	size_t sourceArrayLen);
/* Sorting */
/*! \brief \ref mcr_Map_sort */
#define mcr_NameMap_sort mcr_Map_sort

#define MCR_NAMEMAP_VALUEOF(map, pairPt) \
((mcr_String *)MCR_MAP_VALUEOF (map, pairPt))
/*!
 * \brief \ref mcr_NameMap_element
 *
 * \param map \ref mcr_NameMap
 * \return void *
 */
#define MCR_NAMEMAP_ELEMENT(map, keyPt) \
MCR_MAP_ELEMENT (map, keyPt)
#define MCR_NAMEMAP_INDEXOF(map, elPt) \
MCR_MAP_INDEXOF (map, elPt)
#define MCR_NAMEMAP_SORT(map) MCR_MAP_SORT (map)
#define MCR_NAMEMAP_ITER(map, itHolder, endHolder, bytesHolder) \
MCR_MAP_ITER (map, itHolder, endHolder, bytesHolder)
#define MCR_NAMEMAP_FOR_EACH(map, iterateFnc, ...) \
MCR_MAP_FOR_EACH (map, iterateFnc, __VA_ARGS__)
#define MCR_NAMEMAP_FOR_EACH_VALUE(map, iterateFnc, ...) \
MCR_MAP_FOR_EACH_VALUE (map, iterateFnc, __VA_ARGS__)
#define MCR_NAMEMAP_RANGE(map, firstIndex, lastIndex, \
		iterateFnc, ...) \
MCR_MAP_RANGE (map, firstIndex, lastIndex, iterateFnc, __VA_ARGS__)
#define MCR_NAMEMAP_RANGE_VALUE(map, firstIndex, lastIndex, \
		iterateFnc, ...) \
MCR_MAP_RANGE_VALUE (map, firstIndex, lastIndex, \
		iterateFnc, __VA_ARGS__)

#endif
