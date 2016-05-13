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

/*! \file */

#ifndef MCR_BIMAP_H
#define MCR_BIMAP_H

#include "mcr/util/map.h"

struct mcr_BiMap {
	struct mcr_Map first;
	struct mcr_Map second;
};

/*! See \ref mcr_Map_init */
MCR_API void mcr_BiMap_init(void *dataPt);
/*! dtor */
MCR_API void mcr_BiMap_free(void *dataPt);
/*! See \ref mcr_BiMap_free */
#define mcr_BiMap_free_foreach(mapPt, ignore) mcr_BiMap_free(mapPt)
MCR_API void mcr_BiMap_set_all(struct mcr_BiMap *mapPt, size_t sizeofFirst,
	size_t sizeofSecond, mcr_compare_fnc cmpFirst,
	mcr_compare_fnc cmpSecond, const struct mcr_Interface *ifaceFirst,
	const struct mcr_Interface *ifaceSecond);
/*!
 * \brief Map all first keys to second values, and all second keys to
 * first values.
 *
 * \param firstValuePt (opt) Second keys will map to this value.
 * If null all second keys will be unmapped.
 */
MCR_API int mcr_BiMap_map(struct mcr_BiMap *mapPt,
	const void *firstKeyValuePt, const void *firstKeyArr,
	size_t firstKeyCount, const void *secondKeyValuePt,
	const void *secondKeyArr, size_t secondKeyCount);

MCR_API int mcr_BiMap_addfirst(struct mcr_BiMap *mapPt,
	const void *firstValuePt, const void *secondKeyArr,
	size_t secondKeyCount);
MCR_API int mcr_BiMap_addsecond(struct mcr_BiMap *mapPt,
	const void *secondValuePt, const void *firstKeyArr,
	size_t firstKeyCount);

MCR_API void mcr_BiMap_remfirst(struct mcr_BiMap *mapPt,
	const void *firstKeyValuePt, int removeAllOccurances);
MCR_API void mcr_BiMap_remsecond(struct mcr_BiMap *mapPt,
	const void *secondKeyValuePt, int removeAllOccurances);

/*!
 * \brief Get a pointer to first element type from a key pointer of
 * second type.
 */
MCR_API void *mcr_BiMap_first(const struct mcr_BiMap *mapPt,
	const void *secondKeyPt);
/*!
 * \brief Get a pointer to second element type from a key pointer of
 * first type.
 */
MCR_API void *mcr_BiMap_second(const struct mcr_BiMap *mapPt,
	const void *firstKeyPt);

#endif
