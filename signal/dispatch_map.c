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

#include "mcr/signal/signal.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Map key interface scheme, and why we do not need to manually type every
 * map initializer.
 *
 * Mapping to no keys, modifiers + flags => mcr_Array_DispatchPair_iface
 *
 * Mapping into keys, cur map key interface,
 * val interface is NULL, and val size is size of a map.  Values will be
 * zeroed out(including val iface), and must have their key interface, and
 * value size set manually.  If they are the final map, then they need to map
 * into mcr_Array_DispatchPair_iface.
 *
 * After interfaces are set, they should be constant.  After keys are mapped
 * do not change key interfaces.
 */

/*
 * This function dispatches into a \ref mcr_Trigger object.
 *
 * Triggers are defined later on, and not known by signals or dispatchers.
 * Trigger receiver must not be NULL.
 */
MCR_API bool mcr_Trigger_receive(void *triggerReceiver,
	struct mcr_Signal * dispatchSignal, unsigned int mods);
static void map_set_key_iface(struct mcr_Map *mapPt,
	struct mcr_Interface *keyIface, bool flagFinalMap);
static void map_remove(struct mcr_Map *mapPt, void *ref,
	unsigned int curDepth, unsigned int maxDepth);
static void map_remove_empties(struct mcr_Map *mapPt, bool trimFlag,
	unsigned int curDepth, unsigned int maxDepth);

void mcr_DispatchMap_init(void *dataPt)
{
	struct mcr_DispatchMap *dmPt = dataPt;
	dassert(dmPt);
	mcr_Map_init(&dmPt->map);
	mcr_Array_init(&dmPt->key_iface_set);
	dmPt->key_iface_set.element_size = sizeof(struct mcr_Interface);
}

void mcr_DispatchMap_free(void *dataPt)
{
	struct mcr_DispatchMap *dmPt = dataPt;
	dassert(dmPt);
	mcr_Map_free(&dmPt->map);
	mcr_Array_free(&dmPt->key_iface_set);
}

int mcr_DispatchMap_set_all(struct mcr_DispatchMap *dmPt,
	struct mcr_Interface *keyIfaceSet, size_t keyIfaceCount)
{
	int ret;
	size_t i = keyIfaceCount;
	dassert(dmPt);
	if (!keyIfaceCount || !keyIfaceSet) {
		mset_error(EINVAL);
		return -EINVAL;
	}
	/* First check valid mapping interface values */
	while (i--) {
		if (!keyIfaceSet[i].compare || !keyIfaceSet[i].data_size) {
			mset_error(EINVAL);
			return -EINVAL;
		}
	}
	if ((ret = mcr_DispatchMap_set_key_count(dmPt, keyIfaceCount)))
		return ret;
	dassert(!dmPt->map.set.used);
	memcpy(dmPt->key_iface_set.array, keyIfaceSet,
		sizeof(struct mcr_Interface) * keyIfaceCount);
	map_set_key_iface(&dmPt->map,
		MCR_ARR_ELEMENT(dmPt->key_iface_set, 0), keyIfaceCount == 1);
	return 0;
}

size_t mcr_DispatchMap_key_count(struct mcr_DispatchMap * dmPt)
{
	return dmPt->key_iface_set.used;
}

int mcr_DispatchMap_set_key_count(struct mcr_DispatchMap *dmPt, size_t count)
{
	int ret;
	if (!count) {
		mset_error(EINVAL);
		return -EINVAL;
	}
	mcr_DispatchMap_clear(dmPt);
	if ((ret = mcr_Array_resize(&dmPt->key_iface_set, count)))
		return ret;
	/* used always == size */
	dmPt->key_iface_set.used = count;
	return 0;
}

const struct mcr_Interface *mcr_DispatchMap_key_iface(struct
	mcr_DispatchMap *dmPt, size_t index)
{
	return index < dmPt->key_iface_set.used ?
		mcr_Array_element(&dmPt->key_iface_set, index) : NULL;
}

