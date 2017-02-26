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

#include "mcr/standard/standard.h"
#include "mcr/standard/private.h"
#include MCR_STANDARD_NATIVE_INC
#include "mcr/modules.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int mcr_Key_gen_key = MCR_KEY_ANY;
const unsigned int mcr_Key_gen_up = MCR_BOTH;
static int mcr_Key_init_scan_map(void *mapPt);
static int mcr_Key_Dispatcher_add(void *dispDataPt, struct mcr_Signal *signalPt,
	void *newTrigger, mcr_Dispatcher_receive_fnc receiveFnc);
static int mcr_Key_Dispatcher_clear(void *dispDataPt);
static bool mcr_Key_Dispatcher_dispatch(void *dispDataPt,
	struct mcr_Signal *signalPt, unsigned int mods);
static int mcr_Key_Dispatcher_deinit(void *dispDataPt);
static void mcr_Key_Dispatcher_modifier(void *dispDataPt,
	struct mcr_Signal *signalPt, unsigned int *modsPt);
static int mcr_Key_Dispatcher_remove(void *dispDataPt, void *delTrigger);
static int mcr_Key_Dispatcher_trim(void *dispDataPt);

void mcr_Key_set_all(struct mcr_Key *keyPt, int key, int scan,
	enum mcr_KeyUpType keyUp)
{
	dassert(keyPt);
	MCR_KEY_SET_ALL(*keyPt, key == -1 ? MCR_KEY_ANY : key, scan, keyUp);
}

int mcr_Key_key(const struct mcr_Key *keyPt)
{
	dassert(keyPt);
	return MCR_KEY_KEY(*keyPt);
}

void mcr_Key_set_key(struct mcr_Key *keyPt, int key)
{
	dassert(keyPt);
	MCR_KEY_SET_KEY(*keyPt, key == -1 ? MCR_KEY_ANY : key);
}

int mcr_Key_scan(const struct mcr_Key *keyPt)
{
	dassert(keyPt);
	return MCR_KEY_SCAN(*keyPt);
}

void mcr_Key_set_scan(struct mcr_Key *keyPt, int scan)
{
	dassert(keyPt);
	MCR_KEY_SET_SCAN(*keyPt, scan);
}

enum mcr_KeyUpType mcr_Key_up_type(const struct mcr_Key *keyPt)
{
	dassert(keyPt);
	return MCR_KEY_UP_TYPE(*keyPt);
}

void mcr_Key_set_up_type(struct mcr_Key *keyPt, const enum mcr_KeyUpType keyUp)
{
	dassert(keyPt);
	MCR_KEY_SET_UP_TYPE(*keyPt, keyUp);
}

int mcr_Key_send(struct mcr_Signal *signalData)
{
	dassert(signalData);
	struct mcr_Key *keyPt = mcr_Key_data(signalData);
	return keyPt ? mcr_Key_send_data(keyPt) : 0;
}

int mcr_Key_compare(const void *lhs, const void *rhs)
{
	int l, r, ret;
	const struct mcr_Key *lPt = lhs, *rPt = rhs;
	if (rhs) {
		if (lhs) {
			l = MCR_KEY_KEY(*lPt);
			r = MCR_KEY_KEY(*rPt);
			if ((ret = MCR_CMP_INTEGRAL(l, r)))
				return ret;
			l = MCR_KEY_SCAN(*lPt);
			r = MCR_KEY_SCAN(*rPt);
			if ((ret = MCR_CMP_INTEGRAL(l, r)))
				return ret;
			l = MCR_KEY_UP_TYPE(*lPt);
			r = MCR_KEY_UP_TYPE(*rPt);
			return MCR_CMP_INTEGRAL(l, r);
		}
		return -1;
	}
	return ! !lhs;
}

