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

#include "mcr/standard/standard.h"
#include "mcr/standard/private.h"
#include MCR_STANDARD_NATIVE_INC
#include "mcr/modules.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int mcr_Key_Dispatcher_add(void *dispDataPt, struct mcr_Signal *signalPt,
	void *newTrigger, mcr_Dispatcher_receive_fnc receiveFnc);
static void mcr_Key_Dispatcher_clear(void *dispDataPt);
static bool mcr_Key_Dispatcher_dispatch(void *dispDataPt,
	struct mcr_Signal *signalPt, unsigned int mods);
static void mcr_Key_Dispatcher_free(void *dispDataPt);
static void mcr_Key_Dispatcher_modifier(void *dispDataPt,
	struct mcr_Signal *signalPt, unsigned int *modsPt);
static void mcr_Key_Dispatcher_remove(void *dispDataPt, void *delTrigger);
static void mcr_Key_Dispatcher_trim(void *dispDataPt);

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
		mcr_String_free(&ctx->standard.key_name_any);
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
	int ret;
	if (keyCode == newCode)
		return 0;
	ret = mcr_Key_set_name(ctx, newCode, mcr_Key_name(ctx, keyCode));
	if (!ret)
		return mcr_Key_set_name(ctx, keyCode, NULL);
	return ret;
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
		mcr_BiMap_first(&ctx->standard.key_modifiers, &key);
	return found ? *found : MCR_MOD_ANY;
}

int mcr_Key_mod_key(struct mcr_context *ctx, unsigned int modifier)
{
	int *found = mcr_BiMap_second(&ctx->standard.key_modifiers, &modifier);
	return found ? *found : MCR_KEY_ANY;
}

size_t mcr_Key_mod_count(struct mcr_context * ctx)
{
	return ctx->standard.key_modifiers.first.set.used;
}

void mcr_Key_mod_all(struct mcr_context *ctx, unsigned int *modBuffer,
	size_t bufferLength)
{
	size_t i = ctx->standard.key_modifiers.first.set.used;
	unsigned int *modPt;
	if (!modBuffer)
		return;
	if (bufferLength < i)
		i = bufferLength;
	while (i--) {
		modPt = MCR_ARR_ELEMENT(ctx->standard.key_modifiers.first.set,
			i);
		modBuffer[i] = *modPt;
	}
}

void mcr_Key_mod_clear(struct mcr_context *ctx)
{
	mcr_Map_clear(&ctx->standard.key_modifiers.first);
	mcr_Map_clear(&ctx->standard.key_modifiers.second);
}

int mcr_Key_mod_set_key(struct mcr_context *ctx, unsigned int modifiers,
	int key)
{
	return mcr_BiMap_map(&ctx->standard.key_modifiers, &modifiers, NULL,
		0, &key, NULL, 0);
}

int mcr_Key_mod_add(struct mcr_context *ctx, unsigned int modifiers, int key)
{
	return mcr_BiMap_addsecond(&ctx->standard.key_modifiers, &key,
		&modifiers, 1);
}

int mcr_Key_mod_map(struct mcr_context *ctx, unsigned int modifiers,
	int key, const int *addKeys, size_t bufferLen)
{
	return mcr_BiMap_map(&ctx->standard.key_modifiers, &modifiers, NULL,
		0, &key, addKeys, bufferLen);
}

int mcr_Key_mod_rekey(struct mcr_context *ctx, int oldKey, int newKey)
{
	unsigned int *modPt =
		mcr_BiMap_first(&ctx->standard.key_modifiers, &oldKey);
	if (modPt) {
		mcr_BiMap_remsecond(&ctx->standard.key_modifiers, &oldKey, 0);
		return mcr_Key_mod_set_key(ctx, *modPt, newKey);
	}
	return 0;
}

int mcr_Key_mod_remod(struct mcr_context *ctx, unsigned int modifiers,
	unsigned int newMods)
{
	int *found = mcr_BiMap_second(&ctx->standard.key_modifiers, &modifiers);
	if (found) {
		mcr_BiMap_remfirst(&ctx->standard.key_modifiers, &modifiers, 0);
		return mcr_Key_mod_set_key(ctx, newMods, *found);
	}
	return 0;
}

void mcr_Key_mod_trim(struct mcr_context *ctx)
{
	mcr_Map_trim(&ctx->standard.key_modifiers.first);
	mcr_Map_trim(&ctx->standard.key_modifiers.second);
}

#define keyVals(keyPt, keyOut, scanOut, upOut) \
keyOut = MCR_KEY_KEY(*keyPt); \
scanOut = MCR_KEY_SCAN(*keyPt); \
upOut = MCR_KEY_UP_TYPE(*keyPt);

/* Key */
static int mcr_Key_Dispatcher_add(void *dispDataPt, struct mcr_Signal *signalPt,
	void *newTrigger, mcr_Dispatcher_receive_fnc receiveFnc)
{
	dassert(newTrigger);
	struct mcr_DispatchMap *dispMapPt =
		&((struct mcr_CtxDispatcher *)dispDataPt)->dispatcher.map;
	int keys[] = {
		MCR_KEY_ANY, MCR_KEY_ANY, MCR_BOTH
	};
	struct mcr_Key *keyPt = mcr_Key_data(signalPt);
	void *keyPtArr[] = {
		keys + 0, keys + 1, keys + 2
	};
	if (keyPt) {
		keyVals(keyPt, keys[0], keys[1], keys[2]);
	}
	return mcr_DispatchMap_add(dispMapPt, keyPtArr, newTrigger, receiveFnc);
}

