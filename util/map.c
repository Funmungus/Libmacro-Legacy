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
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void mcr_Map_key_init(const struct mcr_Map *mapPt, void *keyPt);
static void mcr_Map_value_init(const struct mcr_Map *mapPt, void *valuePt);
static void mcr_Map_key_free(const struct mcr_Map *mapPt, void *keyPt);
static void mcr_Map_value_free(const struct mcr_Map *mapPt, void *valuePt);
static void mcr_Map_init_range(struct mcr_Map *mapPt, size_t firstPos,
	size_t lastPos);
static void mcr_Map_free_range(struct mcr_Map *mapPt, size_t firstPos,
	size_t lastPos);
static int mcr_Map_key_copy(const struct mcr_Map *mapPt, void *keyPt,
	const void *copyKeyPt);
static int mcr_Map_value_copy(const struct mcr_Map *mapPt, void *valuePt,
	const void *copyValuePt);

#define mcr_Map_element_init(mapPt, elementPt) \
mcr_Map_key_init(mapPt, elementPt); \
mcr_Map_value_init(mapPt, MCR_MAP_VALUEOF(*(mapPt), elementPt));
#define mcr_Map_element_free(mapPt, elementPt) \
mcr_Map_key_free(mapPt, elementPt); \
mcr_Map_value_free(mapPt, MCR_MAP_VALUEOF(*(mapPt), elementPt));
#define mcr_Map_element_copy(mapPt, elementPt, copyKeyPt, copyValuePt, valOut) \
((valOut = mcr_Map_key_copy(mapPt, elementPt, copyKeyPt)) ? \
	valOut : \
mcr_Map_value_copy(mapPt, MCR_MAP_VALUEOF(*(mapPt), elementPt), \
		copyValuePt))

void mcr_Map_init(void *dataPt)
{
	dassert(dataPt);
	struct mcr_Map *mapPt = dataPt;
	memset(mapPt, 0, sizeof(struct mcr_Map));
	mapPt->key_size = 1;
	mapPt->value_size = 1;
	mcr_Array_init(&mapPt->set);
	mcr_Array_set_all(&mapPt->set, mcr_char_compare, 2);
}

void mcr_Map_free(void *dataPt)
{
	dassert(dataPt);
	struct mcr_Map *mapPt = dataPt;
	mcr_Map_free_range(mapPt, 0, mapPt->set.used - 1);
	mcr_Array_free(&mapPt->set);
}

void mcr_Map_set_all(struct mcr_Map *mapPt, size_t keySize, size_t valueSize,
	mcr_compare_fnc compare, const struct mcr_Interface *keyIface,
	const struct mcr_Interface *valueIface)
{
	dassert(mapPt);
	if (mapPt->set.size)
		mcr_Map_free(mapPt);
	/*
	 * Cannot map without both a comparison function, and elements to
	 * compare
	 */
	if (keyIface) {
		mapPt->key_iface = keyIface;
		if (!keySize)
			keySize = keyIface->data_size;
		if (!compare)
			compare = keyIface->compare;
	}
	if (valueIface) {
		mapPt->value_iface = valueIface;
		if (!valueSize)
			valueSize = valueIface->data_size;
	}
	dassert(keySize);
	dassert(valueSize);
	dassert(compare);
	mapPt->key_size = keySize;
	mapPt->value_size = valueSize;
	mcr_Array_set_all(&mapPt->set, compare, keySize + valueSize);
}

/* Allocation control */
int mcr_Map_minused(struct mcr_Map *mapPt, size_t minimumUsed)
{
	dassert(mapPt);
	size_t prevUsed = mapPt->set.used;
	int ret;
	if (prevUsed < minimumUsed) {
		if ((ret = mcr_Array_minused(&mapPt->set, minimumUsed))) {
			dmsg;
			return ret;
		}
		mcr_Map_init_range(mapPt, prevUsed, minimumUsed - 1);
	}
	return 0;
}

int mcr_Map_minsize(struct mcr_Map *mapPt, size_t minimumSize)
{
	dassert(mapPt);
	return mcr_Array_minsize(&mapPt->set, minimumSize);
}

int mcr_Map_smartsize(struct mcr_Map *mapPt, size_t increasingCount)
{
	dassert(mapPt);
	return mcr_Array_smartsize(&mapPt->set, increasingCount);
}

void mcr_Map_trim(struct mcr_Map *mapPt)
{
	dassert(mapPt);
	mcr_Array_trim(&mapPt->set);
}

int mcr_Map_resize(struct mcr_Map *mapPt, size_t newSize)
{
	dassert(mapPt);
	if (newSize < mapPt->set.used)
		mcr_Map_free_range(mapPt, newSize, mapPt->set.used - 1);
	return mcr_Array_resize(&mapPt->set, newSize);
}

