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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void mcr_StringSet_init(void *dataPt)
{
	mcr_StringSet *setPt = dataPt;
	dassert(setPt);
	mcr_Array_init(dataPt);
	setPt->element_size = sizeof(mcr_String);
}

void mcr_StringSet_free(void *dataPt)
{
	mcr_StringSet *setPt = dataPt;
	dassert(setPt);
	MCR_STRINGSET_FOR_EACH(*setPt, MCR_EXP(mcr_String_free_foreach),);
	mcr_Array_free(setPt);
}

void mcr_StringSet_set_all(mcr_StringSet * setPt, mcr_compare_fnc compare)
{
	if (setPt->size)
		mcr_StringSet_free(setPt);
	mcr_StringSet_set_sorting(setPt, compare);
}

void mcr_StringSet_set_sorting(mcr_StringSet * setPt, mcr_compare_fnc compare)
{
	dassert(setPt);
	setPt->compare = compare;
	if (setPt->used)
		mcr_Array_sort(setPt);
}

int mcr_StringSet_minused(mcr_StringSet * setPt, size_t minimumUsed)
{
	dassert(setPt);
	if (setPt->used < minimumUsed) {
		mcr_String initial;
		mcr_String_init(&initial);
		return mcr_Array_minfill(setPt, minimumUsed, &initial);
	}
	return 0;
}

int mcr_StringSet_resize(mcr_StringSet * setPt, size_t newSize)
{
	dassert(setPt);
	if (newSize < setPt->used) {
		MCR_STRINGSET_RANGE(*setPt, newSize, setPt->used - 1,
			MCR_EXP(mcr_String_free_foreach),);
		return mcr_Array_resize(setPt, newSize);
	}
	return 0;
}

void mcr_StringSet_clear(mcr_StringSet * setPt)
{
	dassert(setPt);
	MCR_STRINGSET_FOR_EACH(*setPt, MCR_EXP(mcr_String_free_foreach),);
	setPt->used = 0;
}

int mcr_StringSet_insert(mcr_StringSet * setPt, size_t pos,
	const char **strArr, size_t count)
{
	dassert(setPt);
	if (!count)
		return 0;
	size_t prevUsed = setPt->used;
	int ret;
	/* Insert between, insert with all 0's, then fill with initial string */
	if (pos < prevUsed) {
		mcr_String initial;
		mcr_String_init(&initial);
		ret = mcr_Array_insert(setPt, pos, NULL, count);
		if (!ret)
			ret = mcr_Array_fill(setPt, pos, count, &initial);
		if (ret) {
			dmsg;
			return ret;
		}
	}
	/* Insert after, initialize all new positions with initial string */
	else {
		if ((ret = mcr_StringSet_minused(setPt, pos + count))) {
			dmsg;
			return ret;
		}
	}
	/* Starting at pos, replace all strings from strArr */
	if (strArr) {
		for (size_t i = 0; i < count; i++) {
			if ((ret = mcr_String_replace(MCR_STRINGSET_ELEMENT
						(*setPt, pos + i),
						strArr[i]))) {
				dmsg;
				return ret;
			}
		}
	}
	return 0;
}

void mcr_StringSet_remove_index(mcr_StringSet * setPt, size_t pos, size_t count)
{
	dassert(setPt);
	if (count > setPt->used || pos + count >= setPt->used)
		count = setPt->used - pos;
	MCR_STRINGSET_RANGE(*setPt, pos, pos + count - 1,
		MCR_EXP(mcr_String_free_foreach),);
	mcr_Array_remove_index(setPt, pos, count);
}

int mcr_StringSet_append(mcr_StringSet * setPt, const char **strArr,
	size_t count, bool flagUnique)
{
	dassert(setPt);
	if (!flagUnique)
		return mcr_StringSet_copy(setPt, setPt->used, strArr, count);
	if (!strArr || !count)
		return 0;
	mcr_String *found;
	int ret;
	if ((ret = mcr_StringSet_smartsize(setPt, count))) {
		dmsg;
		return ret;
	}
	for (size_t i = 0; i < count; i++) {
		if (!(found = mcr_StringSet_find(setPt, strArr[i]))) {
			if ((ret = mcr_StringSet_set(setPt, setPt->used,
						strArr[i]))) {
				dmsg;
				return ret;
			}
			if (setPt->compare)
				mcr_StringSet_sort(setPt);
		}
	}
	return 0;
}

int mcr_StringSet_push(mcr_StringSet * setPt, const char *copyStr,
	bool flagUnique)
{
	return mcr_StringSet_append(setPt, copyStr ? &copyStr : NULL, 1,
		flagUnique);
}

