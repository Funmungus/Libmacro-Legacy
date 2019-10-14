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

static const struct mcr_Interface _MCR_IARRAY_REF = {
	.id = ~0,
	.data_size = sizeof(struct mcr_Array),
	.init = mcr_Array_init_ref,
	.deinit = mcr_Array_deinit
};

static void mcr_Array_sort_memcmp(struct mcr_Array *arrPt);
static void *mcr_Array_find_memcmp(const struct mcr_Array *arrPt,
								   const void *elementPt);

const struct mcr_Interface *mcr_Array_ref_interface()
{
	return &_MCR_IARRAY_REF;
}

int mcr_Array_init(void *arrPt)
{
	struct mcr_Array *localPt = arrPt;
	if (localPt) {
		memset(localPt, 0, sizeof(struct mcr_Array));
		localPt->element_size = 1;
	}
	return 0;
}

struct mcr_Array mcr_Array_new(mcr_compare_fnc compare, size_t elementSize)
{
	struct mcr_Array ret;
	memset(&ret, 0, sizeof(struct mcr_Array));
	ret.element_size = elementSize;
	if (compare)
		ret.compare = compare;
	return ret;
}

int mcr_Array_init_ref(void *arrPt)
{
	if (arrPt) {
		mcr_Array_init(arrPt);
		mcr_Array_set_all(arrPt, mcr_ref_compare, sizeof(void *));
	}
	return 0;
}

int mcr_Array_deinit(void *arrPt)
{
	struct mcr_Array *localPt = arrPt;
	if (arrPt) {
		if (localPt->array) {
			free(localPt->array);
			localPt->array = NULL;
		}
		localPt->used = localPt->size = 0;
	}
	return 0;
}

void mcr_Array_set_all(struct mcr_Array *arrPt, mcr_compare_fnc compare,
					   size_t elementSize)
{
	dassert(arrPt);
	if (arrPt->array)
		mcr_Array_deinit(arrPt);
	arrPt->compare = compare;
	arrPt->element_size = elementSize;
}

/* Allocation control */
int mcr_Array_minused(struct mcr_Array *arrPt, size_t minUsed)
{
	size_t prevUsed = arrPt->used;
	int err;
	dassert(arrPt);
	if (prevUsed < minUsed) {
		if ((err = mcr_Array_resize(arrPt, minUsed)))
			return err;
		arrPt->used = minUsed;
	}
	return 0;
}

int mcr_Array_minsize(struct mcr_Array *arrPt, size_t minSize)
{
	dassert(arrPt);
	if (arrPt->size < minSize)
		return mcr_Array_resize(arrPt, minSize);
	return 0;
}

int mcr_Array_smartsize(struct mcr_Array *arrPt, size_t increasingCount)
{
	dassert(arrPt);
	if (!increasingCount)
		return 0;
	if (increasingCount == 1) {
		/* Double size if no space left */
		if (arrPt->used == arrPt->size)
			return mcr_Array_resize(arrPt, arrPt->size ?
									arrPt->size << 1 : 8);
	}
	/* New size = increasing + current used */
	increasingCount += arrPt->used;
	/* Resize to new value if we do not already have space */
	if (increasingCount > arrPt->size)
		return mcr_Array_resize(arrPt, increasingCount);
	return 0;
}

int mcr_Array_minfill(struct mcr_Array *arrPt, size_t minUsed,
					  const void *fillerElementPt)
{
	size_t prevUsed = arrPt->used;
	int err;
	dassert(arrPt);
	if (prevUsed < minUsed) {
		if ((err = mcr_Array_resize(arrPt, minUsed)))
			return err;
		arrPt->used = minUsed;
		return mcr_Array_fill(arrPt, prevUsed, fillerElementPt,
							  minUsed - prevUsed);
	}
	return 0;
}

void mcr_Array_trim(struct mcr_Array *arrPt)
{
	dassert(arrPt);
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
		mcr_Array_deinit(arrPt);
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
	dassert(arrPt);
	arrPt->used = 0;
}

/* Position */
void *mcr_Array_element(const struct mcr_Array *arrPt, size_t pos)
{
	/* pos >= used is out of bounds */
	if (arrPt && pos < arrPt->used)
		return MCR_ARR_ELEMENT(*arrPt, pos);
	return NULL;
}

void *mcr_Array_first(const struct mcr_Array *arrPt)
{
	return arrPt ? MCR_ARR_FIRST(*arrPt) : NULL;
}

void *mcr_Array_last(const struct mcr_Array *arrPt)
{
	return arrPt ? MCR_ARR_LAST(*arrPt) : NULL;
}

void *mcr_Array_end(const struct mcr_Array *arrPt)
{
	return arrPt ? MCR_ARR_END(*arrPt) : NULL;
}

void *mcr_Array_next(const struct mcr_Array *arrPt, void *posPt)
{
	/* If last, next is out of bounds */
	if (arrPt && posPt < mcr_Array_last(arrPt))
		return MCR_ARR_NEXT(*arrPt, posPt);
	return NULL;
}

