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

#include "mcr/signal/signal.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "mcr/libmacro.h"

static const struct mcr_Interface _MCR_SIGNAL_IFACE = {
	.id = ~0,
	.compare = mcr_Signal_compare,
	.copy = mcr_Signal_copy,
	.data_size = sizeof(struct mcr_Signal),
	.init = mcr_Signal_init,
	.deinit = mcr_Signal_deinit
};

const struct mcr_Interface *mcr_Signal_interface()
{
	return &_MCR_SIGNAL_IFACE;
}

int mcr_Signal_init(void *signalPt)
{
	if (signalPt)
		memset(signalPt, 0, sizeof(struct mcr_Signal));
	return 0;
}

int mcr_Signal_deinit(void *signalPt)
{
	if (signalPt)
		mcr_Instance_deinit(signalPt);
	return 0;
}

int mcr_send(struct mcr_context *ctx, struct mcr_Signal *sigPt)
{
	/* sync this code with mcr_dispatch */
	/* if no interface, or is blocked by dispatch, return success */
	struct mcr_signal *modSignal = &ctx->signal;
	struct mcr_Dispatcher *dispPt =
			sigPt ? sigPt->isignal->dispatcher : NULL, *genPt =
				modSignal->generic_dispatcher_pt;
	bool isGen = genPt && modSignal->is_generic_dispatcher;
	unsigned int mods = modSignal->internal_mods;
	if (dispPt) {
		if (dispPt->dispatch(dispPt, sigPt, mods))
			return 0;
	}
	if (isGen) {
		if (genPt->dispatch(genPt, sigPt, mods))
			return 0;
	}
	if (dispPt && dispPt->modifier) {
		dispPt->modifier(dispPt, sigPt, &modSignal->internal_mods);
	}
	if (isGen && genPt->modifier) {
		genPt->modifier(genPt, sigPt, &modSignal->internal_mods);
	}
	return sigPt->isignal ? sigPt->is_dispatch
		   && mcr_dispatch(ctx,
						   sigPt) ? 0 : sigPt->isignal->send(sigPt) : 0;
}

int mcr_Signal_copy(void *dstPt, const void *srcPt)
{
	struct mcr_Signal *dstSig = dstPt;
	const struct mcr_Signal *srcSig = srcPt;
	int err = mcr_Instance_copy(dstPt, srcPt);
	if (err)
		return err;
	if (dstSig)
		dstSig->is_dispatch = srcSig ? srcSig->is_dispatch : false;
	return 0;
}

int mcr_Signal_compare(const void *lhsSignalPt, const void *rhsSignalPt)
{
	if (rhsSignalPt) {
		if (lhsSignalPt) {
			if (lhsSignalPt == rhsSignalPt)
				return 0;
			const struct mcr_Signal *lSigPt = lhsSignalPt, *rSigPt =
													  rhsSignalPt;
			int valHolder =
				mcr_Instance_compare(lhsSignalPt, rhsSignalPt);
			return valHolder ? valHolder :
				   MCR_CMP(lSigPt->is_dispatch,
						   rSigPt->is_dispatch);
		}
		return -1;
	}
	return ! !lhsSignalPt;
}

int mcr_Signalref_compare(const void *lhsPtPt, const void *rhsPtPt)
{
	if (rhsPtPt) {
		if (lhsPtPt) {
			if (lhsPtPt == rhsPtPt)
				return 0;
			return mcr_Signal_compare(*(const void **)lhsPtPt,
									  *(const void **)rhsPtPt);
		}
		return -1;
	}
	return ! !lhsPtPt;
}

int mcr_signal_initialize(struct mcr_context *ctx)
{
	struct mcr_signal *modSignal = &ctx->signal;
	modSignal->generic_dispatcher_pt = &modSignal->generic_dispatcher.dispatcher;
	int err = mcr_reg_init(mcr_ISignal_reg(ctx));
	dassert(ctx);
	if (err)
		return err;
	modSignal->dispatchers = mcr_Array_new(NULL, sizeof(void *));
	modSignal->map_mod_name =
		mcr_Map_new(sizeof(unsigned int), sizeof(mcr_String),
					mcr_unsigned_compare, NULL, mcr_String_interface());
	modSignal->map_name_mod =
		mcr_Map_new(sizeof(mcr_String), sizeof(unsigned int),
					mcr_name_compare, mcr_String_interface(), NULL);
	mcr_GenericDispatcher_init(&modSignal->generic_dispatcher);
	return 0;
}

