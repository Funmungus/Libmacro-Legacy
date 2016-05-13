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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const struct mcr_Interface _MCR_IARRAY_REF = {
	.id = ~0,
	.data_size = sizeof(struct mcr_Array),
	.init = mcr_Array_init_ref,
	.free = mcr_Array_free
};

const struct mcr_Interface *mcr_Array_ref_iface()
{
	return &_MCR_IARRAY_REF;
}

void mcr_Array_init(void *dataPt)
{
	struct mcr_Array *arrPt = dataPt;
	if (!dataPt)
		return;
	memset(dataPt, 0, sizeof(struct mcr_Array));
	arrPt->element_size = 1;
}

void mcr_Array_init_ref(void *data)
{
	if (!data)
		return;
	mcr_Array_init(data);
	mcr_Array_set_all(data, mcr_ref_compare, sizeof(void *));
}

void mcr_Array_free(void *dataPt)
{
	struct mcr_Array *arrPt = dataPt;
	if (!dataPt)
		return;
	if (arrPt->array) {
		free(arrPt->array);
		arrPt->array = NULL;
	}
	arrPt->used = arrPt->size = 0;
}

void mcr_Array_set_all(struct mcr_Array *arrPt, mcr_compare_fnc compare,
	size_t elementSize)
{
	dassert(arrPt);
	if (arrPt->size)
		mcr_Array_free(arrPt);
	arrPt->compare = compare;
	arrPt->element_size = elementSize;
}

/* Allocation control */
int mcr_Array_minused(struct mcr_Array *arrPt, size_t minimumUsed)
{
	size_t prevUsed = arrPt->used;
	int ret;
	dassert(arrPt);
	if (prevUsed < minimumUsed) {
		if ((ret = mcr_Array_resize(arrPt, minimumUsed)))
			return ret;
		arrPt->used = minimumUsed;
	}
	return 0;
}

int mcr_Array_minsize(struct mcr_Array *arrPt, size_t minimumSize)
{
	dassert(arrPt);
	if (arrPt->size < minimumSize)
		return mcr_Array_resize(arrPt, minimumSize);
	return 0;
}

int mcr_Array_smartsize(struct mcr_Array *arrPt, size_t increasingCount)
{
	int ret;
	dassert(arrPt);
	if (increasingCount == 1) {
		if (arrPt->used == arrPt->size) {
			if ((ret = mcr_Array_resize(arrPt, arrPt->size ?
						arrPt->size << 1 : 8)))
				return ret;
		}
	} else if (increasingCount) {
		if ((ret = mcr_Array_resize(arrPt,
					arrPt->used + increasingCount)))
			return ret;
	}
	return 0;
}

int mcr_Array_minfill(struct mcr_Array *arrPt, size_t minimumUsed,
	const void *fillerElementPt)
{
	size_t prevUsed = arrPt->used;
	int ret;
	dassert(arrPt);
	if (prevUsed < minimumUsed) {
		if ((ret = mcr_Array_resize(arrPt, minimumUsed)))
			return ret;
		arrPt->used = minimumUsed;
		return mcr_Array_fill(arrPt, prevUsed, minimumUsed - prevUsed,
			fillerElementPt);
	}
	return 0;
}

void mcr_Array_trim(struct mcr_Array *arrPt)
{
	if (arrPt)
		mcr_Array_resize(arrPt, arrPt->used);
}

int mcr_Array_resize(struct mcr_Array *arrPt, size_t newSize)
{
	void *newData;
	dassert(arrPt);
	/* No change, do nothing. */
	if (arrPt->size == newSize || !arrPt->element_size)
		return 0;
	if (!newSize) {
		mcr_Array_free(arrPt);
		return 0;
	}
	/* Free removed elements */
	if (newSize < arrPt->used)
		arrPt->used = newSize;
	newData = realloc(arrPt->array, newSize * arrPt->element_size);
	if (!newData) {
		mset_error(ENOMEM);
		return ENOMEM;
	}
	arrPt->array = newData;
	arrPt->size = newSize;
	return 0;
}

void mcr_Array_clear(struct mcr_Array *arrPt)
{
	if (arrPt)
		arrPt->used = 0;
}

/* Position */
void *mcr_Array_element(const struct mcr_Array *arrPt, size_t pos)
{
	dassert(arrPt);
	/* Not allocated or position invalid */
	if (pos >= arrPt->used) {
		mset_error(EFAULT);
		return NULL;
	}
	return MCR_ARR_ELEMENT(*arrPt, pos);
}

void *mcr_Array_next(const struct mcr_Array *arrPt, const void *posPt)
{
	dassert(arrPt);
	return posPt < MCR_ARR_LAST(*arrPt) ? MCR_ARR_NEXT(*arrPt,
		posPt) : NULL;
}

void *mcr_Array_prev(const struct mcr_Array *arrPt, const void *posPt)
{
	dassert(arrPt);
	return posPt > (const void *)arrPt->array ? MCR_ARR_PREV(*arrPt,
		posPt) : NULL;
}