void mcr_Map_clear(struct mcr_Map *mapPt)
{
	dassert(mapPt);
	mcr_Map_free_range(mapPt, 0, mapPt->set.used - 1);
	mapPt->set.used = 0;
}

/* Position */
void *mcr_Map_valueof(const struct mcr_Map *mapPt, const void *pairPt)
{
	dassert(mapPt);
	return MCR_MAP_VALUEOF(*mapPt, pairPt);
}

int mcr_Map_set_key(const struct mcr_Map *mapPt, void *pairPt,
	const void *copyKeyPt)
{
	dassert(mapPt);
	dassert(pairPt);
	return mcr_Map_key_copy(mapPt, pairPt, copyKeyPt);
}

int mcr_Map_set_valueof(const struct mcr_Map *mapPt, void *pairPt,
	const void *copyValuePt)
{
	dassert(mapPt);
	dassert(pairPt);
	return mcr_Map_value_copy(mapPt, MCR_MAP_VALUEOF(*mapPt, pairPt),
		copyValuePt);
}

void *mcr_Map_element(const struct mcr_Map *mapPt, const void *keyPt)
{
	dassert(mapPt);
	return MCR_MAP_ELEMENT(*mapPt, keyPt);
}

void *mcr_Map_value(const struct mcr_Map *mapPt, const void *keyPt)
{
	dassert(mapPt);
	void *ret = MCR_MAP_ELEMENT(*mapPt, keyPt);
	return MCR_MAP_VALUEOF(*mapPt, ret);
}

void *mcr_Map_element_ensured(struct mcr_Map *mapPt, const void *keyPt)
{
	dassert(mapPt);
	dassert(keyPt);
	void *found = MCR_MAP_ELEMENT(*mapPt, keyPt);
	int ret;
	if (found)
		return found;
	if ((ret = mcr_Map_map(mapPt, keyPt, NULL))) {
		dmsg;
		errno = ret;
		return NULL;
	}
	return MCR_MAP_ELEMENT(*mapPt, keyPt);
}

size_t mcr_Map_index(const struct mcr_Map * mapPt, const void *keyPt)
{
	dassert(mapPt);
	void *found = MCR_MAP_ELEMENT(*mapPt, keyPt);
	return MCR_MAP_INDEXOF(*mapPt, found);
}

/* Set/remove mappings */
int mcr_Map_map(struct mcr_Map *mapPt, const void *keyPt, const void *valuePt)
{
	dassert(mapPt);
	dassert(keyPt);
	void *mapping = MCR_MAP_ELEMENT(*mapPt, keyPt);
	int ret;
	if (mapping) {
		mcr_Map_set_valueof(mapPt, mapping, valuePt);
		return 0;
	}
	if ((ret = mcr_Array_smartsize(&mapPt->set, 1))) {
		dmsg;
		return ret;
	}
	++mapPt->set.used;
	mapping = MCR_ARR_LAST(mapPt->set);
	mcr_Map_element_init(mapPt, mapping);
	if ((ret = mcr_Map_element_copy(mapPt, mapping, keyPt, valuePt, ret))) {
		dmsg;
		mcr_Map_element_free(mapPt, mapping);
		mcr_Array_remove_index(&mapPt->set,
			MCR_ARR_INDEX(mapPt->set, mapping), 1);
		return ret;
	}
	MCR_MAP_SORT(*mapPt);
	return 0;
}

int mcr_Map_remap(struct mcr_Map *mapPt, const void *previousKeyPt,
	const void *newKeyPt)
{
	dassert(mapPt);
	dassert(previousKeyPt);
	int ret;
	/* New position optional */
	if (newKeyPt) {
		void *oldPlace = mcr_Map_value(mapPt, previousKeyPt);
		/* Old does not exist */
		if (!oldPlace) {
			mcr_Map_unmap(mapPt, newKeyPt);
			return 0;
		}
		/* Old exists */
		if ((ret = mcr_Map_map(mapPt, newKeyPt, oldPlace))) {
			dmsg;
			return ret;
		}
	}
	/* Remove the old */
	mcr_Map_unmap(mapPt, previousKeyPt);
	return 0;
}

int mcr_Map_map_pair(struct mcr_Map *mapPt, const void *mappingPair)
{
	dassert(mapPt);
	dassert(mappingPair);
	return mcr_Map_map(mapPt, mappingPair,
		MCR_MAP_VALUEOF(*mapPt, mappingPair));
}