static void mcr_Key_Dispatcher_clear(void *dispDataPt)
{
	mcr_DispatchMap_clear(&((struct mcr_CtxDispatcher *)
			dispDataPt)->dispatcher.map);
}

static const int _genKeys[] = { MCR_KEY_ANY, MCR_KEY_ANY, MCR_BOTH };
static const void *_genKeyPtArr[] =
	{ _genKeys + 0, _genKeys + 1, _genKeys + 2 };
static bool mcr_Key_Dispatcher_dispatch(void *dispDataPt,
	struct mcr_Signal *signalPt, unsigned int mods)
{
	struct mcr_DispatchMap *dispMapPt =
		&((struct mcr_CtxDispatcher *)dispDataPt)->dispatcher.map;
	struct mcr_Key *keyPt = mcr_Key_data(signalPt);
	int keys[] = {
		MCR_KEY_ANY, MCR_KEY_ANY, MCR_BOTH
	};
	const void *keyPtArr[] = {
		keys + 0, keys + 1, keys + 2
	};
	struct mcr_Map *maps[] = { &dispMapPt->map, NULL, NULL };
	bool isGen[] = { false, false, false };
	bool isSpecHandled[3];
	bool isGenHandled[3];
	dassert(dispDataPt);
	dassert(signalPt);
	if (keyPt) {
		keyVals(keyPt, keys[0], keys[1], keys[2]);
		if (keys[0] != MCR_KEY_ANY)
			isGen[0] = false;
		if (keys[1] != MCR_KEY_ANY)
			isGen[1] = false;
		if (keys[2] != MCR_BOTH)
			isGen[2] = false;
	}
	return mcr_DispatchMap_dispatch(dispMapPt, signalPt, mods, keyPtArr,
		_genKeyPtArr, maps, isSpecHandled, isGenHandled, isGen);
}

static void mcr_Key_Dispatcher_free(void *dispDataPt)
{
	mcr_DispatchMap_free(&((struct mcr_CtxDispatcher *)
			dispDataPt)->dispatcher.map);
}

static void mcr_Key_Dispatcher_modifier(void *dispDataPt,
	struct mcr_Signal *sigPt, unsigned int *modsPt)
{
	dassert(dispDataPt);
	dassert(modsPt);
	struct mcr_context *ctx =
		((struct mcr_CtxDispatcher *)dispDataPt)->mcr_pt;
	struct mcr_Key *keyPt = mcr_Key_data(sigPt);
	int key;
	unsigned int *found;
	if (keyPt) {
		key = MCR_KEY_KEY(*keyPt);
		found = mcr_BiMap_first(&ctx->standard.key_modifiers, &key);
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

static void mcr_Key_Dispatcher_remove(void *dispDataPt, void *delTrigger)
{
	dassert(delTrigger);
	mcr_DispatchMap_remove(&((struct mcr_CtxDispatcher *)
			dispDataPt)->dispatcher.map, delTrigger);
}

static void mcr_Key_Dispatcher_trim(void *dispDataPt)
{
	mcr_DispatchMap_remove_empties(&((struct mcr_CtxDispatcher *)
			dispDataPt)->dispatcher.map, true);
}

/* Library initializer and cleanup. */
int mcr_Key_initialize(struct mcr_context *ctx)
{
	struct mcr_CtxDispatcher *dispPt = &ctx->standard.key_dispatcher;
	struct mcr_DispatchMap *mapPt = &dispPt->dispatcher.map;
	struct mcr_Interface keyTriggers[3];
	size_t i = 3;
	int err = 0;
	mcr_BiMap_init(&ctx->standard.key_modifiers);
	mcr_BiMap_set_all(&ctx->standard.key_modifiers, sizeof(unsigned int),
		sizeof(int), mcr_unsigned_compare, mcr_int_compare, NULL, NULL);
	while (i--) {
		mcr_iinit(keyTriggers + i);
		mcr_iset_all(keyTriggers + i, mcr_int_compare, NULL,
			sizeof(int), NULL, NULL);
	}
	mcr_Dispatcher_set_all((struct mcr_Dispatcher *)dispPt,
		mcr_Key_Dispatcher_add, mcr_Key_Dispatcher_clear,
		mcr_Key_Dispatcher_dispatch, mcr_Key_Dispatcher_free,
		mcr_Key_Dispatcher_modifier, mcr_Key_Dispatcher_remove,
		mcr_Key_Dispatcher_trim);
	mcr_DispatchMap_init(mapPt);
	dispPt->mcr_pt = ctx;
	if ((err = mcr_DispatchMap_set_all(mapPt, keyTriggers, 3)))
		return err;
	mcr_StringIndex_init(&ctx->standard.key_name_index);
	mcr_String_init(&ctx->standard.key_name_any);
	if ((err = mcr_String_replace(&ctx->standard.key_name_any, "Any")))
		return err;
	return err;
}

void mcr_Key_cleanup(struct mcr_context *ctx)
{
	struct mcr_CtxDispatcher *dispPt = &ctx->standard.key_dispatcher;
	mcr_StringIndex_free(&ctx->standard.key_name_index);
	mcr_String_free(&ctx->standard.key_name_any);
	mcr_DispatchMap_free(&dispPt->dispatcher);
	mcr_BiMap_free(&ctx->standard.key_modifiers);
}

struct mcr_ISignal *mcr_iKey(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.ikey;
}