void *mcr_Array_end(const struct mcr_Array *arrPt)
{
	dassert(arrPt);
	return arrPt ? MCR_ARR_END(*arrPt) : NULL;
}

void *mcr_Array_first(const struct mcr_Array *arrPt)
{
	return arrPt ? MCR_ARR_FIRST(*arrPt) : NULL;
}

void *mcr_Array_last(const struct mcr_Array *arrPt)
{
	return arrPt ? MCR_ARR_LAST(*arrPt) : NULL;
}

size_t mcr_Array_last_index(const struct mcr_Array * arrPt)
{
	return arrPt ? arrPt->used - 1 : (size_t) ~ 0;
}

size_t mcr_Array_index(const struct mcr_Array * arrPt, const void *elPt)
{
	dassert(arrPt);
	return MCR_ARR_INDEX(*arrPt, elPt);
}

/* Add/remove */
int mcr_Array_insert(struct mcr_Array *arrPt, size_t pos,
	const void *elementArr, size_t count)
{
	size_t prevUsed = arrPt->used;
	int ret;
	dassert(arrPt);
	if (!count)
		return 0;
	/* Insert after used, fill empty space */
	if (pos > prevUsed) {
		mset_error(EFAULT);
		return -EFAULT;
	}
	/* Insert in-between, need to move previous items */
	if (pos < prevUsed) {
		if ((ret = mcr_Array_smartsize(arrPt, count)))
			return ret;
		/* Avoid fillers during move */
		arrPt->used += count;
		if ((ret = !mcr_Array_move(arrPt, pos + count, arrPt, pos,
					prevUsed - pos)))
			return ret;
	}
	/* Copy inserting elements */
	return mcr_Array_copy(arrPt, pos, elementArr, count);
}

int mcr_Array_insert_filled(struct mcr_Array *arrPt, size_t pos,
	const void *elementArr, size_t count, const void *fillerPt)
{
	int ret = mcr_Array_minsize(arrPt, pos + count);
	if (!ret)
		ret = mcr_Array_minfill(arrPt, pos, fillerPt);
	if (!ret)
		return mcr_Array_insert(arrPt, pos, elementArr, count);
	return ret;
}

void mcr_Array_remove_index(struct mcr_Array *arrPt, size_t pos, size_t count)
{
	size_t prevUsed = arrPt->used, movePos, moveCount;
	dassert(arrPt);
	/* Limit to removing existing set of elements */
	if (!count || pos >= prevUsed)
		return;
	/* Nothing to move back if removing up to end */
	if (count > prevUsed || pos + count >= prevUsed)
		count = prevUsed - pos;
	movePos = pos + count;
	moveCount = prevUsed - movePos;
	if (moveCount)
		mcr_Array_move(arrPt, pos, arrPt, movePos, moveCount);
	arrPt->used -= count;
}

int mcr_Array_append(struct mcr_Array *arrPt, const void *elementArr,
	size_t len, bool flagUnique)
{
	int ret;
	char *it = (char *)elementArr;
	void *found;
	dassert(arrPt);
	if (!len)
		return 0;
	if ((ret = mcr_Array_smartsize(arrPt, len)))
		return ret;
	dassert(arrPt->size >= arrPt->used + len);
	if (flagUnique) {
		dassert(elementArr);
		while (len) {
			if (!(found = mcr_Array_find(arrPt, it))) {
				if ((ret = mcr_Array_set(arrPt, arrPt->used,
							it)))
					return ret;
			}
			if (arrPt->compare)
				mcr_Array_sort(arrPt);
			it += arrPt->element_size;
			--len;
		}
	} else {
		return mcr_Array_copy(arrPt, arrPt->used, elementArr, len);
	}
	return 0;
}

int mcr_Array_push(struct mcr_Array *arrPt, const void *elementPt,
	bool flagUnique)
{
	return mcr_Array_append(arrPt, elementPt, 1, flagUnique);
}

void mcr_Array_pop(struct mcr_Array *arrPt)
{
	if (arrPt && arrPt->used)
		--arrPt->used;
}

/* Replace current elements */
int mcr_Array_replace(struct mcr_Array *arrPt, const void *arraySource,
	size_t count)
{
	int ret;
	dassert(arrPt);
	if (!count) {
		mcr_Array_free(arrPt);
		return 0;
	}
	if ((ret = mcr_Array_resize(arrPt, count)))
		return ret;
	return mcr_Array_copy(arrPt, 0, arraySource, count);
}

int mcr_Array_copy(struct mcr_Array *dstPt, size_t dstPos,
	const void *srcArray, size_t count)
{
	int ret;
	void *copyDst;
	dassert(dstPt);
	if (!count)
		return 0;
	if (dstPos > dstPt->used) {
		mset_error(EFAULT);
		return -EFAULT;
	}
	if ((ret = mcr_Array_minused(dstPt, dstPos + count)))
		return ret;
	copyDst = MCR_ARR_ELEMENT(*dstPt, dstPos);
	dassert(copyDst);
	if (srcArray)
		memcpy(copyDst, srcArray, dstPt->element_size * count);
	else
		memset(copyDst, 0, dstPt->element_size * count);
	return 0;
}