int mcr_DispatchMap_set_key_iface(struct mcr_DispatchMap *dmPt,
	size_t index, mcr_compare_fnc compare, mcr_copy_fnc copy,
	size_t data_size, mcr_data_fnc init, mcr_data_fnc free)
{
	struct mcr_Interface *itPt;
	dassert(dmPt);
	if (index >= dmPt->key_iface_set.used) {
		mset_error(EFAULT);
		return -EFAULT;
	}
	itPt = mcr_Array_element(&dmPt->key_iface_set, index);
	mcr_iset_all(itPt, compare, copy, data_size, init, free);
	if (!index) {
		map_set_key_iface(&dmPt->map,
			MCR_ARR_ELEMENT(dmPt->key_iface_set, 0),
			dmPt->key_iface_set.used == 1);
	}
	return 0;
}

int mcr_DispatchMap_set_key_compare(struct mcr_DispatchMap *dmPt,
	size_t index, mcr_compare_fnc compare, size_t dataSize)
{
	struct mcr_Interface *ifacePt;
	dassert(dmPt);
	if (index >= dmPt->key_iface_set.used) {
		mset_error(EFAULT);
		return -EFAULT;
	}
	ifacePt = mcr_Array_element(&dmPt->key_iface_set, index);
	mcr_iset_all(ifacePt, compare, NULL, dataSize, NULL, NULL);
	if (!index) {
		map_set_key_iface(&dmPt->map,
			MCR_ARR_ELEMENT(dmPt->key_iface_set, 0),
			dmPt->key_iface_set.used == 1);
	}
	return 0;
}

int mcr_DispatchMap_add(struct mcr_DispatchMap *dmPt, void **keyPtArr,
	void *receiver, mcr_Dispatcher_receive_fnc receiveFnc)
{
	struct mcr_DispatchPair rVal = { receiver, receiveFnc };
	struct mcr_Array *arrPt = mcr_DispatchMap_receivers_ensured(dmPt,
		keyPtArr);
	if (!arrPt) {
		return mcr_error();
	}
	dassert(dmPt);
	if (!receiveFnc) {
		if (!receiver) {
			mset_error(EINVAL);
			return -EINVAL;
		}
		rVal.dispatch = mcr_Trigger_receive;
	} else if (receiveFnc == mcr_Trigger_receive) {
		if (!receiver) {
			mset_error(EINVAL);
			return -EINVAL;
		}
	}
	return mcr_Array_add(arrPt, &rVal, 1, 1);
}

void mcr_DispatchMap_remove(struct mcr_DispatchMap *dmPt, void *ref)
{
	dassert(dmPt);
	/* forall dmPt maps */
	/* recursive */
	fixme;
	map_remove(&dmPt->map, ref, 0,
		(unsigned int)dmPt->key_iface_set.used - 1);
	/* Minimize mapping keys for speed.  Trimming bad here. */
	mcr_DispatchMap_remove_empties(dmPt, false);
}

void mcr_DispatchMap_remove_empties(struct mcr_DispatchMap *dmPt, bool trimFlag)
{
	dassert(dmPt);
	/* forall tfPt maps */
	/* recursive */
	fixme;
	map_remove_empties(&dmPt->map, trimFlag, 0,
		(unsigned int)dmPt->key_iface_set.used - 1);
}

struct mcr_Array *mcr_DispatchMap_receivers(struct mcr_DispatchMap *dmPt,
	void **keyPtArr)
{
	size_t i = 0, iCount = dmPt->key_iface_set.used;
	struct mcr_Map *mapPt = &dmPt->map;
	dassert(dmPt);
	while (mapPt && i < iCount) {
		mapPt = mcr_Map_value(mapPt, keyPtArr[i]);
	}
	return (struct mcr_Array *)mapPt;
}

