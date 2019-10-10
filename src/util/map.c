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

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int mcr_Map_key_init(const struct mcr_Map *mapPt, void *keyPt);
static int mcr_Map_value_init(const struct mcr_Map *mapPt, void *valuePt);
static int mcr_Map_key_deinit(const struct mcr_Map *mapPt, void *keyPt);
static int mcr_Map_value_deinit(const struct mcr_Map *mapPt, void *valuePt);
static int mcr_Map_init_range(struct mcr_Map *mapPt, size_t firstPos,
							  size_t lastPos);
static int mcr_Map_deinit_range(struct mcr_Map *mapPt, size_t firstPos,
								size_t lastPos);
static int mcr_Map_key_copy(const struct mcr_Map *mapPt, void *keyPt,
							void *copyKeyPt);
static int mcr_Map_value_copy(const struct mcr_Map *mapPt, void *valuePt,
							  void *copyValuePt);

static int mcr_Map_element_init(const struct mcr_Map *mapPt, void *elementPt);
static int mcr_Map_element_deinit(const struct mcr_Map *mapPt, void *elementPt);
static int mcr_Map_element_copy(const struct mcr_Map *mapPt, void *elementPt,
								void *copyKeyPt, void *copyValuePt);

static int mcr_Map_unmap_value_memcmp(struct mcr_Map *mapPt,
									  const void *valuePt);
static int mcr_Map_malloc_value(const struct mcr_Map *mapPt, void **valPtPt);
static int mcr_Map_deinit_value(const struct mcr_Map *mapPt, void *valPt);

int mcr_Map_init(void *mapPt)
{
	struct mcr_Map *localMapPt = mapPt;
	if (localMapPt) {
		memset(localMapPt, 0, sizeof(struct mcr_Map));
		localMapPt->key_size = 1;
		localMapPt->value_size = 1;
		mcr_Array_init(&localMapPt->set);
		mcr_Array_set_all(&localMapPt->set, mcr_char_compare, 2);
	}
	return 0;
}

struct mcr_Map mcr_Map_new(size_t keySize,
						   size_t valueSize, mcr_compare_fnc compare,
						   const struct mcr_Interface *keyIface,
						   const struct mcr_Interface *valueIface)
{
	struct mcr_Map ret;
	mcr_Map_init(&ret);
	mcr_Map_set_all(&ret, keySize, valueSize, compare, keyIface,
					valueIface);
	return ret;
}

int mcr_Map_deinit(void *mapPt)
{
	int err = 0;
	struct mcr_Map *localMapPt = mapPt;
	if (localMapPt) {
		err = mcr_Map_deinit_range(localMapPt, 0,
								   localMapPt->set.used - 1);
		mcr_Array_deinit(&localMapPt->set);
	}
	return err;
}

int mcr_Map_set_all(struct mcr_Map *mapPt, size_t keySize,
					size_t valueSize,
					mcr_compare_fnc compare, const struct mcr_Interface *keyIface,
					const struct mcr_Interface *valueIface)
{
	dassert(mapPt);
	if (mapPt->set.size && mcr_Map_deinit(mapPt))
		return mcr_err;
	/* Cannot map without both a comparison function, and elements to
	 * compare
	 */
	if (keyIface) {
		mapPt->key_interface = keyIface;
		if (!keySize)
			keySize = keyIface->data_size;
		if (!compare)
			compare = keyIface->compare;
	}
	if (valueIface) {
		mapPt->value_interface = valueIface;
		if (!valueSize)
			valueSize = valueIface->data_size;
	}
	dassert(keySize);
	dassert(valueSize);
	dassert(compare);
	mapPt->key_size = keySize;
	mapPt->value_size = valueSize;
	mcr_Array_set_all(&mapPt->set, compare, keySize + valueSize);
	return 0;
}

/* Allocation control */
int mcr_Map_minused(struct mcr_Map *mapPt, size_t minUsed)
{
	size_t prevUsed = mapPt->set.used;
	dassert(mapPt);
	if (prevUsed < minUsed) {
		if (mcr_Array_minused(&mapPt->set, minUsed))
			return mcr_err;
		return mcr_Map_init_range(mapPt, prevUsed, minUsed - 1);
	}
	return 0;
}