int mcr_Array_set(struct mcr_Array *arrPt, size_t pos, const void *elementPt)
{
	return mcr_Array_copy(arrPt, pos, elementPt, 1);
}

int mcr_Array_fill(struct mcr_Array *arrPt, size_t pos,
	size_t count, const void *copyElementPt)
{
	int ret;
	char *itPt, *endPt;
	size_t bytes, diff;
	dassert(arrPt);
	bytes = arrPt->element_size;
	/* If creating empty elements, fill them also. */
	if (pos > arrPt->used) {
		diff = pos - arrPt->used;
		pos -= diff;
		count += diff;
	}
	if ((ret = mcr_Array_minused(arrPt, pos + count)))
		return ret;
	itPt = MCR_ARR_ELEMENT(*arrPt, pos);
	dassert(itPt);
	if (copyElementPt) {
		endPt = MCR_ARR_END(*arrPt);
		while (itPt < endPt) {
			memcpy(itPt, copyElementPt, bytes);
			itPt += bytes;
		}
	} else {
		memset(itPt, 0, arrPt->element_size * count);
	}
	return 0;
}

int mcr_Array_move(struct mcr_Array *dstPt, size_t dstPos,
	const struct mcr_Array *srcPt, size_t srcPos, size_t count)
{
	int ret;
	void *copyDst, *copySrc;
	dassert(dstPt);
	dassert(srcPt);
	dassert(dstPt->element_size == srcPt->element_size);
	/* Limit to copying/moving existing items. */
	if (!count)
		return 0;
	if (srcPos >= srcPt->used) {
		mset_error(EFAULT);
		return -EFAULT;
	}
	if (count > srcPt->used || srcPos + count > srcPt->used)
		count = srcPt->used - srcPos;
	if (dstPos > dstPt->used) {
		if ((ret = mcr_Array_fill(dstPt, dstPt->used,
					dstPos - dstPt->used, NULL)))
			return ret;
	}
	if ((ret = mcr_Array_minused(dstPt, dstPos + count)))
		return ret;
	copyDst = MCR_ARR_ELEMENT(*dstPt, dstPos);
	copySrc = MCR_ARR_ELEMENT(*srcPt, srcPos);
	dassert(copyDst);
	dassert(copySrc);
	if (dstPt->array == srcPt->array)
		memmove(copyDst, copySrc, dstPt->element_size * count);
	else
		memcpy(copyDst, copySrc, dstPt->element_size * count);
	return 0;
}

/* */
/* Sortable functions: If no compare function available, memcmp will be */
/* used on an unsorted array, with a slow iteration. */
/* */
void mcr_Array_sort(struct mcr_Array *arrPt)
{
	if (!arrPt)
		return;
	if (arrPt->compare) {
		MCR_ARR_SORT(*arrPt);
	}
	/* TODO memcmp sort */
}

void *mcr_Array_find(const struct mcr_Array *arrPt, const void *elementPt)
{
	char *it, *end;
	dassert(arrPt);
	size_t bytes = arrPt->element_size;
	dassert(elementPt);
	if (arrPt->compare)
		return MCR_ARR_FIND(*arrPt, elementPt);
	for (it = MCR_ARR_FIRST(*arrPt), end = MCR_ARR_END(*arrPt); it < end;
		it += bytes) {
		if (!memcmp(it, elementPt, bytes))
			return it;
	}
	return NULL;
}

int mcr_Array_add(struct mcr_Array *arrPt,
	const void *elementArr, size_t len, bool flagUnique)
{
	int ret;
	char *it;
	dassert(arrPt);
	size_t bytes = arrPt->element_size;
	if (!len)
		return 0;
	if ((ret = mcr_Array_smartsize(arrPt, len)))
		return ret;
	/* Comparable and unique, add elements only if they do not exist */
	if (arrPt->compare && flagUnique) {
		dassert(elementArr);
		it = (char *)elementArr;
		while (len) {
			if (!bsearch(it, arrPt->array, arrPt->used,
					bytes, arrPt->compare)) {
				if ((ret = mcr_Array_set(arrPt, arrPt->used,
							it)))
					return ret;
				MCR_ARR_SORT(*arrPt);
			}
			it += bytes;
			--len;
		}
		return 0;
	}
	/* Not unique, just throw it in and sort it out */
	if ((ret = mcr_Array_append(arrPt, elementArr, len, flagUnique)))
		return ret;
	MCR_ARR_SORT(*arrPt);
	return 0;
}

void mcr_Array_remove(struct mcr_Array *arrPt, const void *removeElementPt)
{
	void *found;
	dassert(arrPt);
	dassert(removeElementPt);
	/* Loop in case elements are not unique */
	while ((found = mcr_Array_find(arrPt, removeElementPt)))
		mcr_Array_remove_index(arrPt, MCR_ARR_INDEX(*arrPt, found), 1);
}
