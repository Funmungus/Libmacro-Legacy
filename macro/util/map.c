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

int mcr_Map_set_all(struct mcr_Map *mapPt, size_t keySize, size_t valueSize,
					mcr_compare_fnc compare, const struct mcr_Interface *keyIface,
					const struct mcr_Interface *valueIface)
{
	int err;
	dassert(mapPt);
	if (mapPt->set.size && (err = mcr_Map_deinit(mapPt)))
		return err;
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
	int err;
	dassert(mapPt);
	if (prevUsed < minUsed) {
		if ((err = mcr_Array_minused(&mapPt->set, minUsed)))
			return err;
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
	int err;
	dassert(mapPt);
	if (newSize < mapPt->set.used &&
		(err = mcr_Map_deinit_range(mapPt, newSize,
									mapPt->set.used - 1)))
		return err;
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
	int err;
	dassert(mapPt);
	dassert(keyPt);
	if ((found = MCR_MAP_ELEMENT(*mapPt, keyPt)))
		return found;
	if ((err = mcr_Map_map(mapPt, keyPt, NULL)))
		return NULL;
	return MCR_MAP_ELEMENT(*mapPt, keyPt);
}

size_t mcr_Map_index(const struct mcr_Map * mapPt, const void *keyPt)
{
	void *found;
	if (!mapPt)
		return -1;
	found = MCR_MAP_ELEMENT(*mapPt, keyPt);
	return MCR_MAP_INDEX(*mapPt, found);
}

void mcr_Map_iter(const struct mcr_Map *mapPt, char **iterPt,
				  char **endPt, size_t * bytesPt)
{
	mcr_Array_iter(&mapPt->set, iterPt, endPt, bytesPt);
}

void mcr_Map_iter_range(const struct mcr_Map *mapPt, char **iterPt,
						char **lastPt, size_t * bytesPt, size_t firstIndex, size_t lastIndex)
{
	mcr_Array_iter_range(&mapPt->set, iterPt, lastPt, bytesPt,
						 firstIndex, lastIndex);
}

/* Set/remove mappings */
int mcr_Map_map(struct mcr_Map *mapPt, void *keyPt, void *valuePt)
{
	void *mapping;
	int err;
	dassert(mapPt);
	dassert(keyPt);
	if ((mapping = MCR_MAP_ELEMENT(*mapPt, keyPt)))
		return mcr_Map_set_valueof(mapPt, mapping, valuePt);
	if ((err = mcr_Array_push(&mapPt->set, NULL)))
		return err;
	mapping = MCR_ARR_LAST(mapPt->set);
	if ((err = mcr_Map_element_init(mapPt, mapping)) ||
		(err = mcr_Map_element_copy(mapPt, mapping, keyPt, valuePt))) {
		dmsg;
		mcr_Map_element_deinit(mapPt, mapping);
		mcr_Array_pop(&mapPt->set);
		return err;
	}
	MCR_MAP_SORT(*mapPt);
	return 0;
}

int mcr_Map_remap(struct mcr_Map *mapPt, const void *previousKeyPt,
				  void *newKeyPt)
{
	int err;
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
		if ((err = mcr_Map_map(mapPt, newKeyPt, oldPlace)))
			return err;
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
	int err;
	void *element = MCR_MAP_ELEMENT(*mapPt, keyPt);
	dassert(mapPt);
	dassert(keyPt);
	if (element) {
		if ((err = mcr_Map_element_deinit(mapPt, element)))
			return err;
		mcr_Array_remove_index(&mapPt->set, MCR_ARR_INDEX(mapPt->set,
							   element), 1);
	}
	return 0;
}

int mcr_Map_unmap_value(struct mcr_Map *mapPt, const void *valuePt)
{
	int err;
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
				if ((err = mcr_Map_element_deinit(mapPt,
												  element)))
					return err;
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
	int err;
	char *keyIt;
	size_t keySize = mapPt->key_size;
	dassert(mapPt);
	if (keyArray && keyCount) {
		/* Mapping will increase size between 0 and the number
		 * of keys */
		if ((err = mcr_Map_smartsize(mapPt, keyCount)))
			return err;
		for (keyIt = (char *)keyArray; keyCount; keyCount--) {
			if ((err = mcr_Map_map(mapPt, keyIt, valuePt)))
				return err;
			keyIt += keySize;
		}
	}
	return 0;
}

int mcr_Map_graph(struct mcr_Map *mapPt, void *keyArray,
				  void *valueArray, size_t sourceArrayLen)
{
	int err;
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
	if ((err = mcr_Map_smartsize(mapPt, sourceArrayLen)))
		return err;
	for (keyIt = (char *)keyArray, vIt = (char *)valueArray;
		 sourceArrayLen; --sourceArrayLen) {
		if ((err = mcr_Map_map(mapPt, keyIt, vIt)))
			return err;
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
				   mcr_casecmp(*(const char *const *)lhs,
							   *(const char *const *)rhs);
		return -1;
	}
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
	return ! !lhs;
}

int mcr_int_compare(const void *lhs, const void *rhs)
{
	if (rhs) {
		if (lhs)
			return MCR_CMP_CAST(*(const int *), lhs, rhs);
		return -1;
	}
	return ! !lhs;
}

int mcr_unsigned_compare(const void *lhs, const void *rhs)
{
	if (rhs) {
		if (lhs)
			return MCR_CMP_CAST(*(const unsigned int *), lhs, rhs);
		return -1;
	}
	return ! !lhs;
}

int mcr_size_t_compare(const void *lhs, const void *rhs)
{
	if (rhs) {
		if (lhs)
			return MCR_CMP_CAST(*(const size_t *), lhs, rhs);
		return -1;
	}
	return ! !lhs;
}

int mcr_ref_compare(const void *lhs, const void *rhs)
{
	if (rhs) {
		if (lhs)
			return MCR_CMP_CAST(*(const void *const *), lhs, rhs);
		return -1;
	}
	return ! !lhs;
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
	int err;
	/* For init range, when initializer is not available we do not memset,
	 * because values will be set while mapping */
	if (kInit) {
		mcr_Map_iter_range(mapPt, &itPt, &lastPt, &bytes, firstPos,
						   lastPos);
		if (itPt && lastPt) {
			while (itPt <= lastPt) {
				if ((err = kInit(itPt)))
					return err;
				itPt += bytes;
			}
		}
	}
	if (vInit) {
		mcr_Map_iter_range(mapPt, &itPt, &lastPt, &bytes, firstPos,
						   lastPos);
		if (itPt && lastPt) {
			while (itPt <= lastPt) {
				if ((err = vInit(itPt + keySize)))
					return err;
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
	int err;
	if (kDeinit) {
		mcr_Map_iter_range(mapPt, &itPt, &lastPt, &bytes, firstPos,
						   lastPos);
		if (itPt && lastPt) {
			while (itPt <= lastPt) {
				if ((err = kDeinit(itPt)))
					return err;
				itPt += bytes;
			}
		}
	}
	if (vDeinit) {
		mcr_Map_iter_range(mapPt, &itPt, &lastPt, &bytes, firstPos,
						   lastPos);
		if (itPt && lastPt) {
			while (itPt <= lastPt) {
				if ((err = vDeinit(itPt + keySize)))
					return err;
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
	if (copyKeyPt)
		memcpy(keyPt, copyKeyPt, mapPt->key_size);
	else if (kDeinit)
		return kDeinit(keyPt);
	else
		memset(keyPt, 0, mapPt->key_size);
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
	if (copyValuePt)
		memcpy(valuePt, copyValuePt, mapPt->value_size);
	else if (vDeinit)
		return vDeinit(valuePt);
	else
		memset(valuePt, 0, mapPt->value_size);
	return 0;
}

static int mcr_Map_element_init(const struct mcr_Map *mapPt, void *elementPt)
{
	int err = mcr_Map_key_init(mapPt, elementPt);
	if (err)
		return err;
	return mcr_Map_value_init(mapPt, MCR_MAP_VALUEOF(*mapPt, elementPt));
}

static int mcr_Map_element_deinit(const struct mcr_Map *mapPt, void *elementPt)
{
	int err = mcr_Map_key_deinit(mapPt, elementPt);
	if (err)
		return err;
	return mcr_Map_value_deinit(mapPt, MCR_MAP_VALUEOF(*mapPt, elementPt));
}

static int mcr_Map_element_copy(const struct mcr_Map *mapPt, void *elementPt,
								void *copyKeyPt, void *copyValuePt)
{
	int err = mcr_Map_key_copy(mapPt, elementPt, copyKeyPt);
	if (err)
		return err;
	return mcr_Map_value_copy(mapPt, MCR_MAP_VALUEOF(*mapPt, elementPt),
							  copyValuePt);
}

static int mcr_Map_unmap_value_memcmp(struct mcr_Map *mapPt,
									  const void *valuePt)
{
	int err;
	char *element;
	void *valHolder;
	size_t elementBytes = mapPt->set.element_size,
		   keyBytes = mapPt->key_size,
		   valueBytes = mapPt->value_size, index;
	element = MCR_ARR_LAST(mapPt->set);
	if (valuePt)
		valHolder = (void *)valuePt;
	else if ((err = mcr_Map_malloc_value(mapPt, &valHolder)))
		return err;
	for (index = mapPt->set.used; index--; element -= elementBytes) {
		/* Val found, remove element */
		if (!memcmp(element + keyBytes, valHolder, valueBytes)) {
			if ((err = mcr_Map_element_deinit(mapPt, element))) {
				dmsg;
				if (!valuePt)
					mcr_Map_deinit_value(mapPt, valHolder);
				return err;
			}
			mcr_Array_remove_index(&mapPt->set, index, 1);
		}
	}
	if (!valuePt)
		return mcr_Map_deinit_value(mapPt, valHolder);
	return 0;
}

static int mcr_Map_malloc_value(const struct mcr_Map *mapPt, void **valPtPt)
{
	int err;
	*valPtPt = malloc(mapPt->value_size);
	if (!*valPtPt) {
		mset_error(ENOMEM);
		return ENOMEM;
	}
	if ((err = mcr_Map_value_init(mapPt, *valPtPt))) {
		dmsg;
		free(*valPtPt);
		*valPtPt = NULL;
		return err;
	}
	return 0;
}

static int mcr_Map_deinit_value(const struct mcr_Map *mapPt, void *valPt)
{
	int err = mcr_Map_value_deinit(mapPt, valPt);
	free(valPt);
	return err;
}
