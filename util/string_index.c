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
#include <stdlib.h>
#include <string.h>

void mcr_StringIndex_init(void *dataPt)
{
	struct mcr_StringIndex *indexPt = dataPt;
	dassert(indexPt);
	memset(indexPt, 0, sizeof(struct mcr_StringIndex));
	mcr_StringMap_init(&indexPt->map);
	mcr_StringSet_init(&indexPt->set);
	indexPt->map.value_size = sizeof(size_t);
}

void mcr_StringIndex_free(void *dataPt)
{
	struct mcr_StringIndex *indexPt = dataPt;
	dassert(indexPt);
	mcr_StringMap_free(&indexPt->map);
	mcr_StringSet_free(&indexPt->set);
}

/* */
/* Allocation control */
/* */
int mcr_StringIndex_minused(struct mcr_StringIndex *indexPt, size_t minimumUsed)
{
	dassert(indexPt);
	return mcr_StringSet_minused(&indexPt->set, minimumUsed);
}

int mcr_StringIndex_minsize(struct mcr_StringIndex *indexPt, size_t minimumSize)
{
	dassert(indexPt);
	return mcr_StringSet_minsize(&indexPt->set, minimumSize);
}

int mcr_StringIndex_smartsize(struct mcr_StringIndex *indexPt,
	size_t increasingCount)
{
	dassert(indexPt);
	return mcr_StringSet_smartsize(&indexPt->set, increasingCount);
}

/*! Minimize allocated space.
 */
void mcr_StringIndex_trim(struct mcr_StringIndex *indexPt)
{
	dassert(indexPt);
	mcr_StringMap_trim(&indexPt->map);
	mcr_StringSet_trim(&indexPt->set);
}

int mcr_StringIndex_resize(struct mcr_StringIndex *indexPt, size_t newSize)
{
	dassert(indexPt);
	size_t prevUsed, i;
	if (!newSize) {
		mcr_StringIndex_free(indexPt);
		return 0;
	}
	prevUsed = indexPt->set.used;
	if (newSize < prevUsed) {
		for (i = newSize; i < prevUsed; i++) {
			mcr_StringMap_unmap_value(&indexPt->map, &i);
		}
		mcr_StringMap_trim(&indexPt->map);
	}
	return mcr_StringSet_resize(&indexPt->set, newSize);
}

/*! Remove all mapped key-value pairs.
 */
void mcr_StringIndex_clear(struct mcr_StringIndex *indexPt)
{
	dassert(indexPt);
	mcr_StringMap_clear(&indexPt->map);
	mcr_StringSet_clear(&indexPt->set);
}

/* */
/* Position/Values */
/* */
mcr_String *mcr_StringIndex_string(struct mcr_StringIndex *indexPt,
	size_t index)
{
	dassert(indexPt);
	return MCR_STRINGINDEX_STRING(*indexPt, index);
}

const char *mcr_StringIndex_name(struct mcr_StringIndex *indexPt, size_t index)
{
	mcr_String *found = MCR_STRINGINDEX_STRING(*indexPt, index);
	dassert(indexPt);
	return found ? found->array : NULL;
}

size_t mcr_StringIndex_index(struct mcr_StringIndex * indexPt,
	const char *strKey)
{
	size_t *found;
	dassert(indexPt);
	if (!strKey)
		return ~0;
	found = mcr_StringMap_value(&indexPt->map, strKey);
	return found ? *found : (size_t) ~ 0;
}

/* Set/remove mappings */
int mcr_StringIndex_map(struct mcr_StringIndex *indexPt,
	size_t index, const char *strKey, const char **addKeys, size_t addCount)
{
	int ret;
	dassert(indexPt);
	dassert(index != (size_t) ~ 0);
	if (strKey) {
		ret = mcr_StringSet_set(&indexPt->set, index, strKey);
		if (!ret)
			ret = mcr_StringMap_map(&indexPt->map, strKey, &index);
		if (ret)
			return ret;
	}
	if (addKeys && addCount)
		return mcr_StringMap_fill(&indexPt->map, addKeys, addCount,
			&index);
	return 0;
}

int mcr_StringIndex_add(struct mcr_StringIndex *indexPt,
	size_t index, const char **addKeys, size_t addCount)
{
	dassert(indexPt);
	return mcr_StringMap_fill(&indexPt->map, addKeys, addCount, &index);
}

int mcr_StringIndex_reindex(struct mcr_StringIndex *indexPt,
	size_t curIndex, size_t newIndex)
{
	mcr_String *found;
	int ret;
	char *itPt, *end;
	size_t bytes;
	dassert(indexPt);
	if (curIndex == newIndex)
		return 0;
	found = mcr_StringSet_element(&indexPt->set, curIndex);
	if (!found)
		return 0;
	if ((ret = mcr_StringSet_set(&indexPt->set, newIndex, found->array)))
		return ret;
	mcr_String_free(found);
	MCR_STRINGMAP_ITER(indexPt->map, itPt, end, bytes);
	itPt = MCR_STRINGMAP_VALUEOF(indexPt->map, itPt);
	while (itPt < end) {
		if (*(size_t *) itPt == curIndex)
			*(size_t *) itPt = newIndex;
		itPt += bytes;
	}
	return 0;
}

int mcr_StringIndex_remap(struct mcr_StringIndex *indexPt,
	const char *strKey, const char *newKey)
{
	size_t mapVal, *found;
	dassert(indexPt);
	dassert(strKey);
	if (newKey) {
		found = mcr_StringMap_value(&indexPt->map, strKey);
		if (!found)
			return 0;
		mapVal = *found;
		mcr_StringMap_unmap(&indexPt->map, strKey);
		return mcr_StringIndex_map(indexPt, mapVal, newKey, NULL, 0);
	} else {
		mcr_StringMap_unmap(&indexPt->map, strKey);
	}
	return 0;
}

void mcr_StringIndex_unmap(struct mcr_StringIndex *indexPt,
	size_t remIndex, bool flagRemoveAll)
{
	mcr_String *found = MCR_STRINGSET_ELEMENT(indexPt->set, remIndex);
	dassert(indexPt);
	if (flagRemoveAll) {
		mcr_StringMap_unmap_value(&indexPt->map, &remIndex);
	} else if (found && found->array) {
		mcr_StringMap_unmap(&indexPt->map, found->array);
	}
	if (found)
		mcr_String_free(found);
}

void mcr_StringIndex_unmap_string(struct mcr_StringIndex *indexPt,
	const char *remString, bool flagRemoveAll)
{
	size_t *found;
	dassert(indexPt);
	if (!remString)
		return;
	found = MCR_MAP_ELEMENT(indexPt->map, &remString);
	found = MCR_MAP_VALUEOF(indexPt->map, found);
	if (found) {
		mcr_StringIndex_unmap(indexPt, *found, flagRemoveAll);
		mcr_StringMap_unmap(&indexPt->map, remString);
	}
}

void mcr_StringIndex_sort(struct mcr_StringIndex *indexPt)
{
	dassert(indexPt);
	mcr_StringMap_sort(&indexPt->map);
	mcr_StringSet_sort(&indexPt->set);
}