struct mcr_Array *mcr_DispatchMap_receivers_ensured(struct mcr_DispatchMap
	*dmPt, void **keyPtArr)
{
	size_t i = 0, iCount = dmPt->key_iface_set.used, last = iCount - 1;
	struct mcr_Map *mapPt = &dmPt->map;
	dassert(dmPt);
	while (i < iCount) {
		if (!(mapPt = mcr_Map_element_ensured(mapPt, keyPtArr[i]))) {
			mset_error(ENOMEM);
			return NULL;
		}
		mapPt = MCR_MAP_VALUEOF(*mapPt, mapPt);
		if (!mapPt->key_iface) {
			map_set_key_iface(mapPt,
				MCR_ARR_ELEMENT(dmPt->key_iface_set, i),
				i == last);
		}
	}
	return (struct mcr_Array *)mapPt;
}

bool mcr_DispatchMap_dispatch(struct mcr_DispatchMap *dmPt,
	struct mcr_Signal *interceptPt, unsigned int mods,
	const void **keyPtArr, const void **genPtArr,
	struct mcr_Map **mapHolderArray, bool * specHandledHolderArr,
	bool * genHandledHolderArr, bool * useGenOnlyArr)
{
	size_t i = 0, count = dmPt->key_iface_set.used;
	void *itPt;
	bool isToBlock = false;
	struct mcr_DispatchPair *receiverIt, *end;
	struct mcr_Array *receiverArrPt;
	dassert(dmPt);
	dassert(interceptPt);
	dassert(keyPtArr);
	dassert(genPtArr);
	dassert(mapHolderArray);
	dassert(specHandledHolderArr);
	dassert(genHandledHolderArr);
	dassert(useGenOnlyArr);
#define localOnComplete(arrPt) \
if ((arrPt)->used) { \
	receiverIt = (struct mcr_DispatchPair *)((arrPt)->array); \
	end = MCR_ARR_END(*arrPt); \
	while (receiverIt < end) { \
		if (receiverIt->dispatch(receiverIt, interceptPt, mods) && !isToBlock) \
			++isToBlock; \
		++receiverIt; \
	} \
}
	memset(specHandledHolderArr, 0, sizeof(bool) * count);
	memset(genHandledHolderArr, 0, sizeof(bool) * count);
	mapHolderArray[0] = &dmPt->map;
	do {
		if (!useGenOnlyArr[i] && !specHandledHolderArr[i]) {
			++specHandledHolderArr[i];
			itPt = MCR_MAP_ELEMENT(*mapHolderArray[i],
				keyPtArr + i);
			if (itPt) {
				/* Final index, dispatch to receivers */
				if (i == 2) {
					receiverArrPt =
						MCR_MAP_VALUEOF(*mapHolderArray
						[2], itPt);
					localOnComplete(receiverArrPt);
				} else {
					/* Move to next mapping */
					mapHolderArray[i + 1] =
						MCR_MAP_VALUEOF(*mapHolderArray
						[i], itPt);
					++i;
					continue;
				}
			}
		}
		if (!genHandledHolderArr[i]) {
			++genHandledHolderArr[i];
			itPt = MCR_MAP_ELEMENT(*mapHolderArray[i],
				genPtArr + i);
			if (itPt) {
				/* Final index, dispatch to receivers */
				if (i == 2) {
					receiverArrPt =
						MCR_MAP_VALUEOF(*mapHolderArray
						[2], itPt);
					localOnComplete(receiverArrPt);
				} else {
					/* Move to next mapping */
					mapHolderArray[i + 1] =
						MCR_MAP_VALUEOF(*mapHolderArray
						[i], itPt);
					++i;
					continue;
				}
			}
		}

		/* This level is handled, go back to previous */
		specHandledHolderArr[i] = false;
		genHandledHolderArr[i] = false;
		/* If current is index 0, then we are done */
		if (!i--)
			break;
	} while (true);
	return isToBlock;
#undef localOnComplete
}

void mcr_DispatchMap_clear(struct mcr_DispatchMap *dmPt)
{
	dassert(dmPt);
	mcr_Map_clear(&dmPt->map);
}

