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

#include "mcr/standard/standard.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mcr/libmacro.h"

extern const int mcr_Key_gen_key;
const int mcr_Key_gen_key = MCR_KEY_ANY;
extern const unsigned int mcr_Key_gen_up;
const unsigned int mcr_Key_gen_up = MCR_BOTH;

void mcr_Key_set_all(struct mcr_Key *keyPt, int key,
					 enum mcr_ApplyType applyKey)
{
	dassert(keyPt);
	keyPt->key = key;
	keyPt->apply = applyKey;
}

int mcr_Key_send(struct mcr_Signal *sigPt)
{
	dassert(sigPt);
	struct mcr_Key *keyPt = mcr_Key_data(sigPt);
	return keyPt ? mcr_Key_send_data(keyPt) : 0;
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
	ret = mcr_StringIndex_name(&ctx->standard.key_name_index, (size_t)keyCode);
	return ret ? ret : ctx->standard.key_name_any.array;
}

size_t mcr_Key_count(struct mcr_context * ctx)
{
	return ctx->standard.key_name_index.set.used;
}

int mcr_Key_set_name(struct mcr_context *ctx, int keyCode, const char *newName)
{
	if (keyCode == MCR_KEY_ANY) {
		if (newName) {
			return mcr_String_replace(&ctx->standard.key_name_any,
									  newName);
		}
		mcr_String_deinit(&ctx->standard.key_name_any);
		return 0;
	}
	if (newName) {
		return mcr_StringIndex_map(&ctx->standard.key_name_index,
								   (size_t)keyCode, newName);
	}
	mcr_StringIndex_unmap(&ctx->standard.key_name_index, (size_t)keyCode, 1);
	return 0;
}

int mcr_Key_add(struct mcr_context *ctx, int keyCode, const char **addNames,
				size_t bufferLen)
{
	return mcr_StringIndex_add(&ctx->standard.key_name_index, (size_t)keyCode,
							   addNames, bufferLen);
}

int mcr_Key_map(struct mcr_context *ctx, int keyCode,
				const char *newName, const char **addNames, size_t bufferLen)
{
	if (keyCode == MCR_KEY_ANY) {
		return mcr_Key_set_name(ctx, keyCode, newName) &&
			   mcr_Key_add(ctx, keyCode, addNames, bufferLen);
	}
	if (mcr_StringIndex_map(&ctx->standard.key_name_index, (size_t)keyCode,
							newName))
		return mcr_err;
	return mcr_StringIndex_add(&ctx->standard.key_name_index, (size_t)keyCode,
							   addNames, bufferLen);
}