void mcr_Map_unmap(struct mcr_Map *mapPt, const void *keyPt)
{
	dassert(mapPt);
	dassert(keyPt);
	void *element = MCR_MAP_ELEMENT(*mapPt, keyPt);
	if (element) {
		mcr_Map_element_free(mapPt, element);
		mcr_Array_remove_index(&mapPt->set, MCR_ARR_INDEX(mapPt->set,
				element), 1);
	}
}

void mcr_Map_unmap_value(struct mcr_Map *mapPt, const void *valuePt)
{
	dassert(mapPt);
	dassert(valuePt);
	char *element = MCR_ARR_LAST(mapPt->set);
	if (!element)
		return;
	size_t elementBytes = mapPt->set.element_size,
		keyBytes = mapPt->key_size,
		valueBytes = mapPt->value_size, index = mapPt->set.used - 1;
	mcr_compare_fnc vCmp = mapPt->value_iface ?
		mapPt->value_iface->compare : NULL;
	if (vCmp) {
		for (index = mapPt->set.used; index--; element -= elementBytes) {
			if (vCmp(element + keyBytes, valuePt)) {
				mcr_Map_element_free(mapPt, element);
				mcr_Array_remove_index(&mapPt->set, index, 1);
			}
		}
	} else {
		for (index = mapPt->set.used; index--; element -= elementBytes) {
			if (memcmp(element + keyBytes, valuePt, valueBytes)) {
				mcr_Map_element_free(mapPt, element);
				mcr_Array_remove_index(&mapPt->set, index, 1);
			}
		}
	}
}

int mcr_Map_fill(struct mcr_Map *mapPt, const void *keyArray,
	size_t keyCount, const void *valuePt)
{
	dassert(mapPt);
	int ret;
	if (keyArray && keyCount) {
		char *keyIt;
		size_t keySize = mapPt->key_size;
		/* Each mapping may or may not increase used */
		if (mapPt->set.used == mapPt->set.size &&
			(ret = mcr_Map_smartsize(mapPt, keyCount))) {
			dmsg;
			return ret;
		}
		for (keyIt = (char *)keyArray; keyCount; keyCount--) {
			if ((ret = mcr_Map_map(mapPt, keyIt, valuePt))) {
				dmsg;
				return ret;
			}
			keyIt += keySize;
		}
	}
	return 0;
}

int mcr_Map_graph(struct mcr_Map *mapPt, const void *keyArray,
	const void *valueArray, size_t sourceArrayLen)
{
	dassert(mapPt);
	int ret;
	/* Each mapping may or may not increase used */
	if (mapPt->set.used == mapPt->set.size &&
		(ret = mcr_Map_smartsize(mapPt, sourceArrayLen))) {
		dmsg;
		return ret;
	}
	size_t kSize = mapPt->key_size, vSize = mapPt->value_size;
	for (char *keyIt = (char *)keyArray, *vIt = (char *)valueArray;
		sourceArrayLen; --sourceArrayLen) {
		if ((ret = mcr_Map_map(mapPt, keyIt, vIt))) {
			dmsg;
			return ret;
		}
		keyIt += kSize;
		vIt += vSize;
	}
	return 0;
}

/* Sorting */
void mcr_Map_sort(struct mcr_Map *mapPt)
{
	dassert(mapPt);
	MCR_MAP_SORT(*mapPt);
}

/* Comparison */
int mcr_char_compare(const void *lhs, const void *rhs)
{
	if (rhs) {
		if (lhs)
			return MCR_CMP_CAST(*(const char *), lhs, rhs);
		return -1;
	}
	return ! !lhs;
}

int mcr_name_compare(const void *lhs, const void *rhs)
{
	if (rhs) {
		if (lhs)
			return lhs == rhs ? 0 :
				strcasecmp(*(const char *const *)lhs,
				*(const char *const *)rhs);
		return -1;
	}
	/* Not rhs, but may have lhs */
	return ! !lhs;
}

int mcr_str_compare(const void *lhs, const void *rhs)
{
	if (rhs) {
		if (lhs)
			return lhs == rhs ? 0 :
				strcmp(*(const char *const *)lhs,
				*(const char *const *)rhs);
		return -1;
	}
	/* Not rhs, but may have lhs */
	return ! !lhs;
}

int mcr_int_compare(const void *lhs, const void *rhs)
{
	if (rhs) {
		if (lhs)
			return MCR_CMP_CAST(*(const int *), lhs, rhs);
		return -1;
	}
	/* Not rhs, but may have lhs */
	return ! !lhs;
}

int mcr_unsigned_compare(const void *lhs, const void *rhs)
{
	if (rhs) {
		if (lhs)
			return MCR_CMP_CAST(*(const unsigned int *), lhs, rhs);
		return -1;
	}
	/* Not rhs, but may have lhs */
	return ! !lhs;
}

