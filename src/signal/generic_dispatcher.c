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

#include <string.h>

static int gendisp_add(void *dispPt,
					   struct mcr_Signal * sigPt, void *receiver,
					   mcr_Dispatcher_receive_fnc receiverFnc);
static int gendisp_clear(void *dispPt);
static bool gendisp_dispatch(void *dispPt,
							 struct mcr_Signal * sigPt, unsigned int mods);
static void gendisp_modifier(void *dispPt,
							 struct mcr_Signal * sigPt, unsigned int *modsPt);
static int gendisp_remove(void *dispPt, void *remReceiver);

int mcr_GenericDispatcher_init(void *genericDispatcherPt)
{
	struct mcr_GenericDispatcher *dispPt = genericDispatcherPt;
	if (!dispPt)
		return 0;
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
	dispPt->dispatcher.trim = mcr_GenericDispatcher_trim;
	return 0;
}

int mcr_GenericDispatcher_deinit(void *genericDispatcherPt)
{
	struct mcr_GenericDispatcher *dispPt = genericDispatcherPt;
	if (!dispPt)
		return 0;
	mcr_Map_deinit(&dispPt->signal_receivers);
	mcr_Array_deinit(&dispPt->receivers);
	return 0;
}

int mcr_GenericDispatcher_trim(void *genericDispatcherPt)
{
	struct mcr_GenericDispatcher *dispPt = genericDispatcherPt;
	if (!dispPt)
		return 0;
	mcr_err = 0;
	mcr_Map_trim(&dispPt->signal_receivers);
	mcr_Array_trim(&dispPt->receivers);
	return mcr_err;
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
		return mcr_Array_add(MCR_MAP_VALUEOF(genDisp->signal_receivers, elementPt),
							 &disp, 1, true);
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
