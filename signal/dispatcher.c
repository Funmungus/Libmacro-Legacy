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
#include "mcr/signal/private.h"
#include "mcr/modules.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

static unsigned int _internalMods = 0;
static mtx_t _modLock;
static struct timespec _modTimeout;
static size_t _initialize_count = 0;

/* Public access modifiers */
unsigned int mcr_modifiers(struct mcr_context *ctx)
{
	UNUSED(ctx);
	return _internalMods;
}

void mcr_set_modifiers(struct mcr_context *ctx, unsigned int setMods)
{
	int err = mtx_timedlock(&_modLock, &_modTimeout);
	UNUSED(ctx);
	dassert(_initialize_count);
	_internalMods = setMods;
	if (err == thrd_success)
		mtx_unlock(&_modLock);
}

void mcr_add_modifiers(struct mcr_context *ctx, unsigned int addMods)
{
	int err = mtx_timedlock(&_modLock, &_modTimeout);
	UNUSED(ctx);
	dassert(_initialize_count);
	_internalMods |= addMods;
	if (err == thrd_success)
		mtx_unlock(&_modLock);
}

void mcr_remove_modifiers(struct mcr_context *ctx, unsigned int remMods)
{
	int err = mtx_timedlock(&_modLock, &_modTimeout);
	UNUSED(ctx);
	dassert(_initialize_count);
	_internalMods &= (~remMods);
	if (err == thrd_success)
		mtx_unlock(&_modLock);
}

struct timespec mcr_modifiers_timeout(struct mcr_context *ctx)
{
	UNUSED(ctx);
	return _modTimeout;
}

void mcr_modifiers_set_timeout(struct mcr_context *ctx,
	struct timespec *setTime)
{
	UNUSED(ctx);
	if (setTime) {
		_modTimeout = *setTime;
	} else {
		_modTimeout.tv_sec = 1;
		_modTimeout.tv_nsec = 0;
	}
}

bool mcr_dispatch(struct mcr_context *ctx, struct mcr_Signal *signalData)
{
	struct mcr_Dispatcher *dispPt = signalData->isig->dispatcher,
		*genPt = ctx->signal.dispatcher_generic_pt;
	bool isGen = genPt && ctx->signal.dispatcher_generic_enabled;
	int locked;
	/* If found only dispatch to enabled methods. */
	if (dispPt) {
		if (dispPt->dispatch(dispPt, signalData, _internalMods))
			return true;
	}
	if (isGen) {
		if (genPt->dispatch(genPt, signalData, _internalMods))
			return true;
	}
	locked = mtx_timedlock(&_modLock, &_modTimeout);
	if (dispPt && dispPt->modifier) {
		dispPt->modifier(dispPt, signalData, &_internalMods);
	}
	if (isGen && genPt->modifier) {
		genPt->modifier(genPt, signalData, &_internalMods);
	}
	if (locked == thrd_success) {
		mtx_unlock(&_modLock);
	}
	return false;
}

size_t mcr_Dispatcher_count(struct mcr_context * ctx)
{
	return ctx->signal.dispatchers.used;
}

bool mcr_Dispatcher_is_enabled(struct mcr_context * ctx,
	struct mcr_ISignal * isigPt)
{
	return isigPt ? ! !isigPt->dispatcher :
		ctx->signal.dispatcher_generic_enabled &&
		ctx->signal.dispatcher_generic_pt;
}

void mcr_Dispatcher_set_enabled(struct mcr_context *ctx,
	struct mcr_ISignal *isigPt, bool enable)
{
	if (isigPt) {
		if (enable)
			isigPt->dispatcher = mcr_Dispatcher_from_id(ctx,
				mcr_inst_id(isigPt));
		else
			isigPt->dispatcher = NULL;
	} else {
		ctx->signal.dispatcher_generic_enabled = enable;
	}
}

void mcr_Dispatcher_set_enabled_all(struct mcr_context *ctx, bool enable)
{
	size_t i, maxi = ctx->signal.isignals.iset.used,
		dispMaxi = ctx->signal.dispatchers.used;
	if (dispMaxi < maxi)
		maxi = dispMaxi;
	for (i = maxi; i--;) {
		mcr_Dispatcher_set_enabled(ctx, mcr_ISignal_from_id(ctx, i),
			enable);
	}
	mcr_Dispatcher_set_enabled(ctx, NULL, enable);
}

