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

#include "mcr/signal/signal.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "mcr/libmacro.h"

bool mcr_dispatch(struct mcr_context *ctx, struct mcr_Signal *sigPt)
{
	struct mcr_signal *modSignal = &ctx->signal;
	struct mcr_Dispatcher *dispPt =
			sigPt && sigPt->isignal ? sigPt->isignal->dispatcher : NULL, *genPt =
				modSignal->generic_dispatcher_pt;
	bool isGen = genPt && modSignal->is_generic_dispatcher;
	unsigned int mods = modSignal->internal_modifiers;
	/* If found only dispatch to enabled methods. */
	if (dispPt) {
		if (dispPt->dispatch(dispPt, sigPt, mods))
			return true;
	}
	if (isGen) {
		if (genPt->dispatch(genPt, sigPt, mods))
			return true;
	}
	if (dispPt && dispPt->modifier)
		dispPt->modifier(dispPt, sigPt, &modSignal->internal_modifiers);
	if (isGen && genPt->modifier)
		genPt->modifier(genPt, sigPt, &modSignal->internal_modifiers);
	return false;
}

size_t mcr_Dispatcher_count(struct mcr_context * ctx)
{
	dassert(ctx);
	return ctx->signal.dispatchers.used;
}

bool mcr_Dispatcher_is_enabled(struct mcr_context * ctx,
							   struct mcr_ISignal * isigPt)
{
	dassert(ctx);
	return isigPt ? ! !isigPt->dispatcher :
		   ctx->signal.is_generic_dispatcher &&
		   ctx->signal.generic_dispatcher_pt;
}