int mcr_Key_copy(void *dstPt, void *srcPt)
{
	dassert(dstPt);
	struct mcr_Key *dPt = dstPt, *sPt = srcPt;
	if (sPt) {
		MCR_KEY_SET_KEY(*dPt, MCR_KEY_KEY(*sPt));
		MCR_KEY_SET_SCAN(*dPt, MCR_KEY_SCAN(*sPt));
		MCR_KEY_SET_UP_TYPE(*dPt, MCR_KEY_UP_TYPE(*sPt));
	} else {
		MCR_KEY_SET_KEY(*dPt, MCR_KEY_ANY);
		MCR_KEY_SET_SCAN(*dPt, 0);
		MCR_KEY_SET_UP_TYPE(*dPt, MCR_BOTH);
	}
	return 0;
}

int mcr_Key_name_key(struct mcr_context *ctx, const char *keyName)
{
	if (!keyName)
		return MCR_KEY_ANY;
	int ret = (int)mcr_StringIndex_index(&ctx->standard.key_name_index,
		keyName);
	return ret == -1 ? MCR_KEY_ANY : ret;
}

const char *mcr_Key_name(struct mcr_context *ctx, int keyCode)
{
	const char *ret;
	if (keyCode == MCR_KEY_ANY)
		return ctx->standard.key_name_any.array;
	ret = mcr_StringIndex_name(&ctx->standard.key_name_index, keyCode);
	return ret ? ret : ctx->standard.key_name_any.array;
}

size_t mcr_Key_count(struct mcr_context * ctx)
{
	return ctx->standard.key_name_index.set.used;
}

int mcr_Key_set_name(struct mcr_context *ctx, int keyCode, const char *newName)
{
	if (keyCode == MCR_KEY_ANY) {
		if (newName)
			return mcr_String_replace(&ctx->standard.key_name_any,
				newName);
		mcr_String_deinit(&ctx->standard.key_name_any);
		return 0;
	}
	if (newName)
		return mcr_StringIndex_map(&ctx->standard.key_name_index,
			keyCode, newName, NULL, 0);
	mcr_StringIndex_unmap(&ctx->standard.key_name_index, keyCode, 1);
	return 0;
}

int mcr_Key_add(struct mcr_context *ctx, int keyCode, const char **addNames,
	size_t bufferLen)
{
	return mcr_StringIndex_add(&ctx->standard.key_name_index, keyCode,
		addNames, bufferLen);
}

int mcr_Key_map(struct mcr_context *ctx, int keyCode,
	const char *newName, const char **addNames, size_t bufferLen)
{
	if (keyCode == MCR_KEY_ANY) {
		return mcr_Key_set_name(ctx, keyCode, newName) &&
			mcr_Key_add(ctx, keyCode, addNames, bufferLen);
	}
	return mcr_StringIndex_map(&ctx->standard.key_name_index, keyCode,
		newName, addNames, bufferLen);
}

int mcr_Key_rekey(struct mcr_context *ctx, int keyCode, int newCode)
{
	int err;
	if (keyCode == newCode)
		return 0;
	err = mcr_Key_set_name(ctx, newCode, mcr_Key_name(ctx, keyCode));
	if (!err)
		return mcr_Key_set_name(ctx, keyCode, NULL);
	return err;
}

int mcr_Key_rename(struct mcr_context *ctx, const char *oldName,
	const char *newName)
{
	return mcr_StringIndex_remap(&ctx->standard.key_name_index, oldName,
		newName);
}

void mcr_Key_trim(struct mcr_context *ctx)
{
	mcr_StringIndex_trim(&ctx->standard.key_name_index);
}

void mcr_Key_clear(struct mcr_context *ctx)
{
	mcr_StringIndex_clear(&ctx->standard.key_name_index);
}

/* Modifier mapping */
unsigned int mcr_Key_mod(struct mcr_context *ctx, int key)
{
	unsigned int *found =
		mcr_Map_value(&ctx->standard.map_key_modifier, &key);
	return found ? *found : MCR_MOD_ANY;
}

int mcr_Key_mod_key(struct mcr_context *ctx, unsigned int modifier)
{
	int *found = mcr_Map_value(&ctx->standard.map_modifier_key, &modifier);
	return found ? *found : MCR_KEY_ANY;
}

size_t mcr_Key_mod_count(struct mcr_context * ctx)
{
	return ctx->standard.map_key_modifier.set.used;
}

