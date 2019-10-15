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

#include "mcr/util/util.h"

#include <stdlib.h>
#include <string.h>

int mcr_StringIndex_init(void *indexPt)
{
	struct mcr_StringIndex *localPt = indexPt;
	if (localPt) {
		memset(localPt, 0, sizeof(struct mcr_StringIndex));
		mcr_Map_init(&localPt->map);
		mcr_StringSet_init(&localPt->set);
		/// \todo size_t interface
		mcr_Map_set_all(&localPt->map, sizeof(mcr_String),
						sizeof(size_t), NULL, mcr_String_interface(), NULL);
	}
	return 0;
}

struct mcr_StringIndex mcr_StringIndex_new()
{
	struct mcr_StringIndex ret;
	mcr_StringIndex_init(&ret);
	return ret;
}

int mcr_StringIndex_deinit(void *indexPt)
{
	struct mcr_StringIndex *localPt = indexPt;
	if (localPt) {
		mcr_Map_deinit(&localPt->map);
		mcr_StringSet_deinit(&localPt->set);
	}
	return 0;
}

/* Allocation control */
int mcr_StringIndex_minused(struct mcr_StringIndex *indexPt, size_t minUsed)
{
	dassert(indexPt);
	return mcr_StringSet_minused(&indexPt->set, minUsed);
}

int mcr_StringIndex_minsize(struct mcr_StringIndex *indexPt, size_t minSize)
{
	dassert(indexPt);
	return mcr_StringSet_minsize(&indexPt->set, minSize);
}

int mcr_StringIndex_smartsize(struct mcr_StringIndex *indexPt,
							  size_t increasingCount)
{
	dassert(indexPt);
	return mcr_StringSet_smartsize(&indexPt->set, increasingCount);
}

void mcr_StringIndex_trim(struct mcr_StringIndex *indexPt)
{
	dassert(indexPt);
	mcr_Map_trim(&indexPt->map);
	mcr_StringSet_trim(&indexPt->set);
}

int mcr_StringIndex_resize(struct mcr_StringIndex *indexPt, size_t newSize)
{
	size_t prevUsed, i;
	dassert(indexPt);
	if (!newSize) {
		mcr_StringIndex_deinit(indexPt);
		return 0;
	}
	prevUsed = indexPt->set.used;
	if (newSize < prevUsed) {
		for (i = newSize; i < prevUsed; i++) {
			mcr_Map_unmap_value(&indexPt->map, &i);
		}
		mcr_Map_trim(&indexPt->map);
	}
	return mcr_StringSet_resize(&indexPt->set, newSize);
}

void mcr_StringIndex_clear(struct mcr_StringIndex *indexPt)
{
	dassert(indexPt);
	mcr_Map_clear(&indexPt->map);
	mcr_StringSet_clear(&indexPt->set);
}

/* Position/Values */
mcr_String *mcr_StringIndex_string(struct mcr_StringIndex *indexPt,
								   size_t index)
{
	dassert(indexPt);
	return MCR_STRINGINDEX_STRING(*indexPt, index);
}

const char *mcr_StringIndex_name(const struct mcr_StringIndex *indexPt,
								 size_t index)
{
	mcr_String *found;
	if (!indexPt)
		return NULL;
	found = MCR_STRINGINDEX_STRING(*indexPt, index);
	return found ? found->array : NULL;
}

size_t mcr_StringIndex_index(const struct mcr_StringIndex * indexPt,
								const char *strKey)
{
	size_t *found;
	if (!indexPt || !strKey)
		return (size_t)-1;
	found = mcr_Map_value(&indexPt->map, &strKey);
	return found ? *found : (size_t) ~ 0;
}

/* Set/remove mappings */
int mcr_StringIndex_map(struct mcr_StringIndex *indexPt,
						size_t index, const char *strKey)
{
	dassert(indexPt);
	dassert(index != (size_t)-1);
	if (strKey && strKey[0]) {
		if (mcr_StringSet_minused(&indexPt->set, index + 1))
			return mcr_err;
		if (mcr_StringSet_set(&indexPt->set, index, strKey))
			return mcr_err;
		if (mcr_Map_map(&indexPt->map, (void *)&strKey, &index))
			return mcr_err;
	}
	return 0;
}

int mcr_StringIndex_add(struct mcr_StringIndex *indexPt,
						size_t index, const char **addKeys, size_t addCount)
{
	size_t i;
	dassert(indexPt);
	if (!addKeys || !addCount)
		return 0;
	for (i = 0; i < addCount; i++) {
		if (mcr_Map_map(&indexPt->map, (void *)(addKeys + i), &index))
			return mcr_err;
	}
	return 0;
}

int mcr_StringIndex_reindex(struct mcr_StringIndex *indexPt,
							size_t curIndex, size_t newIndex)
{
	mcr_String *found;
	char *itPt, *end;
	size_t bytes;
	dassert(indexPt);
	if (curIndex == newIndex)
		return 0;
	found = mcr_StringSet_element(&indexPt->set, curIndex);
	if (!found)
		return 0;
	if (mcr_StringSet_set(&indexPt->set, newIndex, found->array))
		return mcr_err;
	mcr_String_deinit(found);
	mcr_Map_iter(&indexPt->map, &itPt, &end, &bytes);
	itPt = MCR_MAP_VALUEOF(indexPt->map, itPt);
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
	if (strKey == newKey)
		return 0;
	if (newKey) {
		if (!(found = mcr_Map_value(&indexPt->map, &strKey))) {
			mcr_StringIndex_unmap_string(indexPt, newKey, true);
			return 0;
		}
		mapVal = *found;
		if (mcr_StringIndex_map(indexPt, mapVal, newKey)) {
			return mcr_err;
		}
	}
	return mcr_Map_unmap(&indexPt->map, &strKey);
}

void mcr_StringIndex_unmap(struct mcr_StringIndex *indexPt,
						   size_t remIndex, bool flagRemoveAll)
{
	mcr_String *found = MCR_STRINGSET_ELEMENT(indexPt->set, remIndex);
	dassert(indexPt);
	if (flagRemoveAll) {
		mcr_Map_unmap_value(&indexPt->map, &remIndex);
	} else if (found && found->array) {
		mcr_Map_unmap(&indexPt->map, &found);
	}
	if (found)
		mcr_String_deinit(found);
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
		mcr_Map_unmap(&indexPt->map, &remString);
	}
}

void mcr_StringIndex_sort(struct mcr_StringIndex *indexPt)
{
	dassert(indexPt);
	mcr_Map_sort(&indexPt->map);
	mcr_StringSet_sort(&indexPt->set);
}