void mcr_DispatchMap_trim(struct mcr_DispatchMap *dmPt)
{
	dassert(dmPt);
	mcr_Map_trim(&dmPt->map);
}

static void map_set_key_iface(struct mcr_Map *mapPt,
	struct mcr_Interface *keyIface, bool flagFinalMap)
{
	if (flagFinalMap)
		mcr_Map_set_all(mapPt, 0, sizeof(struct mcr_Array),
			NULL, keyIface, mcr_Array_DispatchPair_iface());
	else
		mcr_Map_set_all(mapPt, 0, sizeof(struct mcr_Map),
			NULL, keyIface, NULL);
}

static void map_remove(struct mcr_Map *mapPt, void *ref,
	unsigned int curDepth, unsigned int maxDepth)
{
	struct mcr_Array *arrPt;
	char *itPt, *end;
	size_t bytes;
	MCR_MAP_ITER(*mapPt, itPt, end, bytes);
	if (!itPt)
		return;
	itPt += mapPt->key_size;
	if (curDepth == maxDepth) {
		while (itPt < end) {
			arrPt = (struct mcr_Array *)(itPt);
			mcr_Array_remove(arrPt, &ref);
			itPt += bytes;
		}
	} else {
		while (itPt < end) {
			map_remove((struct mcr_Map *)(itPt), ref,
				curDepth + 1, maxDepth);
			itPt += bytes;
		}
	}
}

static void map_remove_empties(struct mcr_Map *mapPt, bool trimFlag,
	unsigned int curDepth, unsigned int maxDepth)
{
	size_t i;
	struct mcr_Map *itPt;
	struct mcr_Array *arrPt;
	unsigned int nextDepth = curDepth + 1;
	/* On final map, entries are arrays */
	if (curDepth == maxDepth) {
		arrPt = MCR_ARR_LAST(mapPt->set);
		arrPt = MCR_MAP_VALUEOF(*mapPt, arrPt);
		for (i = mapPt->set.used; i--;
			arrPt = MCR_ARR_PREV(mapPt->set, arrPt)) {
			if (arrPt->used) {
				if (trimFlag)
					mcr_Array_trim(arrPt);
			} else {
				mcr_Array_free(arrPt);
				mcr_Array_remove_index(&mapPt->set, i, 1);
			}
		}
	} else {
		itPt = MCR_ARR_LAST(mapPt->set);
		itPt = MCR_MAP_VALUEOF(*mapPt, itPt);
		for (i = mapPt->set.used; i--;
			itPt = MCR_ARR_PREV(mapPt->set, itPt)) {
			map_remove_empties(itPt, trimFlag, nextDepth, maxDepth);
			if (itPt->set.used) {
				if (trimFlag)
					mcr_Map_trim(itPt);
			} else {
				mcr_Map_free(itPt);
				mcr_Array_remove_index(&mapPt->set, i, 1);
			}
		}
	}
	if (trimFlag)
		mcr_Map_trim(mapPt);
	mcr_Map_sort(mapPt);
}

bool mcr_DispatchMap_map_dispatch(struct mcr_Map *mapPt,
	struct mcr_Signal *sigDispatch, unsigned int mods, void **keyPtArr,
	size_t keyCount)
{
	size_t i = 0;
	char *it, *end;
	struct mcr_Array *arrPt;
	struct mcr_DispatchPair *receiver;
	while (i < keyCount) {
		mapPt = mcr_Map_value(mapPt, keyPtArr[i++]);
		if (!mapPt)
			return 0;
	}
	/* After all keys are used, we are mapped into the receivers array */
	arrPt = (struct mcr_Array *)mapPt;
	MCR_ARR_ITER(*arrPt, it, end, i);
	dassert(i == sizeof(struct mcr_DispatchPair));
	if (!it)
		return 0;
	while (it < end) {
		receiver = (struct mcr_DispatchPair *)it;
		if (receiver->dispatch(receiver->receiver, sigDispatch, mods))
			return 1;
		it += sizeof(struct mcr_DispatchPair);
	}
	return 0;
}