void mcr_Key_mod_all(struct mcr_context *ctx, unsigned int *modBuffer,
	size_t bufferLength)
{
	size_t i = ctx->standard.map_key_modifier.set.used;
	unsigned int *modPt;
	if (!modBuffer)
		return;
	if (bufferLength < i)
		i = bufferLength;
	while (i--) {
		modPt = MCR_ARR_ELEMENT(ctx->standard.map_key_modifier.set, i);
		modBuffer[i] = *modPt;
	}
}

void mcr_Key_mod_clear(struct mcr_context *ctx)
{
	mcr_Map_clear(&ctx->standard.map_key_modifier);
	mcr_Map_clear(&ctx->standard.map_modifier_key);
}

int mcr_Key_mod_set_key(struct mcr_context *ctx, unsigned int modifiers,
	int key)
{
	int err =
		mcr_Map_map(&ctx->standard.map_key_modifier, &key, &modifiers);
	if (!err)
		err = mcr_Map_map(&ctx->standard.map_modifier_key, &modifiers,
			&key);
	return err;
}

int mcr_Key_mod_add(struct mcr_context *ctx, unsigned int modifiers, int key)
{
	return mcr_Map_map(&ctx->standard.map_modifier_key, &modifiers, &key);
}

int mcr_Key_mod_map(struct mcr_context *ctx, unsigned int modifiers,
	int key, int *addKeys, size_t bufferLen)
{
	int err = mcr_Key_mod_set_key(ctx, modifiers, key);
	if (!err)
		err = mcr_Map_fill(&ctx->standard.map_key_modifier, addKeys,
			bufferLen, &modifiers);
	return err;
}

int mcr_Key_mod_rekey(struct mcr_context *ctx, int oldKey, int newKey)
{
	int err;
	unsigned int *modPt =
		mcr_Map_value(&ctx->standard.map_key_modifier, &oldKey);
	if (modPt) {
		if ((err = mcr_Map_map(&ctx->standard.map_modifier_key, modPt,
					&newKey)))
			return err;
		return mcr_Map_remap(&ctx->standard.map_key_modifier, &oldKey,
			&newKey);
	}
	return 0;
}

int mcr_Key_mod_remod(struct mcr_context *ctx, unsigned int modifiers,
	unsigned int newMods)
{
	int err;
	int *found = mcr_Map_value(&ctx->standard.map_modifier_key, &modifiers);
	if (found) {
		if ((err = mcr_Map_map(&ctx->standard.map_key_modifier, found,
					&newMods)))
			return err;
		return mcr_Map_remap(&ctx->standard.map_modifier_key,
			&modifiers, &newMods);
	}
	return 0;
}

void mcr_Key_mod_trim(struct mcr_context *ctx)
{
	mcr_Map_trim(&ctx->standard.map_key_modifier);
	mcr_Map_trim(&ctx->standard.map_modifier_key);
}

static int mcr_Key_Dispatcher_add_keys(struct mcr_Map *keyMap, int key,
	int scan, void *newTrigger, mcr_Dispatcher_receive_fnc receiveFnc)
{
	/* first map int => map, second unsigned => dispatch array */
	struct mcr_Map *scanMapPt;
	struct mcr_Array *arrPt;
	struct mcr_DispatchPair dispPair = { newTrigger, receiveFnc };
	scanMapPt = mcr_Map_element_ensured(keyMap, &key);
	if (!scanMapPt)
		return mcr_error();	// Expect ENOMEM
	scanMapPt = MCR_MAP_VALUEOF(*keyMap, scanMapPt);
	arrPt = mcr_Map_element_ensured(scanMapPt, &scan);
	if (!arrPt)
		return mcr_error();	// Expect ENOMEM
	if ((arrPt = MCR_MAP_VALUEOF(*scanMapPt, arrPt))) {
		/* Multiple object references may exist */
		return mcr_Array_add(arrPt, &dispPair, 1, false);
	}
	return mcr_error();	// Expect ENOMEM
}