int mcr_Dispatcher_add(struct mcr_context *ctx,
	struct mcr_Signal *interceptPt, void *receiver,
	mcr_Dispatcher_receive_fnc receiveFnc)
{
	struct mcr_Dispatcher *dispPt =
		mcr_Dispatcher_from_id(ctx, mcr_inst_id(interceptPt));
	return dispPt
		&& dispPt->add ? dispPt->add(dispPt, interceptPt,
		receiver, receiveFnc) : 0;
}

int mcr_Dispatcher_add_generic(struct mcr_context *ctx,
	struct mcr_Signal *interceptPt,
	void *receiver, mcr_Dispatcher_receive_fnc receiveFnc)
{
	struct mcr_Dispatcher *genPt = ctx->signal.dispatcher_generic_pt;
	return genPt
		&& genPt->add ? genPt->add(genPt, interceptPt,
		receiver, receiveFnc) : 0;
}

void mcr_Dispatcher_clear(struct mcr_context *ctx, struct mcr_ISignal *isigPt)
{
	struct mcr_Dispatcher *dispPt =
		mcr_Dispatcher_from_id(ctx, mcr_inst_id(isigPt));
	if (dispPt && dispPt->clear)
		dispPt->clear(dispPt);
}

void mcr_Dispatcher_clear_all(struct mcr_context *ctx)
{
	struct mcr_Dispatcher *dispPt;
	struct mcr_Dispatcher *genPt = ctx->signal.dispatcher_generic_pt;
	size_t i = mcr_Dispatcher_count(ctx);
	while (i--) {
		dispPt = mcr_Dispatcher_from_id(ctx, i);
		if (dispPt && dispPt->clear)
			dispPt->clear(dispPt);
	}
	if (genPt && genPt->clear)
		genPt->clear(genPt);
}

void mcr_Dispatcher_free(struct mcr_context *ctx, struct mcr_ISignal *isigPt)
{
	struct mcr_Dispatcher *dispPt =
		mcr_Dispatcher_from_id(ctx, mcr_inst_id(isigPt));
	if (dispPt && dispPt->free)
		dispPt->free(dispPt);
}

void mcr_Dispatcher_free_all(struct mcr_context *ctx)
{
	struct mcr_Dispatcher *dispPt;
	struct mcr_Dispatcher *genPt = ctx->signal.dispatcher_generic_pt;
	size_t i = mcr_Dispatcher_count(ctx);
	while (i--) {
		dispPt = mcr_Dispatcher_from_id(ctx, i);
		if (dispPt && dispPt->free)
			dispPt->free(dispPt);
	}
	if (genPt && genPt->free)
		genPt->free(genPt);
}

void mcr_Dispatcher_modify(struct mcr_context *ctx,
	struct mcr_Signal *interceptPt, unsigned int *modsPt)
{
	struct mcr_Dispatcher *dispPt =
		mcr_Dispatcher_from_id(ctx, mcr_inst_id(interceptPt));
	if (dispPt && dispPt->modifier)
		dispPt->modifier(dispPt, interceptPt, modsPt);
}

void mcr_Dispatcher_remove(struct mcr_context *ctx,
	struct mcr_ISignal *isigPt, void *remReceiver)
{
	struct mcr_Dispatcher *dispPt =
		mcr_Dispatcher_from_id(ctx, mcr_inst_id(isigPt));
	dassert(remReceiver);
	if (dispPt && dispPt->remove)
		dispPt->remove(dispPt, remReceiver);
}

void mcr_Dispatcher_remove_all(struct mcr_context *ctx, void *remReceiver)
{
	struct mcr_Dispatcher *dispPt;
	struct mcr_Dispatcher *genPt = ctx->signal.dispatcher_generic_pt;
	size_t i = mcr_Dispatcher_count(ctx);
	dassert(remReceiver);
	while (i--) {
		dispPt = mcr_Dispatcher_from_id(ctx, i);
		if (dispPt && dispPt->remove)
			dispPt->remove(dispPt, remReceiver);
	}
	if (genPt && genPt->remove)
		genPt->remove(genPt, remReceiver);
}