int mcr_Map_minsize(struct mcr_Map *mapPt, size_t minSize)
{
	dassert(mapPt);
	return mcr_Array_minsize(&mapPt->set, minSize);
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
	if (newSize < mapPt->set.used &&
		mcr_Map_deinit_range(mapPt, newSize,
							 mapPt->set.used - 1)) {
		return mcr_err;
	}
	return mcr_Array_resize(&mapPt->set, newSize);
}

int mcr_Map_clear(struct mcr_Map *mapPt)
{
	int err = mcr_Map_deinit_range(mapPt, 0, mapPt->set.used - 1);
	mapPt->set.used = 0;
	return err;
}

/* Position */
void *mcr_Map_valueof(const struct mcr_Map *mapPt, void *pairPt)
{
	if (mapPt)
		return MCR_MAP_VALUEOF(*mapPt, pairPt);
	return NULL;
}

int mcr_Map_set_key(const struct mcr_Map *mapPt, void *pairPt, void *copyKeyPt)
{
	return mapPt ? mcr_Map_key_copy(mapPt, pairPt, copyKeyPt) : 0;
}

int mcr_Map_set_valueof(const struct mcr_Map *mapPt, void *pairPt,
						void *copyValuePt)
{
	dassert(pairPt);
	return mapPt ? mcr_Map_value_copy(mapPt, MCR_MAP_VALUEOF(*mapPt,
									  pairPt), copyValuePt) : 0;
}

void *mcr_Map_element(const struct mcr_Map *mapPt, const void *keyPt)
{
	return mapPt ? MCR_MAP_ELEMENT(*mapPt, keyPt) : NULL;
}

void *mcr_Map_value(const struct mcr_Map *mapPt, const void *keyPt)
{
	void *ret;
	if (!mapPt)
		return NULL;
	ret = MCR_MAP_ELEMENT(*mapPt, keyPt);
	return MCR_MAP_VALUEOF(*mapPt, ret);
}

void *mcr_Map_element_ensured(struct mcr_Map *mapPt, void *keyPt)
{
	void *found;
	dassert(mapPt);
	dassert(keyPt);
	if ((found = MCR_MAP_ELEMENT(*mapPt, keyPt)))
		return found;
	if (mcr_Map_map(mapPt, keyPt, NULL))
		return NULL;
	return MCR_MAP_ELEMENT(*mapPt, keyPt);
}

size_t mcr_Map_index(const struct mcr_Map * mapPt, const void *keyPt)
{
	void *found;
	if (!mapPt)
		return (size_t)-1;
	found = MCR_MAP_ELEMENT(*mapPt, keyPt);
	return MCR_MAP_INDEX(*mapPt, found);
}

void mcr_Map_iter(const struct mcr_Map *mapPt, char **iterPt,
				  char **endPt, size_t *bytesPt)
{
	mcr_Array_iter(&mapPt->set, iterPt, endPt, bytesPt);
}

void mcr_Map_iter_range(const struct mcr_Map *mapPt, char **iterPt,
						char **lastPt, size_t *bytesPt, size_t firstIndex,
						size_t lastIndex)
{
	mcr_Array_iter_range(&mapPt->set, iterPt, lastPt, bytesPt,
						 firstIndex, lastIndex);
}

/* Set/remove mappings */
int mcr_Map_map(struct mcr_Map *mapPt, void *keyPt, void *valuePt)
{
	void *mapping;
	dassert(mapPt);
	dassert(keyPt);
	if ((mapping = MCR_MAP_ELEMENT(*mapPt, keyPt)))
		return mcr_Map_set_valueof(mapPt, mapping, valuePt);
	if (mcr_Array_push(&mapPt->set, NULL))
		return mcr_err;
	mapping = MCR_ARR_LAST(mapPt->set);
	if (mcr_Map_element_init(mapPt, mapping) ||
		mcr_Map_element_copy(mapPt, mapping, keyPt, valuePt)) {
		dmsg;
		mcr_Map_element_deinit(mapPt, mapping);
		mcr_Array_pop(&mapPt->set);
		return mcr_err;
	}
	MCR_MAP_SORT(*mapPt);
	return 0;
}