/* Key */
static int mcr_Key_Dispatcher_add(void *dispDataPt, struct mcr_Signal *signalPt,
	void *newTrigger, mcr_Dispatcher_receive_fnc receiveFnc)
{
	struct mcr_CtxDispatcher *dispPt = dispDataPt;
	struct mcr_Key *keyPt = mcr_Key_data(signalPt);
	unsigned int upType;
	int err;
	struct mcr_Map *dispMaps = dispPt->ctx->standard.key_dispatcher_maps;
	/* mcr_Trigger_receive requires an object */
	dassert(newTrigger || receiveFnc);
	if (!receiveFnc)
		receiveFnc = mcr_Trigger_receive;
	if (keyPt) {
		upType = MCR_KEY_UP_TYPE(*keyPt);
		if (upType < MCR_BOTH)
			return mcr_Key_Dispatcher_add_keys(dispMaps + upType,
				MCR_KEY_KEY(*keyPt), MCR_KEY_SCAN(*keyPt),
				newTrigger, receiveFnc);
		/* Generic up, add to both */
		if ((err = mcr_Key_Dispatcher_add_keys(dispMaps,
					MCR_KEY_KEY(*keyPt),
					MCR_KEY_SCAN(*keyPt), newTrigger,
					receiveFnc)))
			return err;
		return mcr_Key_Dispatcher_add_keys(dispMaps + 1,
			MCR_KEY_KEY(*keyPt), MCR_KEY_SCAN(*keyPt), newTrigger,
			receiveFnc);
	}
	/* Generic up, add to both */
	if ((err = mcr_Key_Dispatcher_add_keys(dispMaps, MCR_KEY_ANY,
				MCR_KEY_ANY, newTrigger, receiveFnc)))
		return err;
	return mcr_Key_Dispatcher_add_keys(dispMaps + 1, MCR_KEY_ANY,
		MCR_KEY_ANY, newTrigger, receiveFnc);
}

static int mcr_Key_Dispatcher_clear(void *dispDataPt)
{
	struct mcr_CtxDispatcher *dispPt = dispDataPt;
	struct mcr_Map *maps = dispPt->ctx->standard.key_dispatcher_maps;
	/* Down, up, and generic */
	mcr_Map_clear(maps++);
	mcr_Map_clear(maps++);
	mcr_Map_clear(maps);
	return 0;
}

/* Optimize this function as much as possible, TODO: iterator instead of
 * "for each" */
static bool mcr_Key_Dispatcher_dispatch(void *dispDataPt,
	struct mcr_Signal *signalPt, unsigned int mods)
{
#define localDispAll(itPt) \
	if (((struct mcr_DispatchPair *)itPt)->dispatch(*(void **)itPt, signalPt, mods)) \
		return true;
#define localDispArrPt \
	if (arrPt) { \
		MCR_ARR_FOR_EACH(*arrPt, localDispAll); \
	}
#define localDispScanMap(mapPt, scanKey) \
	if (mapPt) { \
		if (scanKey != MCR_KEY_ANY) { \
			arrPt = mcr_Map_value(mapPt, \
				&scanKey); \
			localDispArrPt; \
		} \
		arrPt = mcr_Map_value(mapPt, \
			&mcr_Key_gen_key); \
		localDispArrPt; \
	}