void mcr_Dispatcher_trim(struct mcr_context *ctx, struct mcr_ISignal *isigPt)
{
	struct mcr_Dispatcher *dispPt =
		mcr_Dispatcher_from_id(ctx, mcr_inst_id(isigPt));
	if (dispPt && dispPt->trim)
		dispPt->trim(dispPt);
}

void mcr_Dispatcher_trim_all(struct mcr_context *ctx)
{
	struct mcr_Dispatcher *dispPt;
	struct mcr_Dispatcher *genPt = ctx->signal.dispatcher_generic_pt;
	size_t i = mcr_Dispatcher_count(ctx);
	while (i--) {
		dispPt = mcr_Dispatcher_from_id(ctx, i);
		if (dispPt && dispPt->trim)
			dispPt->trim(dispPt);
	}
	if (genPt && genPt->trim)
		genPt->trim(genPt);
}

void mcr_Dispatcher_init(void *dispPt)
{
	dassert(dispPt);
	memset(dispPt, 0, sizeof(struct mcr_Dispatcher));
}

void mcr_Dispatcher_set_all(struct mcr_Dispatcher *dispPt,
	mcr_Dispatcher_add_fnc add, mcr_Dispatcher_fnc clear,
	mcr_Dispatcher_dispatch_fnc dispatch, mcr_Dispatcher_fnc free,
	mcr_Dispatcher_modify_fnc modifier, mcr_Dispatcher_remove_fnc remove,
	mcr_Dispatcher_fnc trim)
{
	dassert(dispPt);
	dispPt->add = add;
	dispPt->clear = clear;
	dispPt->dispatch = dispatch;
	dispPt->free = free;
	dispPt->modifier = modifier;
	dispPt->remove = remove;
	dispPt->trim = trim;
}

int mcr_Dispatcher_register(struct mcr_context *ctx, void *dispPt,
	size_t signalTypeId)
{
	int ret;
	struct mcr_Dispatcher *prevPt, **prevPtPt;
	if (signalTypeId == (size_t) ~ 0) {
		prevPtPt = &ctx->signal.dispatcher_generic_pt;
	} else {
		if ((ret = mcr_Array_minfill(&ctx->signal.dispatchers,
					signalTypeId + 1, NULL)))
			return ret;
		prevPtPt =
			MCR_ARR_ELEMENT(ctx->signal.dispatchers, signalTypeId);
	}
	dassert(prevPtPt);
	prevPt = *prevPtPt;
	if (dispPt != prevPt) {
		if (prevPt) {
			if (prevPt->clear)
				prevPt->clear(prevPt);
			if (prevPt->trim)
				prevPt->trim(prevPt);
		}
		*prevPtPt = dispPt;
	}
	return 0;
}

/* Dispatchers */
struct mcr_Dispatcher *mcr_Dispatcher_from_id(struct mcr_context *ctx,
	size_t signalTypeId)
{
	dassert(ctx);
	if (signalTypeId == (size_t) ~ 0)
		return ctx->signal.dispatcher_generic_pt;
	struct mcr_Dispatcher **ret =
		MCR_ARR_ELEMENT(ctx->signal.dispatchers, signalTypeId);
	return ret ? *ret : NULL;
}

int mcr_dispatcher_initialize(struct mcr_context *ctx)
{
	mcr_Array_init(&ctx->signal.dispatchers);
	mcr_Array_set_all(&ctx->signal.dispatchers, NULL, sizeof(void *));
	/* modifiers initialized */
	if (_initialize_count) {
		++_initialize_count;
		return 0;
	}
	_modTimeout.tv_sec = 1;
	_modTimeout.tv_nsec = 0;
	mcr_NameBiMap_init(&ctx->signal.mod_names);
	mcr_NameBiMap_set_all(&ctx->signal.mod_names, sizeof(unsigned int),
		mcr_unsigned_compare, NULL);
	if (mtx_init(&_modLock, mtx_timed) == thrd_success) {
		/* Successful, increase initialized count */
		++_initialize_count;
		return 0;
	}
	mset_error(EPERM);
	return -EPERM;
}

void mcr_dispatcher_cleanup(struct mcr_context *ctx)
{
	mcr_Dispatcher_free_all(ctx);
	mcr_Array_free(&ctx->signal.dispatchers);
	/* Remove an initialized reference of internal modifiers */
	if (_initialize_count && !--_initialize_count) {
		mtx_destroy(&_modLock);
		_initialize_count = 0;
	}
}
