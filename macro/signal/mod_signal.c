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
#include "mcr/modules.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

static void gendisp_init(struct mcr_GenericDispatcher *dispPt);
static void gendisp_deinit(struct mcr_GenericDispatcher *dispPt);
static int gendisp_add(void *dispPt,
		       struct mcr_Signal * sigPt, void *receiver,
		       mcr_Dispatcher_receive_fnc receiverFnc);
static int gendisp_clear(void *dispPt);
static bool gendisp_dispatch(void *dispPt,
			     struct mcr_Signal * sigPt, unsigned int mods);
static void gendisp_modifier(void *dispPt,
			    struct mcr_Signal * sigPt, unsigned int *modsPt);
static int gendisp_remove(void *dispPt, void *remReceiver);
static int gendisp_trim(void *dispPt);

int mcr_signal_initialize(struct mcr_context *ctx)
{
	struct mcr_mod_signal *modSignal = &ctx->signal;
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
	gendisp_init(&modSignal->generic_dispatcher);
	return 0;
}

int mcr_signal_deinitialize(struct mcr_context *ctx)
{
	struct mcr_mod_signal *modSignal = &ctx->signal;
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
	gendisp_deinit(&modSignal->generic_dispatcher);
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
	gendisp_trim(&ctx->signal.generic_dispatcher);
}

static void gendisp_init(struct mcr_GenericDispatcher *dispPt)
{
	memset(dispPt, 0, sizeof(struct mcr_GenericDispatcher));
	dispPt->receivers = mcr_Array_new(mcr_ref_compare,
					  sizeof(struct mcr_DispatchPair));
	dispPt->signal_receivers = mcr_Map_new(sizeof(void *), sizeof(struct mcr_Array),
					       mcr_ref_compare, NULL,
					       mcr_Array_DispatchPair_interface());
	dispPt->dispatcher.add = gendisp_add;
	dispPt->dispatcher.clear = gendisp_clear;
	dispPt->dispatcher.dispatch = gendisp_dispatch;
	dispPt->dispatcher.modifier = gendisp_modifier;
	dispPt->dispatcher.remove = gendisp_remove;
	dispPt->dispatcher.trim = gendisp_trim;
}

static void gendisp_deinit(struct mcr_GenericDispatcher *dispPt)
{
	mcr_Map_deinit(&dispPt->signal_receivers);
	mcr_Array_deinit(&dispPt->receivers);
}

static int gendisp_add(void *dispPt,
		       struct mcr_Signal * sigPt, void *receiver,
		       mcr_Dispatcher_receive_fnc receiverFnc)
{
	struct mcr_GenericDispatcher *genDisp = dispPt;
	struct mcr_DispatchPair disp;
	void *elementPt;
	disp.dispatch = receiverFnc;
	disp.receiver = receiver;
	if (sigPt) {
		elementPt = mcr_Map_element_ensured(&genDisp->signal_receivers,
					  &sigPt);
		if (!elementPt)
			return mcr_err;
		return mcr_Array_add(MCR_MAP_VALUEOF(genDisp->signal_receivers, elementPt), &disp, 1, true);
	}
	return mcr_Array_add(&genDisp->receivers, &disp, 1, true);
}

static int gendisp_clear(void *dispPt)
{
	struct mcr_GenericDispatcher *genDisp = dispPt;
	mcr_err = 0;
	mcr_Map_clear(&genDisp->signal_receivers);
	mcr_Array_clear(&genDisp->receivers);
	return mcr_err;
}

static bool gendisp_dispatch(void *dispPt,
			     struct mcr_Signal * sigPt, unsigned int mods)
{
#define localDispAll(itPt) \
	dassert(((struct mcr_DispatchPair *)itPt)->dispatch); \
	if (((struct mcr_DispatchPair *)itPt)->dispatch(*(void **)itPt, sigPt, mods)) \
		return true;

	struct mcr_GenericDispatcher *genDisp = dispPt;
	struct mcr_Array *arrPt = mcr_Map_value(&genDisp->signal_receivers, &sigPt);
	mcr_err = 0;
	if (arrPt) {
		MCR_ARR_FOR_EACH(*arrPt, localDispAll);
	}
	MCR_ARR_FOR_EACH(genDisp->receivers, localDispAll);
	return false;
}

static void gendisp_modifier(void *dispPt,
			    struct mcr_Signal * sigPt, unsigned int *modsPt)
{
	UNUSED(dispPt);
	UNUSED(sigPt);
	UNUSED(modsPt);
}

static int gendisp_remove(void *dispPt, void *remReceiver)
{
	struct mcr_GenericDispatcher *genDisp = dispPt;
	mcr_err = 0;
#define localRemove(itPt) \
	mcr_Array_remove((struct mcr_Array *)itPt, &remReceiver);
	MCR_MAP_FOR_EACH_VALUE(genDisp->signal_receivers, localRemove);
	mcr_Array_remove(&genDisp->receivers, &remReceiver);
	return mcr_err;
}

static int gendisp_trim(void *dispPt)
{
	struct mcr_GenericDispatcher *genDisp = dispPt;
	mcr_err = 0;
	mcr_Map_trim(&genDisp->signal_receivers);
	mcr_Array_trim(&genDisp->receivers);
	return mcr_err;
}