	struct mcr_CtxDispatcher *dispMapPt = dispDataPt;
	struct mcr_Key *keyPt = mcr_Key_data(signalPt);
	struct mcr_Map *maps = dispMapPt->ctx->standard.key_dispatcher_maps;
	struct mcr_Map *scanMapPt;
	struct mcr_Array *arrPt;
	int key;
	unsigned int scan, upType;
	dassert(dispDataPt);
	if (keyPt) {
		key = MCR_KEY_KEY(*keyPt);
		scan = MCR_KEY_SCAN(*keyPt);
		upType = MCR_KEY_UP_TYPE(*keyPt);
		if (upType < MCR_BOTH) {
			/* Specific up type */
			if (key != MCR_KEY_ANY) {
				/* specific: up, key, spec+gen: scan */
				scanMapPt = mcr_Map_value(maps + upType, &key);
				localDispScanMap(scanMapPt, scan);
			}
			/* specific: up, gen: key, spec+gen: scan */
			scanMapPt =
				mcr_Map_value(maps + upType, &mcr_Key_gen_key);
			localDispScanMap(scanMapPt, scan);
		} else {
			/* Generic up type */
			if (key != MCR_KEY_ANY) {
				/* specific: key, spec+gen: scan */
				/* down */
				scanMapPt = mcr_Map_value(maps, &key);
				localDispScanMap(scanMapPt, scan);
				/* and up */
				scanMapPt = mcr_Map_value(maps + 1, &key);
				localDispScanMap(scanMapPt, scan);
			}
			/* gen: key, spec+gen: scan */
			/* down */
			scanMapPt = mcr_Map_value(maps, &mcr_Key_gen_key);
			localDispScanMap(scanMapPt, scan);
			/* and up */
			scanMapPt = mcr_Map_value(maps + 1, &mcr_Key_gen_key);
			localDispScanMap(scanMapPt, scan);
			return false;
		}
	}
	/* all generic, TODO: Generic receivers will receive twice */
	scanMapPt = mcr_Map_value(maps, &mcr_Key_gen_key);
	if (scanMapPt) {
		arrPt = mcr_Map_value(scanMapPt, &mcr_Key_gen_key);
		localDispArrPt;
	}
	scanMapPt = mcr_Map_value(maps + 1, &mcr_Key_gen_key);
	if (scanMapPt) {
		arrPt = mcr_Map_value(scanMapPt, &mcr_Key_gen_key);
		localDispArrPt;
	}
	return false;
#undef localDispScanMap
#undef localDispArrPt
#undef localDispAll
}

static int mcr_Key_Dispatcher_deinit(void *dispDataPt)
{
	struct mcr_CtxDispatcher *dispPt = dispDataPt;
	struct mcr_Map *maps = dispPt->ctx->standard.key_dispatcher_maps;
	/* Down, up, and generic */
	mcr_Map_deinit(maps++);
	mcr_Map_deinit(maps++);
	mcr_Map_deinit(maps);
	return 0;
}

static void mcr_Key_Dispatcher_modifier(void *dispDataPt,
	struct mcr_Signal *sigPt, unsigned int *modsPt)
{
	dassert(dispDataPt);
	dassert(modsPt);
	struct mcr_context *ctx = ((struct mcr_CtxDispatcher *)dispDataPt)->ctx;
	struct mcr_Key *keyPt = mcr_Key_data(sigPt);
	int key;
	unsigned int *found;
	if (keyPt) {
		key = MCR_KEY_KEY(*keyPt);
		found = mcr_Map_value(&ctx->standard.map_key_modifier, &key);
		if (found) {
			switch (MCR_KEY_UP_TYPE(*keyPt)) {
			case MCR_DOWN:
				*modsPt |= *found;
				break;
			case MCR_UP:
			case MCR_BOTH:
				*modsPt &= ~(*found);
				break;
			case MCR_TOGGLE:
				if ((*modsPt & *found) == *found)
					*modsPt &= ~(*found);
				else
					*modsPt |= *found;
				break;
			default:
				mset_error(EINVAL);
				break;
			}
		}
	}
}

static void mcr_Key_Dispatcher_scan_map_remove(struct mcr_Map *scanMap,
	void *delTrigger)
{
	struct mcr_Array *arrPt;
#define localRemove(itPt) \
	arrPt = (struct mcr_Array *)itPt; \
	mcr_Array_remove(arrPt, &delTrigger);
	MCR_MAP_FOR_EACH_VALUE(*scanMap, localRemove);
#undef localRemove
}

static int mcr_Key_Dispatcher_remove(void *dispDataPt, void *delTrigger)
{
#define localRemove(itPt) \
	mcr_Key_Dispatcher_scan_map_remove((struct mcr_Map *)itPt, delTrigger)
	struct mcr_CtxDispatcher *dispPt = dispDataPt;
	struct mcr_Map *maps = dispPt->ctx->standard.key_dispatcher_maps;
	MCR_MAP_FOR_EACH_VALUE(maps[0], localRemove);
	MCR_MAP_FOR_EACH_VALUE(maps[1], localRemove);
	return 0;
#undef localRemove
}