void mcr_Dispatcher_set_enabled(struct mcr_context *ctx,
								struct mcr_ISignal *isigPt, bool enable)
{
	dassert(ctx);
	if (isigPt) {
		if (enable) {
			isigPt->dispatcher = mcr_Dispatcher_from_id(ctx,
								 mcr_iid(isigPt));
		} else {
			isigPt->dispatcher = NULL;
		}
	} else {
		ctx->signal.is_generic_dispatcher = enable;
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
		mcr_Dispatcher_from_id(ctx, mcr_Instance_id(interceptPt));
	if (!receiveFnc)
		receiveFnc = mcr_Trigger_receive;
	/* mcr_Trigger_receive requires an object */
	if (receiveFnc == mcr_Trigger_receive && !receiver)
		return EINVAL;
	return dispPt
		   && dispPt->add ? dispPt->add(dispPt, interceptPt,
										receiver, receiveFnc) : 0;
}

int mcr_Dispatcher_add_generic(struct mcr_context *ctx,
							   struct mcr_Signal *interceptPt,
							   void *receiver, mcr_Dispatcher_receive_fnc receiveFnc)
{
	struct mcr_Dispatcher *genPt = ctx->signal.generic_dispatcher_pt;
	if (!receiveFnc)
		receiveFnc = mcr_Trigger_receive;
	/* mcr_Trigger_receive requires an object */
	if (receiveFnc == mcr_Trigger_receive && !receiver)
		return EINVAL;
	return genPt
		   && genPt->add ? genPt->add(genPt, interceptPt,
									  receiver, receiveFnc) : 0;
}

int mcr_Dispatcher_clear(struct mcr_context *ctx, struct mcr_ISignal *isigPt)
{
	struct mcr_Dispatcher *dispPt =
		mcr_Dispatcher_from_id(ctx, mcr_iid(isigPt));
	if (dispPt && dispPt->clear)
		return dispPt->clear(dispPt);
	return 0;
}

int mcr_Dispatcher_clear_all(struct mcr_context *ctx)
{
	struct mcr_Dispatcher *dispPt;
	struct mcr_Dispatcher *genPt = ctx->signal.generic_dispatcher_pt;
	size_t i = mcr_Dispatcher_count(ctx);
	int err;
	while (i--) {
		dispPt = mcr_Dispatcher_from_id(ctx, i);
		if (dispPt && dispPt->clear && (err = dispPt->clear(dispPt)))
			mset_error_return(err);
	}
	if (genPt && genPt->clear)
		return genPt->clear(genPt);
	return 0;
}

void mcr_Dispatcher_modify(struct mcr_context *ctx,
						   struct mcr_Signal *interceptPt, unsigned int *modsPt)
{
	struct mcr_Dispatcher *dispPt =
		mcr_Dispatcher_from_id(ctx, mcr_Instance_id(interceptPt));
	if (dispPt && dispPt->modifier)
		dispPt->modifier(dispPt, interceptPt, modsPt);
}

int mcr_Dispatcher_remove(struct mcr_context *ctx,
						  struct mcr_ISignal *isigPt, void *remReceiver)
{
	struct mcr_Dispatcher *dispPt =
		mcr_Dispatcher_from_id(ctx, mcr_iid(isigPt));
	if (dispPt && dispPt->remove)
		return dispPt->remove(dispPt, remReceiver);
	return 0;
}

int mcr_Dispatcher_remove_all(struct mcr_context *ctx, void *remReceiver)
{
	struct mcr_Dispatcher *dispPt;
	struct mcr_Dispatcher *genPt = ctx->signal.generic_dispatcher_pt;
	size_t i = mcr_Dispatcher_count(ctx);
	int err;
	while (i--) {
		dispPt = mcr_Dispatcher_from_id(ctx, i);
		if (dispPt && dispPt->remove &&
			(err = dispPt->remove(dispPt, remReceiver))) {
			mset_error_return(err);
		}
	}
	if (genPt && genPt->remove)
		return genPt->remove(genPt, remReceiver);
	return 0;
}

int mcr_Dispatcher_trim(struct mcr_context *ctx, struct mcr_ISignal *isigPt)
{
	struct mcr_Dispatcher *dispPt =
		mcr_Dispatcher_from_id(ctx, mcr_iid(isigPt));
	if (dispPt && dispPt->trim)
		return dispPt->trim(dispPt);
	return 0;
}

int mcr_Dispatcher_trim_all(struct mcr_context *ctx)
{
	struct mcr_Dispatcher *dispPt;
	struct mcr_Dispatcher *genPt = ctx->signal.generic_dispatcher_pt;
	size_t i = mcr_Dispatcher_count(ctx);
	int err;
	while (i--) {
		dispPt = mcr_Dispatcher_from_id(ctx, i);
		if (dispPt && dispPt->trim && (err = dispPt->trim(dispPt)))
			mset_error_return(err);
	}
	if (genPt && genPt->trim)
		return genPt->trim(genPt);
	return 0;
}

int mcr_Dispatcher_init(void *dispPt)
{
	if (dispPt)
		memset(dispPt, 0, sizeof(struct mcr_Dispatcher));
	return 0;
}

struct mcr_Dispatcher mcr_Dispatcher_new(mcr_Dispatcher_add_fnc add,
		mcr_Dispatcher_fnc clear, mcr_Dispatcher_dispatch_fnc dispatch,
		mcr_Dispatcher_modify_fnc modifier, mcr_Dispatcher_remove_fnc remove,
		mcr_Dispatcher_fnc trim)
{
	struct mcr_Dispatcher ret = { 0 };
	if (add)
		ret.add = add;
	if (clear)
		ret.clear = clear;
	if (dispatch)
		ret.dispatch = dispatch;
	if (modifier)
		ret.modifier = modifier;
	if (remove)
		ret.remove = remove;
	if (trim)
		ret.trim = trim;
	return ret;
}

void mcr_Dispatcher_set_all(struct mcr_Dispatcher *dispPt,
							mcr_Dispatcher_add_fnc add, mcr_Dispatcher_fnc clear,
							mcr_Dispatcher_dispatch_fnc dispatch,
							mcr_Dispatcher_modify_fnc modifier, mcr_Dispatcher_remove_fnc remove,
							mcr_Dispatcher_fnc trim)
{
	dassert(dispPt);
	dispPt->add = add;
	dispPt->clear = clear;
	dispPt->dispatch = dispatch;
	dispPt->modifier = modifier;
	dispPt->remove = remove;
	dispPt->trim = trim;
}

int mcr_Dispatcher_register(struct mcr_context *ctx, void *dispPt,
							size_t signalTypeId)
{
	struct mcr_Dispatcher *prevPt, **prevPtPt;
	dassert(ctx);
	if (signalTypeId == (size_t) ~ 0) {
		prevPtPt = &ctx->signal.generic_dispatcher_pt;
	} else {
		if (mcr_Array_minfill(&ctx->signal.dispatchers,
							  signalTypeId + 1, NULL)) {
			return mcr_err;
		}
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
		return ctx->signal.generic_dispatcher_pt;
	if (signalTypeId > MCR_ARR_LAST_INDEX(ctx->signal.dispatchers))
		return NULL;
	struct mcr_Dispatcher **ret =
		MCR_ARR_ELEMENT(ctx->signal.dispatchers, signalTypeId);
	return ret ? *ret : NULL;
}