int mcr_signal_deinitialize(struct mcr_context *ctx)
{
	struct mcr_signal *modSignal = &ctx->signal;
	dassert(ctx);
	mcr_err = 0;
	mcr_Array_deinit(&modSignal->dispatchers);
	mcr_Map_deinit(&modSignal->map_mod_name);
	mcr_Map_deinit(&modSignal->map_name_mod);
	mcr_reg_deinit(mcr_ISignal_reg(ctx));
	if (modSignal->generic_dispatcher_pt) {
		if (modSignal->generic_dispatcher_pt->clear)
			modSignal->generic_dispatcher_pt->clear(modSignal->generic_dispatcher_pt);
		if (modSignal->generic_dispatcher_pt->trim)
			modSignal->generic_dispatcher_pt->trim(modSignal->generic_dispatcher_pt);
	}
	mcr_GenericDispatcher_deinit(&modSignal->generic_dispatcher);
	return mcr_err;
}

int mcr_signal_load_contract(struct mcr_context *ctx)
{
	unsigned int mods[] = { MCR_MF_NONE,
							MCR_ALT, MCR_ALTGR, MCR_AMIGA, MCR_CMD,
							MCR_CODE, MCR_COMPOSE, MCR_CTRL, MCR_FN, MCR_FRONT,
							MCR_GRAPH, MCR_HYPER, MCR_META, MCR_SHIFT, MCR_SUPER,
							MCR_SYMBOL, MCR_TOP, MCR_WIN, MCR_MF_USER
						  };
	const char *modNames[] = { "None",
							   "Alt", "AltGr", "Amiga", "Cmd",
							   "Code", "Compose", "Ctrl", "Fn", "Front",
							   "Graph", "Hyper", "Meta", "Shift", "Super",
							   "Symbol", "Top", "Win", "User"
							 };
	unsigned int addMods[] = {  MCR_MOD_ANY,
								MCR_ALT, MCR_ALTGR, MCR_CMD, MCR_CTRL, MCR_WIN
							 };
	const char *addNames[][2] = { {"Any", ""},
		{"Option", ""}, {"Alt Gr", "alt_gr"}, {"Command", ""},
		{"Control", ""}, {"Window", "Windows"}
	};
	int i = arrlen(mods), err;
	struct mcr_Map *modNamesPt, *namesModPt;
	dassert(arrlen(addNames) == arrlen(addMods));
	mcr_ModFlags_maps(ctx, &modNamesPt, &namesModPt);
	while (i--) {
		if ((err = mcr_Map_map(modNamesPt, mods + i, (void *)(modNames + i))))
			return err;
		if ((err = mcr_Map_map(namesModPt, (void *)(modNames + i), mods + i)))
			return err;
	}
	i = arrlen(addMods);
	while (i--) {
		/* names[i][1] is a string, if empty then only one name
		 * to add */
		if ((err = mcr_Map_map(namesModPt, (void *)(addNames[i] + 0), addMods + 0)))
			return err;
		if (addNames[i][1][0]
			&& (err = mcr_Map_map(namesModPt, (void *)(addNames[i] + 0), addMods + 0)))
			return err;
	}
	return 0;
}

void mcr_signal_trim(struct mcr_context *ctx)
{
	struct mcr_Map *modNamesPt, *namesModPt;
	mcr_reg_trim(mcr_ISignal_reg(ctx));
	mcr_Dispatcher_trim_all(ctx);
	mcr_Array_trim(&ctx->signal.dispatchers);
	mcr_ModFlags_maps(ctx, &modNamesPt, &namesModPt);
	mcr_Map_trim(modNamesPt);
	mcr_Map_trim(namesModPt);
	mcr_GenericDispatcher_trim(&ctx->signal.generic_dispatcher);
}
