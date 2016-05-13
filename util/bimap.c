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

#include "mcr/util/util.h"

void mcr_BiMap_init(void *dataPt)
{
	struct mcr_BiMap *mapPt = dataPt;
	if (mapPt) {
		mcr_Map_init(&mapPt->first);
		mcr_Map_init(&mapPt->second);
	}
}

void mcr_BiMap_free(void *dataPt)
{
	struct mcr_BiMap *mapPt = dataPt;
	if (mapPt) {
		mcr_Map_free(&mapPt->first);
		mcr_Map_free(&mapPt->second);
	}
}

void mcr_BiMap_set_all(struct mcr_BiMap *mapPt, size_t sizeofFirst,
	size_t sizeofSecond, mcr_compare_fnc cmpFirst,
	mcr_compare_fnc cmpSecond, const struct mcr_Interface *ifaceFirst,
	const struct mcr_Interface *ifaceSecond)
{
	dassert(mapPt);
	mcr_Map_set_all(&mapPt->first, sizeofFirst, sizeofSecond, cmpFirst,
		ifaceFirst, ifaceSecond);
	mcr_Map_set_all(&mapPt->second, sizeofSecond, sizeofFirst, cmpSecond,
		ifaceSecond, ifaceFirst);
}

int mcr_BiMap_map(struct mcr_BiMap *mapPt,
	const void *firstKeyValuePt, const void *firstKeyArr,
	size_t firstKeyCount, const void *secondKeyValuePt,
	const void *secondKeyArr, size_t secondKeyCount)
{
	int ret;
	dassert(mapPt);
	if (secondKeyValuePt) {
		/* first -> second */
		if (firstKeyValuePt) {
			if ((ret = mcr_Map_map(&mapPt->first, firstKeyValuePt,
						secondKeyValuePt)))
				return ret;
		}
		/* first key set -> second */
		if (firstKeyArr && firstKeyCount) {
			if ((ret = mcr_Map_fill(&mapPt->first, firstKeyArr,
						firstKeyCount,
						secondKeyValuePt)))
				return ret;
		}
	}
	if (firstKeyValuePt) {
		/* second -> first */
		if (secondKeyValuePt) {
			if ((ret = mcr_Map_map(&mapPt->second, secondKeyValuePt,
						secondKeyValuePt)))
				return ret;
		}
		/* second key set -> first */
		if (secondKeyArr && secondKeyCount) {
			if ((ret = mcr_Map_fill(&mapPt->second, secondKeyArr,
						secondKeyCount,
						secondKeyValuePt)))
				return ret;
		}
	}
	return 0;
}

int mcr_BiMap_addfirst(struct mcr_BiMap *mapPt,
	const void *firstValuePt, const void *secondKeyArr,
	size_t secondKeyCount)
{
	dassert(mapPt);
	return mcr_Map_fill(&mapPt->second, secondKeyArr, secondKeyCount,
		firstValuePt);
}

int mcr_BiMap_addsecond(struct mcr_BiMap *mapPt,
	const void *secondValuePt, const void *firstKeyArr,
	size_t firstKeyCount)
{
	dassert(mapPt);
	return mcr_Map_fill(&mapPt->first, firstKeyArr, firstKeyCount,
		secondValuePt);
}

void mcr_BiMap_remfirst(struct mcr_BiMap *mapPt,
	const void *firstKeyValuePt, int removeAllOccurances)
{
	dassert(mapPt);
	mcr_Map_unmap(&mapPt->first, firstKeyValuePt);
	if (removeAllOccurances)
		mcr_Map_unmap_value(&mapPt->second, firstKeyValuePt);
}

void mcr_BiMap_remsecond(struct mcr_BiMap *mapPt,
	const void *secondKeyValuePt, int removeAllOccurances)
{
	dassert(mapPt);
	mcr_Map_unmap(&mapPt->second, secondKeyValuePt);
	if (removeAllOccurances)
		mcr_Map_unmap_value(&mapPt->first, secondKeyValuePt);
}

void *mcr_BiMap_first(const struct mcr_BiMap *mapPt, const void *secondKeyPt)
{
	dassert(mapPt);
	return mcr_Map_value(&mapPt->second, secondKeyPt);
}

void *mcr_BiMap_second(const struct mcr_BiMap *mapPt, const void *firstKeyPt)
{
	dassert(mapPt);
	return mcr_Map_value(&mapPt->first, firstKeyPt);
}