void mcr_StringSet_pop(mcr_StringSet * setPt)
{
	dassert(setPt);
	if (setPt->used) {
		mcr_String_free(MCR_STRINGSET_LAST(*setPt));
		--setPt->used;
	}
}

int mcr_StringSet_replace(mcr_StringSet * setPt,
	const char **arraySource, size_t count)
{
	dassert(setPt);
	int ret;
	if (!count) {
		mcr_StringSet_free(setPt);
		return 0;
	}
	if ((ret = mcr_StringSet_resize(setPt, count))) {
		dmsg;
		return ret;
	}
	return mcr_StringSet_copy(setPt, 0, arraySource, count);
}

int mcr_StringSet_copy(mcr_StringSet * dstPt, size_t dstPos,
	const char **srcArray, size_t count)
{
	dassert(dstPt);
	if (!count)
		return 0;
	int ret;
	if ((ret = mcr_StringSet_minused(dstPt, dstPos + count))) {
		dmsg;
		return ret;
	}
	if (srcArray) {
		for (size_t i = 0; i < count; i++) {
			if ((ret = mcr_String_replace(MCR_STRINGSET_ELEMENT
						(*dstPt, dstPos + i),
						srcArray[i]))) {
				dmsg;
				return ret;
			}
		}
	} else {
		MCR_STRINGSET_RANGE(*dstPt, dstPos, dstPos + count - 1,
			MCR_EXP(mcr_String_free_foreach),);
	}
	return 0;
}

int mcr_StringSet_set(mcr_StringSet * setPt, size_t pos, const char *copyStr)
{
	dassert(setPt);
	dassert(pos <= setPt->used);
	int ret = mcr_StringSet_minused(setPt, pos);
	if (ret) {
		dmsg;
		return ret;
	}
	if (copyStr)
		return mcr_String_replace(MCR_STRINGSET_ELEMENT(*setPt, pos),
			copyStr);
	mcr_String_free(MCR_STRINGSET_ELEMENT(*setPt, pos));
	return 0;
}

int mcr_StringSet_fill(mcr_StringSet * setPt, size_t pos,
	size_t count, const char *copyStr)
{
	dassert(setPt);
	int ret;
	if ((ret = mcr_StringSet_minused(setPt, pos + count))) {
		dmsg;
		return ret;
	}
	MCR_STRINGSET_ITER_RANGE(*setPt, char *itPt, char *end,
		size_t bytes, pos, pos + count - 1);
	while (itPt < end) {
		if ((ret = mcr_String_replace((mcr_String *) itPt, copyStr))) {
			dmsg;
			return ret;
		}
		itPt += bytes;
	}
	return 0;
}

void mcr_StringSet_sort(mcr_StringSet * setPt)
{
	dassert(setPt);
	MCR_STRINGSET_SORT(*setPt);
}

mcr_String *mcr_StringSet_find(const mcr_StringSet * setPt, const char *strKey)
{
	dassert(setPt);
	dassert(strKey);
	if (!setPt->used)
		return NULL;
	if (setPt->compare) {
		return MCR_STRINGSET_FIND(*setPt, strKey);
	}
	MCR_STRINGSET_ITER(*setPt, char *itPt, char *end, size_t bytes);
	while (itPt < end) {
		if (!strcasecmp(((mcr_String *) itPt)->array, strKey))
			return ((mcr_String *) itPt);
		itPt += bytes;
	}
	return NULL;
}

int mcr_StringSet_add(mcr_StringSet * setPt,
	const char **copyStr, size_t count, bool flagUnique)
{
	dassert(setPt);
	int ret;
	if (!count)
		return 0;
	if ((ret = mcr_StringSet_smartsize(setPt, count))) {
		dmsg;
		return ret;
	}
	if (setPt->compare && flagUnique) {
		mcr_String *found;
		dassert(copyStr);
		for (size_t i = 0; i < count; i++) {
			if (!(found = mcr_StringSet_find(setPt, copyStr[i]))) {
				if ((ret = mcr_StringSet_set(setPt, setPt->used,
							copyStr[i]))) {
					dmsg;
					return ret;
				}
				MCR_STRINGSET_SORT(*setPt);
			}
		}
		return 0;
	}
	if ((ret = mcr_StringSet_append(setPt, copyStr, count, flagUnique))) {
		dmsg;
		return ret;
	}
	MCR_STRINGSET_SORT(*setPt);
	return 0;
}

void mcr_StringSet_remove(mcr_StringSet * setPt, const char *removeStr)
{
	dassert(setPt);
	dassert(removeStr);
	mcr_String *found;
	while ((found = mcr_StringSet_find(setPt, removeStr)))
		mcr_StringSet_remove_index(setPt, MCR_STRINGSET_INDEXOF(*setPt,
				found), 1);
}