void *mcr_Array_prev(const struct mcr_Array *arrPt, void *posPt)
{
	/* If first, previous is out of bounds */
	if (arrPt && posPt > (const void *)arrPt->array)
		return MCR_ARR_PREV(*arrPt, posPt);
	return NULL;
}

size_t mcr_Array_index(const struct mcr_Array * arrPt, const void *elPt)
{
	/* element - array = offset, offset / element size will be the index */
	if (arrPt && (const char *)elPt >= arrPt->array)
		return MCR_ARR_INDEX(*arrPt, elPt);
	return -1;
}

size_t mcr_Array_last_index(const struct mcr_Array * arrPt)
{
	return arrPt ? arrPt->used - 1 : (size_t) - 1;
}

void mcr_Array_iter(const struct mcr_Array *arrPt, char **iterPt,
					char **endPt, size_t * bytesPt)
{
	if (iterPt)
		*iterPt = mcr_Array_first(arrPt);
	if (endPt)
		*endPt = mcr_Array_end(arrPt);
	if (bytesPt)
		*bytesPt = arrPt ? arrPt->element_size : 0;
}

void mcr_Array_iter_range(const struct mcr_Array *arrPt, char **iterPt,
						  char **lastPt, size_t * bytesPt, size_t firstIndex, size_t lastIndex)
{
	if (iterPt)
		*iterPt = mcr_Array_element(arrPt, firstIndex);
	if (lastPt)
		*lastPt = mcr_Array_element(arrPt, lastIndex);
	if (bytesPt)
		*bytesPt = arrPt ? arrPt->element_size : 0;
}

/* Add/remove */
int mcr_Array_insert(struct mcr_Array *arrPt, size_t pos,
					 const void *elementArr, size_t count)
{
	size_t prevUsed = arrPt->used;
	int err;
	dassert(arrPt);
	if (!count)
		return 0;
	if (pos > prevUsed) {
		mset_error(EFAULT);
		return EFAULT;
	}
	/* Insert in-between, need to move previous items */
	if (pos < prevUsed) {
		if ((err = mcr_Array_smartsize(arrPt, count)))
			return err;
		/* Avoid fillers during move */
		arrPt->used += count;
		if ((err = mcr_Array_move(arrPt, pos + count, arrPt, pos,
								  prevUsed - pos)))
			return err;
	}
	/* Copy inserting elements */
	return mcr_Array_copy(arrPt, pos, elementArr, count);
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
					 size_t count)
{
	int err;
	dassert(arrPt);
	if (!count)
		return 0;
	if ((err = mcr_Array_smartsize(arrPt, count)))
		return err;
	dassert(arrPt->size >= arrPt->used + count);
	return mcr_Array_copy(arrPt, arrPt->used, elementArr, count);
}

int mcr_Array_push(struct mcr_Array *arrPt, const void *elementPt)
{
	return mcr_Array_append(arrPt, elementPt, 1);
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
	int err;
	dassert(arrPt);
	if (!count) {
		mcr_Array_deinit(arrPt);
		return 0;
	}
	if ((err = mcr_Array_resize(arrPt, count)))
		return err;
	return mcr_Array_copy(arrPt, 0, arraySource, count);
}