int mcr_Map_remap(struct mcr_Map *mapPt, const void *previousKeyPt,
				  void *newKeyPt)
{
	void *oldPlace;
	dassert(mapPt);
	if (previousKeyPt == newKeyPt)
		return 0;
	/* New position optional */
	if (newKeyPt) {
		oldPlace = mcr_Map_value(mapPt, previousKeyPt);
		/* Old does not exist (possibly null key), remove mapping to
		 * new */
		if (!oldPlace)
			return mcr_Map_unmap(mapPt, newKeyPt);
		/* Old exists, map new key to old value.
		 * If new exists this will copy over current value in-place,
		 * if not this will push new value to end, and then sort.
		 */
		if (mcr_Map_map(mapPt, newKeyPt, oldPlace))
			return mcr_err;
	}
	/* In all cases, remove the old key */
	return mcr_Map_unmap(mapPt, previousKeyPt);
}

int mcr_Map_map_pair(struct mcr_Map *mapPt, void *mappingPair)
{
	dassert(mapPt);
	dassert(mappingPair);
	return mcr_Map_map(mapPt, mappingPair,
					   MCR_MAP_VALUEOF(*mapPt, mappingPair));
}

int mcr_Map_unmap(struct mcr_Map *mapPt, const void *keyPt)
{
	void *element = MCR_MAP_ELEMENT(*mapPt, keyPt);
	dassert(mapPt);
	dassert(keyPt);
	if (element) {
		if (mcr_Map_element_deinit(mapPt, element))
			return mcr_err;
		mcr_Array_remove_index(&mapPt->set, MCR_ARR_INDEX(mapPt->set,
							   element), 1);
	}
	return 0;
}

int mcr_Map_unmap_value(struct mcr_Map *mapPt, const void *valuePt)
{
	char *element;
	size_t elementBytes = mapPt->set.element_size,
			  keyBytes = mapPt->key_size, index;
	mcr_compare_fnc vCmp = mapPt->value_interface ?
						   mapPt->value_interface->compare : NULL;
	dassert(mapPt);
	if (!mapPt->set.used)
		return 0;
	if (vCmp) {
		element = MCR_ARR_LAST(mapPt->set);
		for (index = mapPt->set.used; index--; element -= elementBytes) {
			/* Val found, remove element */
			if (!vCmp(element + keyBytes, valuePt)) {
				if (mcr_Map_element_deinit(mapPt,
										   element)) {
					return mcr_err;
				}
				mcr_Array_remove_index(&mapPt->set, index, 1);
			}
		}
		return 0;
	}
	return mcr_Map_unmap_value_memcmp(mapPt, valuePt);
}

int mcr_Map_fill(struct mcr_Map *mapPt, void *keyArray,
				 size_t keyCount, void *valuePt)
{
	char *keyIt;
	size_t keySize = mapPt->key_size;
	dassert(mapPt);
	if (keyArray && keyCount) {
		/* Mapping will increase size between 0 and the number
		 * of keys */
		if (mcr_Map_smartsize(mapPt, keyCount))
			return mcr_err;
		for (keyIt = (char *)keyArray; keyCount; keyCount--) {
			if (mcr_Map_map(mapPt, keyIt, valuePt))
				return mcr_err;
			keyIt += keySize;
		}
	}
	return 0;
}

