/* Libmacro - A multi-platform, extendable macro and hotkey C library
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void mcr_StringSet_deinit_range(mcr_StringSet * setPt, size_t firstIndex,
									   size_t lastIndex);

int mcr_StringSet_init(void *setPt)
{
	mcr_StringSet *localPt = setPt;
	if (setPt) {
		mcr_Array_init(setPt);
		localPt->element_size = sizeof(mcr_String);
		localPt->compare = mcr_String_compare;
	}
	return 0;
}

mcr_StringSet mcr_StringSet_new()
{
	mcr_StringSet ret;
	mcr_StringSet_init(&ret);
	return ret;
}

int mcr_StringSet_deinit(void *setPt)
{
	mcr_StringSet *localPt = setPt;
	if (setPt) {
		MCR_STRINGSET_FOR_EACH(*localPt, mcr_String_deinit);
		mcr_Array_deinit(setPt);
	}
	return 0;
}

void mcr_StringSet_set_all(mcr_StringSet * setPt, mcr_compare_fnc compare)
{
	dassert(setPt);
	if (setPt->size)
		mcr_StringSet_deinit(setPt);
	setPt->compare = compare;
}

int mcr_StringSet_minused(mcr_StringSet * setPt, size_t minUsed)
{
	mcr_String initial;
	dassert(setPt);
	if (setPt->used < minUsed) {
		mcr_String_init(&initial);
		return mcr_Array_minfill(setPt, minUsed, &initial);
	}
	return 0;
}

int mcr_StringSet_resize(mcr_StringSet * setPt, size_t newSize)
{
	dassert(setPt);
	if (newSize < setPt->used) {
		mcr_StringSet_deinit_range(setPt, newSize, setPt->used - 1);
		return mcr_Array_resize(setPt, newSize);
	}
	return 0;
}

void mcr_StringSet_clear(mcr_StringSet * setPt)
{
	dassert(setPt);
	MCR_STRINGSET_FOR_EACH(*setPt, mcr_String_deinit);
	setPt->used = 0;
}

int mcr_StringSet_insert(mcr_StringSet * setPt, size_t pos,
						 const char **strArr, size_t count)
{
	size_t i;
	size_t prevUsed = setPt->used;
	int err;
	mcr_String initial;
	dassert(setPt);
	if (!count)
		return 0;
	if (pos > prevUsed) {
		mset_error(EFAULT);
		return EFAULT;
	}
	/* Insert between, insert with all 0's, then fill with initial string */
	mcr_String_init(&initial);
	if ((err = mcr_Array_insert(setPt, pos, NULL, count)) ||
		(err = mcr_Array_fill(setPt, pos, &initial, count)))
		return err;
	/* String deinit, not needed */
	/* Starting at pos, replace all strings from strArr */
	if (strArr) {
		i = count;
		while (i--) {
			if ((err = mcr_String_replace(MCR_STRINGSET_ELEMENT
										  (*setPt, pos + i), strArr[i])))
				return err;
		}
	}
	return 0;
}

void mcr_StringSet_remove_index(mcr_StringSet * setPt, size_t pos, size_t count)
{
	dassert(setPt);
	/* Limit to removing existing set of elements */
	if (!count || pos >= setPt->used)
		return;
	if (count > setPt->used || pos + count >= setPt->used)
		count = setPt->used - pos;
	mcr_StringSet_deinit_range(setPt, pos, pos + count - 1);
	mcr_Array_remove_index(setPt, pos, count);
}

int mcr_StringSet_append(mcr_StringSet * setPt, const char **strArr,
						 size_t count)
{
	return mcr_StringSet_copy(setPt, setPt->used, strArr, count);
}

int mcr_StringSet_push(mcr_StringSet * setPt, const char *copyStr)
{
	return mcr_StringSet_append(setPt, copyStr ? &copyStr : NULL, 1);
}

void mcr_StringSet_pop(mcr_StringSet * setPt)
{
	dassert(setPt);
	if (setPt->used) {
		mcr_String_deinit(mcr_StringSet_last(setPt));
		--setPt->used;
	}
}

int mcr_StringSet_replace(mcr_StringSet * setPt,
						  const char **arraySource, size_t count)
{
	int err;
	dassert(setPt);
	if (!count) {
		mcr_StringSet_deinit(setPt);
		return 0;
	}
	/* Frees if reducing */
	if ((err = mcr_StringSet_resize(setPt, count)))
		return err;
	/* Initializes new elements if increasing */
	return mcr_StringSet_copy(setPt, 0, arraySource, count);
}