int mcr_Array_copy(struct mcr_Array *dstPt, size_t dstPos,
				   const void *srcArray, size_t count)
{
	int err;
	void *copyDst;
	dassert(dstPt);
	if (!count)
		return 0;
	if (dstPos > dstPt->used) {
		mset_error(EFAULT);
		return EFAULT;
	}
	if ((err = mcr_Array_minused(dstPt, dstPos + count)))
		return err;
	copyDst = mcr_Array_element(dstPt, dstPos);
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

int mcr_Array_fill(struct mcr_Array *arrPt, size_t dstPos,
				   const void *copyElementPt, size_t count)
{
	int err;
	char *itPt, *endPt;
	size_t bytes, diff;
	dassert(arrPt);
	bytes = arrPt->element_size;
	/* If creating empty elements, fill them also. */
	if (dstPos > arrPt->used) {
		diff = dstPos - arrPt->used;
		dstPos -= diff;
		count += diff;
	}
	if ((err = mcr_Array_minused(arrPt, dstPos + count)))
		return err;
	itPt = mcr_Array_element(arrPt, dstPos);
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
	int err;
	void *copyDst, *copySrc;
	dassert(dstPt);
	dassert(srcPt);
	dassert(dstPt->element_size == srcPt->element_size);
	/* Limit to copying/moving existing items. */
	if (!count)
		return 0;
	if (srcPos >= srcPt->used) {
		mset_error(EFAULT);
		return EFAULT;
	}
	if (count > srcPt->used || srcPos + count > srcPt->used)
		count = srcPt->used - srcPos;
	if (dstPos > dstPt->used) {
		if ((err = mcr_Array_fill(dstPt, dstPt->used, NULL,
								  dstPos - dstPt->used)))
			return err;
	}
	if ((err = mcr_Array_minused(dstPt, dstPos + count)))
		return err;
	copyDst = mcr_Array_element(dstPt, dstPos);
	copySrc = mcr_Array_element(srcPt, srcPos);
	dassert(copyDst);
	dassert(copySrc);
	if (dstPt->array == srcPt->array)
		memmove(copyDst, copySrc, dstPt->element_size * count);
	else
		memcpy(copyDst, copySrc, dstPt->element_size * count);
	return 0;
}

/* Sorted functions */
void mcr_Array_sort(struct mcr_Array *arrPt)
{
	dassert(arrPt);
	/* No need to sort 0 - 1 elements */
	if (arrPt->used > 1) {
		if (arrPt->compare)
			qsort(arrPt->array, arrPt->used, arrPt->element_size,
				  arrPt->compare);
		else
			mcr_Array_sort_memcmp(arrPt);
	}
}

void *mcr_Array_find(const struct mcr_Array *arrPt, const void *elementPt)
{
	void *ret = NULL, *elHolder;
	/* No need to sort 0 - 1 elements */
	if (!arrPt)
		return NULL;
	if (elementPt) {
		elHolder = (void *)elementPt;
	} else {
		if (!(elHolder = malloc(arrPt->element_size))) {
			mset_error(ENOMEM);
			return NULL;
		}
		memset(elHolder, 0, arrPt->element_size);
	}
	if (arrPt->compare)
		ret = bsearch(elHolder, arrPt->array, arrPt->used,
					  arrPt->element_size, arrPt->compare);
	else
		ret = mcr_Array_find_memcmp(arrPt, elHolder);
	if (!elementPt)
		free(elHolder);
	return ret;
}

int mcr_Array_add(struct mcr_Array *arrPt,
				  const void *elementArr, size_t count, bool flagUnique)
{
	int err;
	char *itPt;
	size_t bytes = arrPt->element_size;
	dassert(arrPt);
	if (!count)
		return 0;
	if ((err = mcr_Array_smartsize(arrPt, count)))
		return err;
	if (flagUnique) {
		/* Unique but no array, only one empty item is possible to
		 * add */
		if (!elementArr) {
			if (!mcr_Array_find(arrPt, NULL)) {
				mcr_Array_push(arrPt, NULL);
				mcr_Array_sort(arrPt);
			}
			return 0;
		}
		itPt = (char *)elementArr;
		while (count--) {
			if (!mcr_Array_find(arrPt, itPt)) {
				if ((err = mcr_Array_push(arrPt, itPt)))
					return err;
				mcr_Array_sort(arrPt);
			}
			itPt += bytes;
		}
		return 0;
	}
	/* Not unique, just throw it in and sort it out */
	if ((err = mcr_Array_append(arrPt, elementArr, count)))
		return err;
	mcr_Array_sort(arrPt);
	return 0;
}

void mcr_Array_remove(struct mcr_Array *arrPt, const void *removeElementPt)
{
	void *found;
	dassert(arrPt);
	dassert(removeElementPt);
	/* Loop in case elements are not unique */
	while ((found = mcr_Array_find(arrPt, removeElementPt)))
		mcr_Array_remove_index(arrPt, mcr_Array_index(arrPt, found), 1);
}

static void mcr_Array_sort_memcmp(struct mcr_Array *arrPt)
{
	bool hasSwap = true;
	char *firstPt, *prevPt, *ritPt, *elHolder;
	size_t bytes = arrPt->element_size;
	dassert(arrPt);
	dassert(arrPt->used > 1);
	if (!(elHolder = malloc(arrPt->element_size))) {
		mset_error(ENOMEM);
		return;
	}
	/// \todo quicksort
	/* Bubble sort backwards to optimize sorted arrays with new values */
	while (hasSwap) {
		hasSwap = false;
		/* Bytes set above */
		firstPt = MCR_ARR_FIRST(*arrPt);
		ritPt = MCR_ARR_LAST(*arrPt);
		prevPt = MCR_ARR_PREV(*arrPt, ritPt);
		while (prevPt >= firstPt) {
			/* Sorted pairs will always be prev < current */
			/* If prev > current, then swap prev and current */
			if (memcmp(prevPt, ritPt, bytes) > 0) {
				hasSwap = true;
				memcpy(elHolder, prevPt, bytes);
				memcpy(prevPt, ritPt, bytes);
				memcpy(ritPt, elHolder, bytes);
			}
			/* Move to next pair */
			ritPt -= bytes;
			prevPt -= bytes;
		}
	}
	free(elHolder);
}

static void *mcr_Array_find_memcmp(const struct mcr_Array *arrPt,
								   const void *elementPt)
{
	char *itPt, *endPt;
	size_t bytes;
	dassert(arrPt);
	dassert(elementPt);
	fixme;
	/// \todo quicksort
	for (mcr_Array_iter(arrPt, &itPt, &endPt, &bytes);
		 itPt < endPt; itPt += bytes) {
		if (!memcmp(itPt, elementPt, bytes))
			return itPt;
	}
	return NULL;
}