int mcr_size_t_compare(const void *lhs, const void *rhs)
{
	if (rhs) {
		if (lhs)
			return MCR_CMP_CAST(*(const size_t *), lhs, rhs);
		return -1;
	}
	/* Not rhs, but may have lhs */
	return ! !lhs;
}

int mcr_ref_compare(const void *lhs, const void *rhs)
{
	if (rhs) {
		if (lhs)
			return MCR_CMP_CAST(*(void *const *), lhs, rhs);
		return -1;
	}
	/* Not rhs, but may have lhs */
	return ! !lhs;
}

static void mcr_Map_key_init(const struct mcr_Map *mapPt, void *keyPt)
{
	mcr_data_fnc kInit = mapPt->key_iface ? mapPt->key_iface->init : NULL;
	if (kInit)
		kInit(keyPt);
}

static void mcr_Map_value_init(const struct mcr_Map *mapPt, void *valuePt)
{
	mcr_data_fnc vInit = mapPt->value_iface ?
		mapPt->value_iface->init : NULL;
	if (vInit)
		vInit(valuePt);
}

static void mcr_Map_key_free(const struct mcr_Map *mapPt, void *keyPt)
{
	mcr_data_fnc kFree = mapPt->key_iface ? mapPt->key_iface->free : NULL;
	if (kFree)
		kFree(keyPt);
}

static void mcr_Map_value_free(const struct mcr_Map *mapPt, void *valuePt)
{
	mcr_data_fnc vFree = mapPt->value_iface ?
		mapPt->value_iface->free : NULL;
	if (vFree)
		vFree(valuePt);
}

static void mcr_Map_init_range(struct mcr_Map *mapPt, size_t firstPos,
	size_t lastPos)
{
	/* Non-variadic, so we need MCR_ARGSWAP */
	mcr_data_fnc kInit = mapPt->key_iface ?
		mapPt->key_iface->init : NULL,
		vInit = mapPt->value_iface ? mapPt->value_iface->init : NULL;
	char *it, *end;
	size_t bytes;
	if (kInit) {
		MCR_MAP_ITER_RANGE(*mapPt, it, end, bytes, firstPos, lastPos);
		while (it < end) {
			kInit(it);
			it += bytes;
		}
	}
	if (vInit) {
		MCR_MAP_ITER_RANGE(*mapPt, it, end, bytes, firstPos, lastPos);
		while (it < end) {
			vInit(it);
			it += bytes;
		}
	}
}

static void mcr_Map_free_range(struct mcr_Map *mapPt, size_t firstPos,
	size_t lastPos)
{
	/* Non-variadic, so we need MCR_ARGSWAP */
	mcr_data_fnc kFree = mapPt->key_iface ?
		mapPt->key_iface->free : NULL,
		vFree = mapPt->value_iface ? mapPt->value_iface->free : NULL;
	char *it, *end;
	size_t bytes;
	if (kFree) {
		MCR_MAP_ITER_RANGE(*mapPt, it, end, bytes, firstPos, lastPos);
		while (it < end) {
			kFree(it);
			it += bytes;
		}
	}
	if (vFree) {
		MCR_MAP_ITER_RANGE(*mapPt, it, end, bytes, firstPos, lastPos);
		while (it < end) {
			vFree(it);
			it += bytes;
		}
	}
}

static int mcr_Map_key_copy(const struct mcr_Map *mapPt, void *keyPt,
	const void *copyKeyPt)
{
	mcr_copy_fnc kCopy = mapPt->key_iface ? mapPt->key_iface->copy : NULL;
	mcr_data_fnc kFree = mapPt->key_iface ? mapPt->key_iface->free : NULL;
	if (kCopy)
		return kCopy(keyPt, (void *)copyKeyPt);

	if (copyKeyPt)
		memcpy(keyPt, copyKeyPt, mapPt->key_size);
	else if (kFree)
		kFree(keyPt);
	else
		memset(keyPt, 0, mapPt->key_size);
	return 0;
}

static int mcr_Map_value_copy(const struct mcr_Map *mapPt, void *valuePt,
	const void *copyValuePt)
{
	mcr_copy_fnc vCopy = mapPt->value_iface ?
		mapPt->value_iface->copy : NULL;
	mcr_data_fnc vFree = mapPt->value_iface ?
		mapPt->value_iface->free : NULL;
	if (vCopy)
		return vCopy(valuePt, (void *)copyValuePt);

	if (copyValuePt)
		memcpy(valuePt, copyValuePt, mapPt->value_size);
	else if (vFree)
		vFree(valuePt);
	else
		memset(valuePt, 0, mapPt->value_size);
	return 0;
}