int mcr_StringSet_copy(mcr_StringSet * dstPt, size_t dstPos,
					   const char **srcArray, size_t count)
{
	size_t i;
	int err;
	dassert(dstPt);
	if (!count)
		return 0;
	if ((err = mcr_StringSet_minused(dstPt, dstPos + count)))
		return err;
	if (srcArray) {
		i = count;
		while (i--) {
			if ((err = mcr_String_replace(MCR_STRINGSET_ELEMENT
										  (*dstPt, dstPos + i),
										  srcArray[i])))
				return err;
		}
	} else {
		mcr_StringSet_deinit_range(dstPt, dstPos, dstPos + count - 1);
	}
	return 0;
}

int mcr_StringSet_set(mcr_StringSet * setPt, size_t pos, const char *copyStr)
{
	int err = mcr_StringSet_minused(setPt, pos);
	dassert(setPt);
	dassert(pos <= setPt->used);
	if (err)
		return err;
	if (copyStr)
		return mcr_String_replace(MCR_STRINGSET_ELEMENT(*setPt, pos),
								  copyStr);
	mcr_String_deinit(MCR_STRINGSET_ELEMENT(*setPt, pos));
	return 0;
}

int mcr_StringSet_fill(mcr_StringSet * setPt, size_t pos,
					   size_t count, const char *copyStr)
{
	int err;
	char *itPt, *end;
	size_t bytes;
	dassert(setPt);
	if ((err = mcr_StringSet_minused(setPt, pos + count)))
		return err;
	mcr_Array_iter_range(setPt, &itPt, &end, &bytes, pos, pos + count);
	while (itPt < end) {
		if ((err = mcr_String_replace((mcr_String *) itPt, copyStr)))
			return err;
		itPt += bytes;
	}
	return 0;
}

void mcr_StringSet_sort(mcr_StringSet * setPt)
{
	dassert(setPt);
	if (setPt->used > 1) {
		qsort(setPt->array, setPt->used, setPt->element_size,
			  setPt->compare ? setPt->compare : mcr_String_compare);
	}
}

mcr_String *mcr_StringSet_find(const mcr_StringSet * setPt, const char *strKey)
{
	if (!setPt)
		return NULL;
	/* Default empty string to search */
	if (!strKey)
		strKey = "";
	return bsearch(&strKey, setPt->array, setPt->used, setPt->element_size,
				   setPt->compare ? setPt->compare : mcr_String_compare);
}

int mcr_StringSet_add(mcr_StringSet * setPt,
					  const char **copyStr, size_t count, bool flagUnique)
{
	int err;
	size_t i;
	mcr_String *found;
	dassert(setPt);
	if (!count)
		return 0;
	if ((err = mcr_StringSet_smartsize(setPt, count)))
		return err;
	if (flagUnique) {
		dassert(copyStr);
		for (i = 0; i < count; i++) {
			if (!(found = mcr_StringSet_find(setPt, copyStr[i]))) {
				if ((err = mcr_StringSet_set(setPt, setPt->used,
											 copyStr[i])))
					return err;
				mcr_StringSet_sort(setPt);
			}
		}
		return 0;
	}
	if ((err = mcr_StringSet_append(setPt, copyStr, count)))
		return err;
	mcr_StringSet_sort(setPt);
	return 0;
}

void mcr_StringSet_remove(mcr_StringSet * setPt, const char *removeStr)
{
	mcr_String *found;
	dassert(setPt);
	dassert(removeStr);
	while ((found = mcr_StringSet_find(setPt, removeStr)))
		mcr_StringSet_remove_index(setPt, mcr_StringSet_index(setPt,
								   found), 1);
}

static void mcr_StringSet_deinit_range(mcr_StringSet * setPt, size_t firstIndex,
									   size_t lastIndex)
{
	char *itPt, *lastPt;
	size_t bytes;
	mcr_Array_iter_range(setPt, &itPt, &lastPt, &bytes, firstIndex,
						 lastIndex);
	if (itPt && lastPt) {
		while (itPt <= lastPt) {
			mcr_String_deinit((void *)itPt);
			itPt += bytes;
		}
	}
}