static void mcr_Key_Dispatcher_remove_empties(struct mcr_Map *mapPt)
{
	char *firstPt, *itPt;
	size_t bytes, valSize = mapPt->value_size;
	struct mcr_Array *arrPt;
	struct mcr_Map *mapElement;
	mcr_Map_iter_range(mapPt, &firstPt, &itPt, &bytes, 0, mapPt->set.used);
	if (firstPt && itPt) {
		while (itPt >= firstPt) {
			if (valSize == sizeof(struct mcr_Map)) {
				/* Values are maps, remove their mapped empties */
				mapElement = MCR_MAP_VALUEOF(*mapPt, itPt);
				mcr_Key_Dispatcher_remove_empties(mapElement);
				if (!mapElement->set.used)
					mcr_Map_unmap(mapPt, itPt);
			} else {
				arrPt = MCR_MAP_VALUEOF(*mapPt, itPt);
				if (!arrPt->used)
					mcr_Map_unmap(mapPt, itPt);
			}
			itPt -= bytes;
		}
	}
}

static int mcr_Key_Dispatcher_trim(void *dispDataPt)
{
	struct mcr_CtxDispatcher *dispPt = dispDataPt;
	struct mcr_Map *maps = dispPt->ctx->standard.key_dispatcher_maps;
	mcr_Key_Dispatcher_remove_empties(maps);
	mcr_Key_Dispatcher_remove_empties(maps + 1);
	return 0;
}

static int mcr_Key_init_scan_map(void *mapPt)
{
	if (mapPt) {
		mcr_Map_init(mapPt);
		mcr_Map_set_all(mapPt, sizeof(unsigned int), 0,
			mcr_unsigned_compare, NULL,
			mcr_Array_DispatchPair_interface());
	}
	return 0;
}

/* Library initializer and deinitialize. */
int mcr_Key_initialize(struct mcr_context *ctx)
{
	struct mcr_mod_standard *standard = &ctx->standard;
	int err = 0;
	mcr_Dispatcher_set_all(&standard->key_dispatcher.dispatcher,
		mcr_Key_Dispatcher_add, mcr_Key_Dispatcher_clear,
		mcr_Key_Dispatcher_dispatch, mcr_Key_Dispatcher_deinit,
		mcr_Key_Dispatcher_modifier, mcr_Key_Dispatcher_remove,
		mcr_Key_Dispatcher_trim);
	standard->key_dispatcher.ctx = ctx;
	standard->scan_map_interface =
		mcr_Interface_new(sizeof(struct mcr_Map), mcr_Key_init_scan_map,
		mcr_Map_deinit, NULL, NULL);
	standard->key_dispatcher_maps[0] = standard->key_dispatcher_maps[1] =
		mcr_Map_new(sizeof(int), 0, mcr_int_compare, NULL,
		&standard->scan_map_interface);
	standard->map_key_modifier = mcr_Map_new(sizeof(int),
		sizeof(unsigned int), mcr_int_compare, NULL, NULL);
	standard->map_modifier_key = mcr_Map_new(sizeof(unsigned int),
		sizeof(int), mcr_unsigned_compare, NULL, NULL);
	if ((err = mcr_Dispatcher_register(ctx, &standard->key_dispatcher,
				mcr_iKey(ctx)->interface.id)))
		return err;
	mcr_StringIndex_init(&standard->key_name_index);
	mcr_String_init(&standard->key_name_any);
	if ((err = mcr_String_replace(&standard->key_name_any, "Any")))
		return err;
	return err;
}

int mcr_Key_deinitialize(struct mcr_context *ctx)
{
	int err;
	struct mcr_mod_standard *standard = &ctx->standard;
	mcr_StringIndex_deinit(&standard->key_name_index);
	mcr_String_deinit(&standard->key_name_any);
	if ((err = mcr_Map_deinit(standard->key_dispatcher_maps)))
		return err;
	if ((err = mcr_Map_deinit(standard->key_dispatcher_maps + 1)))
		return err;
	if ((err = mcr_Map_deinit(&standard->map_key_modifier)))
		return err;
	return mcr_Map_deinit(&standard->map_modifier_key);
}

struct mcr_ISignal *mcr_iKey(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.ikey;
}