int mcr_Key_rekey(struct mcr_context *ctx, int keyCode, int newCode)
{
	if (keyCode == newCode)
		return 0;
	if (mcr_Key_set_name(ctx, newCode, mcr_Key_name(ctx, keyCode)))
		return mcr_err;
	if (mcr_Key_set_name(ctx, keyCode, NULL))
		return mcr_err;
	return 0;
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
unsigned int mcr_Key_modifier(struct mcr_context *ctx, int key)
{
	unsigned int *found =
		mcr_Map_value(&ctx->standard.map_key_modifier, &key);
	return found ? *found : MCR_MF_NONE;
}

int mcr_Key_modifier_key(struct mcr_context *ctx, unsigned int modifier)
{
	int *found = mcr_Map_value(&ctx->standard.map_modifier_key, &modifier);
	return found ? *found : MCR_KEY_ANY;
}

size_t mcr_Key_modifier_count(struct mcr_context * ctx)
{
	return ctx->standard.map_key_modifier.set.used;
}

size_t mcr_Key_modifier_key_count(struct mcr_context * ctx)
{
	return ctx->standard.map_modifier_key.set.used;
}

void mcr_Key_modifier_all(struct mcr_context *ctx, unsigned int *modBuffer,
						  size_t bufferLength)
{
	size_t i = ctx->standard.map_modifier_key.set.used;
	unsigned int *modPt;
	if (!modBuffer)
		return;
	if (bufferLength < i)
		i = bufferLength;
	while (i--) {
		modPt = MCR_ARR_ELEMENT(ctx->standard.map_modifier_key.set, i);
		modBuffer[i] = *modPt;
	}
}

void mcr_Key_modifier_key_all(struct mcr_context *ctx, int *keyBuffer,
							  size_t bufferLength)
{
	size_t i = ctx->standard.map_key_modifier.set.used;
	int *keyPt;
	if (!keyBuffer)
		return;
	if (bufferLength < i)
		i = bufferLength;
	while (i--) {
		keyPt = MCR_ARR_ELEMENT(ctx->standard.map_key_modifier.set, i);
		keyBuffer[i] = *keyPt;
	}
}

void mcr_Key_modifier_clear(struct mcr_context *ctx)
{
	mcr_Map_clear(&ctx->standard.map_key_modifier);
	mcr_Map_clear(&ctx->standard.map_modifier_key);
}

int mcr_Key_modifier_set_key(struct mcr_context *ctx, unsigned int modifiers,
							 int key)
{
	if (mcr_Map_map(&ctx->standard.map_key_modifier, &key, &modifiers) ||
		mcr_Map_map(&ctx->standard.map_modifier_key, &modifiers,
					&key)) {
		return mcr_err;
	}
	return 0;
}

int mcr_Key_modifier_add(struct mcr_context *ctx, unsigned int modifiers,
						 int key)
{
	return mcr_Map_map(&ctx->standard.map_key_modifier, &key, &modifiers);
}

int mcr_Key_modifier_map(struct mcr_context *ctx, unsigned int modifiers,
						 int key, int *addKeys, size_t bufferLen)
{
	size_t i;
	if (bufferLen)
		dassert(addKeys);
	if (mcr_Key_modifier_set_key(ctx, modifiers, key))
		return mcr_err;
	for (i = 0; i < bufferLen; i++) {
		if (mcr_Key_modifier_add(ctx, modifiers, addKeys[i]))
			return mcr_err;
	}
	return 0;
}

int mcr_Key_modifier_rekey(struct mcr_context *ctx, int oldKey, int newKey)
{
	unsigned int *modPt =
		mcr_Map_value(&ctx->standard.map_key_modifier, &oldKey);
	if (modPt) {
		if (mcr_Map_map(&ctx->standard.map_modifier_key, modPt,
						&newKey)) {
			return mcr_err;
		}
		return mcr_Map_remap(&ctx->standard.map_key_modifier, &oldKey,
							 &newKey);
	}
	return 0;
}

int mcr_Key_modifier_remod(struct mcr_context *ctx, unsigned int modifiers,
						   unsigned int newMods)
{
	int *found = mcr_Map_value(&ctx->standard.map_modifier_key, &modifiers);
	if (found) {
		if (mcr_Map_map(&ctx->standard.map_key_modifier, found,
						&newMods)) {
			return mcr_err;
		}
		return mcr_Map_remap(&ctx->standard.map_modifier_key,
							 &modifiers, &newMods);
	}
	return 0;
}

void mcr_Key_modifier_trim(struct mcr_context *ctx)
{
	mcr_Map_trim(&ctx->standard.map_key_modifier);
	mcr_Map_trim(&ctx->standard.map_modifier_key);
}

static int mcr_Key_Dispatcher_add_keys(struct mcr_Map *keyMap, int key,
									   void *newTrigger, mcr_Dispatcher_receive_fnc receiveFnc)
{
	/* first map int => map, second unsigned => dispatch array */
	struct mcr_Array *arrPt;
	struct mcr_DispatchPair dispPair = { newTrigger, receiveFnc };
	arrPt = mcr_Map_element_ensured(keyMap, &key);
	if (!arrPt)
		return mcr_err;	// Expect ENOMEM
	arrPt = MCR_MAP_VALUEOF(*keyMap, arrPt);
	/* Multiple object references may exist */
	return mcr_Array_add(arrPt, &dispPair, 1, false);
}

/* Key */
int mcr_Key_Dispatcher_add(void *dispDataPt, struct mcr_Signal *signalPt,
						   void *newTrigger, mcr_Dispatcher_receive_fnc receiveFnc)
{
	struct mcr_CtxDispatcher *dispPt = dispDataPt;
	struct mcr_Key *keyPt = mcr_Key_data(signalPt);
	int applyType;
	struct mcr_Map *dispMaps = dispPt->ctx->standard.key_dispatcher_maps;
	if (!receiveFnc)
		receiveFnc = mcr_Trigger_receive;
	/* mcr_Trigger_receive requires an object */
	if (receiveFnc == mcr_Trigger_receive && !newTrigger)
		return EINVAL;
	if (keyPt) {
		applyType = keyPt->apply;
		if (applyType < MCR_BOTH) {
			return mcr_Key_Dispatcher_add_keys(dispMaps + applyType,
											   keyPt->key, newTrigger, receiveFnc);
		}
		/* Generic up, add to both */
		if (mcr_Key_Dispatcher_add_keys(dispMaps,
										keyPt->key, newTrigger, receiveFnc)) {
			return mcr_err;
		}
		return mcr_Key_Dispatcher_add_keys(dispMaps + 1,
										   keyPt->key, newTrigger, receiveFnc);
	}
	/* Generic up, add to both */
	if (mcr_Key_Dispatcher_add_keys(dispMaps, MCR_KEY_ANY,
									newTrigger, receiveFnc)) {
		return mcr_err;
	}
	return mcr_Key_Dispatcher_add_keys(dispMaps + 1, MCR_KEY_ANY,
									   newTrigger, receiveFnc);
}

int mcr_Key_Dispatcher_clear(void *dispDataPt)
{
	struct mcr_CtxDispatcher *dispPt = dispDataPt;
	struct mcr_Map *maps = dispPt->ctx->standard.key_dispatcher_maps;
	/* Down, up, and generic */
	mcr_Map_clear(maps++);
	mcr_Map_clear(maps);
	return 0;
}

/// \todo iterator instead of "for each"
bool mcr_Key_Dispatcher_dispatch(void *dispDataPt,
								 struct mcr_Signal * signalPt, unsigned int mods)
{
#define localDispAll(itPt) \
	dassert(((struct mcr_DispatchPair *)itPt)->dispatch); \
	if (((struct mcr_DispatchPair *)itPt)->dispatch(*(void **)itPt, signalPt, mods)) \
		return true;
#define localDispArrPt \
	if (arrPt) { \
		MCR_ARR_FOR_EACH(*arrPt, localDispAll); \
	}

	struct mcr_CtxDispatcher *dispMapPt = dispDataPt;
	struct mcr_Key *keyPt = mcr_Key_data(signalPt);
	struct mcr_Map *maps = dispMapPt->ctx->standard.key_dispatcher_maps;
	struct mcr_Array *arrPt;
	int key, applyType;
	dassert(dispDataPt);
	if (keyPt) {
		key = keyPt->key;
		applyType = keyPt->apply;
		if (applyType < MCR_BOTH) {
			/* Specific up type */
			if (key != MCR_KEY_ANY) {
				arrPt = mcr_Map_value(maps + applyType, &key);
				localDispArrPt;
			}
			arrPt = mcr_Map_value(maps + applyType, &mcr_Key_gen_key);
			localDispArrPt;
		} else {
			/* Generic up type */
			if (key != MCR_KEY_ANY) {
				arrPt = mcr_Map_value(maps, &key);
				localDispArrPt;
				arrPt = mcr_Map_value(maps + 1, &key);
				localDispArrPt;
			}
		}
	}
	/* all generic */
	/// \bug Generic receivers may receive twice
	arrPt = mcr_Map_value(maps, &mcr_Key_gen_key);
	localDispArrPt;
	arrPt = mcr_Map_value(maps + 1, &mcr_Key_gen_key);
	localDispArrPt;
	return false;
#undef localDispArrPt
#undef localDispAll
}

void mcr_Key_Dispatcher_modifier(void *dispDataPt,
								 struct mcr_Signal *sigPt, unsigned int *modsPt)
{
	dassert(dispDataPt);
	dassert(modsPt);
	struct mcr_context *ctx = ((struct mcr_CtxDispatcher *)dispDataPt)->ctx;
	struct mcr_Key *keyPt = mcr_Key_data(sigPt);
	int key;
	unsigned int *found;
	if (keyPt) {
		key = keyPt->key;
		found = mcr_Map_value(&ctx->standard.map_key_modifier, &key);
		if (found) {
			switch (keyPt->apply) {
			case MCR_SET:
				*modsPt |= *found;
				break;
			case MCR_UNSET:
			case MCR_BOTH:
				*modsPt &= ~(*found);
				break;
			case MCR_TOGGLE:
				if ((*modsPt & *found) == *found) {
					*modsPt &= ~(*found);
				} else {
					*modsPt |= *found;
				}
				break;
			}
		}
	}
}

int mcr_Key_Dispatcher_remove(void *dispDataPt, void *delTrigger)
{
#define localRemove(itPt) \
	mcr_Array_remove((struct mcr_Array *)itPt, &delTrigger);
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
					mcr_Map_unmap(mapPt, (void *)itPt);
			} else {
				arrPt = MCR_MAP_VALUEOF(*mapPt, itPt);
				if (!arrPt->used)
					mcr_Map_unmap(mapPt, (void *)itPt);
			}
			itPt -= bytes;
		}
	}
}

int mcr_Key_Dispatcher_trim(void *dispDataPt)
{
	struct mcr_CtxDispatcher *dispPt = dispDataPt;
	struct mcr_Map *maps = dispPt->ctx->standard.key_dispatcher_maps;
	mcr_Key_Dispatcher_remove_empties(maps);
	mcr_Key_Dispatcher_remove_empties(maps + 1);
	return 0;
}

struct mcr_ISignal *mcr_iKey(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.ikey;
}