int mcr_Map_graph(struct mcr_Map *mapPt, void *keyArray,
				  void *valueArray, size_t sourceArrayLen)
{
	char *keyIt, *vIt;
	size_t kSize = mapPt->key_size, vSize = mapPt->value_size;
	dassert(mapPt);
	/* Nothing to map */
	if (!keyArray)
		return 0;
	/* No values, fill all with null */
	if (!valueArray)
		return mcr_Map_fill(mapPt, keyArray, sourceArrayLen, NULL);
	/* Mapping will increase size between 0 and the number
	 * of mappings */
	if (mcr_Map_smartsize(mapPt, sourceArrayLen))
		return mcr_err;
	for (keyIt = (char *)keyArray, vIt = (char *)valueArray;
		 sourceArrayLen; --sourceArrayLen) {
		if (mcr_Map_map(mapPt, keyIt, vIt))
			return mcr_err;
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
	return MCR_CMP_PTR(const char, lhs, rhs);
}

int mcr_name_compare(const void *lhs, const void *rhs)
{
	if (lhs && rhs) {
		return mcr_casecmp(*(const char *const *)lhs, *(const char *const *)rhs);
	}
	return MCR_CMP(lhs, rhs);
}

int mcr_str_compare(const void *lhs, const void *rhs)
{
	if (lhs && rhs)
		return strcmp(*(const char *const *)lhs, *(const char *const *)rhs);
	return MCR_CMP(lhs, rhs);
}

int mcr_int_compare(const void *lhs, const void *rhs)
{
	return MCR_CMP_PTR(const int, lhs, rhs);
}

int mcr_unsigned_compare(const void *lhs, const void *rhs)
{
	return MCR_CMP_PTR(const unsigned int, lhs, rhs);
}

int mcr_ref_compare(const void *lhs, const void *rhs)
{
	return MCR_CMP_PTR(const void *const, lhs, rhs);
}

static int mcr_Map_key_init(const struct mcr_Map *mapPt, void *keyPt)
{
	mcr_data_fnc kInit =
		mapPt->key_interface ? mapPt->key_interface->init : NULL;
	if (kInit)
		return kInit(keyPt);
	memset(keyPt, 0, mapPt->key_size);
	return 0;
}

static int mcr_Map_value_init(const struct mcr_Map *mapPt, void *valuePt)
{
	mcr_data_fnc vInit = mapPt->value_interface ?
						 mapPt->value_interface->init : NULL;
	if (vInit)
		return vInit(valuePt);
	memset(valuePt, 0, mapPt->value_size);
	return 0;
}

static int mcr_Map_key_deinit(const struct mcr_Map *mapPt, void *keyPt)
{
	mcr_data_fnc kDeinit =
		mapPt->key_interface ? mapPt->key_interface->deinit : NULL;
	return kDeinit ? kDeinit(keyPt) : 0;
	/* Key will probably not be referenced again */
}

static int mcr_Map_value_deinit(const struct mcr_Map *mapPt, void *valuePt)
{
	mcr_data_fnc vDeinit = mapPt->value_interface ?
						   mapPt->value_interface->deinit : NULL;
	return vDeinit ? vDeinit(valuePt) : 0;
	/* Value will probably not be referenced again */
}

static int mcr_Map_init_range(struct mcr_Map *mapPt, size_t firstPos,
							  size_t lastPos)
{
	mcr_data_fnc kInit = mapPt->key_interface ?
						 mapPt->key_interface->init : NULL,
						 vInit =
							 mapPt->value_interface ? mapPt->value_interface->init : NULL;
	char *itPt, *lastPt;
	size_t bytes, keySize = mapPt->key_size;
	/* For init range, when initializer is not available we do not memset,
	 * because values will be set while mapping */
	if (kInit) {
		mcr_Map_iter_range(mapPt, &itPt, &lastPt, &bytes, firstPos,
						   lastPos);
		if (itPt && lastPt) {
			while (itPt <= lastPt) {
				if (kInit(itPt))
					return mcr_err;
				itPt += bytes;
			}
		}
	}
	if (vInit) {
		mcr_Map_iter_range(mapPt, &itPt, &lastPt, &bytes, firstPos,
						   lastPos);
		if (itPt && lastPt) {
			while (itPt <= lastPt) {
				if (vInit(itPt + keySize))
					return mcr_err;
				itPt += bytes;
			}
		}
	}
	return 0;
}

static int mcr_Map_deinit_range(struct mcr_Map *mapPt, size_t firstPos,
								size_t lastPos)
{
	mcr_data_fnc kDeinit = mapPt->key_interface ?
						   mapPt->key_interface->deinit : NULL,
						   vDeinit =
							   mapPt->value_interface ? mapPt->value_interface->deinit : NULL;
	char *itPt, *lastPt;
	size_t bytes, keySize = mapPt->key_size;
	if (kDeinit) {
		mcr_Map_iter_range(mapPt, &itPt, &lastPt, &bytes, firstPos,
						   lastPos);
		if (itPt && lastPt) {
			while (itPt <= lastPt) {
				if (kDeinit(itPt))
					return mcr_err;
				itPt += bytes;
			}
		}
	}
	if (vDeinit) {
		mcr_Map_iter_range(mapPt, &itPt, &lastPt, &bytes, firstPos,
						   lastPos);
		if (itPt && lastPt) {
			while (itPt <= lastPt) {
				if (vDeinit(itPt + keySize))
					return mcr_err;
				itPt += bytes;
			}
		}
	}
	return 0;
}

static int mcr_Map_key_copy(const struct mcr_Map *mapPt, void *keyPt,
							void *copyKeyPt)
{
	mcr_copy_fnc kCopy =
		mapPt->key_interface ? mapPt->key_interface->copy : NULL;
	mcr_data_fnc kDeinit =
		mapPt->key_interface ? mapPt->key_interface->deinit : NULL;
	/* kCopy should handle copying null value */
	if (kCopy)
		return kCopy(keyPt, copyKeyPt);

	/* copy directly, or deinit */
	if (copyKeyPt) {
		memcpy(keyPt, copyKeyPt, mapPt->key_size);
	} else if (kDeinit) {
		return kDeinit(keyPt);
	} else {
		memset(keyPt, 0, mapPt->key_size);
	}
	return 0;
}

static int mcr_Map_value_copy(const struct mcr_Map *mapPt, void *valuePt,
							  void *copyValuePt)
{
	mcr_copy_fnc vCopy = mapPt->value_interface ?
						 mapPt->value_interface->copy : NULL;
	mcr_data_fnc vDeinit = mapPt->value_interface ?
						   mapPt->value_interface->deinit : NULL;
	/* kCopy should handle copying null value */
	if (vCopy)
		return vCopy(valuePt, copyValuePt);

	/* copy directly, or deinit */
	if (copyValuePt) {
		memcpy(valuePt, copyValuePt, mapPt->value_size);
	} else if (vDeinit) {
		return vDeinit(valuePt);
	} else {
		memset(valuePt, 0, mapPt->value_size);
	}
	return 0;
}

static int mcr_Map_element_init(const struct mcr_Map *mapPt, void *elementPt)
{
	if (mcr_Map_key_init(mapPt, elementPt))
		return mcr_err;
	return mcr_Map_value_init(mapPt, MCR_MAP_VALUEOF(*mapPt, elementPt));
}

static int mcr_Map_element_deinit(const struct mcr_Map *mapPt, void *elementPt)
{
	if (mcr_Map_key_deinit(mapPt, elementPt))
		return mcr_err;
	return mcr_Map_value_deinit(mapPt, MCR_MAP_VALUEOF(*mapPt, elementPt));
}

static int mcr_Map_element_copy(const struct mcr_Map *mapPt, void *elementPt,
								void *copyKeyPt, void *copyValuePt)
{
	if (mcr_Map_key_copy(mapPt, elementPt, copyKeyPt))
		return mcr_err;
	return mcr_Map_value_copy(mapPt, MCR_MAP_VALUEOF(*mapPt, elementPt),
							  copyValuePt);
}

static int mcr_Map_unmap_value_memcmp(struct mcr_Map *mapPt,
									  const void *valuePt)
{
	char *element;
	void *valHolder = 0;
	size_t elementBytes = mapPt->set.element_size,
			  keyBytes = mapPt->key_size,
			  valueBytes = mapPt->value_size, index;
	element = MCR_ARR_LAST(mapPt->set);
	if (!valuePt) {
		if (mcr_Map_malloc_value(mapPt, &valHolder))
			return mcr_err;
		valuePt = valHolder;
	}
	for (index = mapPt->set.used; index--; element -= elementBytes) {
		/* Val found, remove element */
		if (!memcmp(element + keyBytes, valuePt, valueBytes)) {
			if (mcr_Map_element_deinit(mapPt, element)) {
				if (valHolder)
					mcr_Map_deinit_value(mapPt, valHolder);
				return mcr_err;
			}
			mcr_Array_remove_index(&mapPt->set, index, 1);
		}
	}
	if (valHolder)
		return mcr_Map_deinit_value(mapPt, valHolder);
	return 0;
}

static int mcr_Map_malloc_value(const struct mcr_Map *mapPt, void **valPtPt)
{
	*valPtPt = malloc(mapPt->value_size);
	if (!*valPtPt)
		mset_error_return(ENOMEM);
	if (mcr_Map_value_init(mapPt, *valPtPt)) {
		dmsg;
		free(*valPtPt);
		*valPtPt = NULL;
		return mcr_err;
	}
	return 0;
}

static int mcr_Map_deinit_value(const struct mcr_Map *mapPt, void *valPt)
{
	int err = mcr_Map_value_deinit(mapPt, valPt);
	free(valPt);
	return err;
}
