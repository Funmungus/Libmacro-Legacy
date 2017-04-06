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
#include <string.h>

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
	struct mcr_mod_signal *modSignal = &ctx->signal;
	struct mcr_Dispatcher *dispPt =
			sigPt ? sigPt->isignal->dispatcher : NULL, *genPt =
				modSignal->dispatcher_generic_pt;
	bool isGen = genPt && modSignal->dispatcher_generic_enabled;
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

int mcr_Signal_copy(void *dstPt, void *srcPt)
{
	struct mcr_Signal *dstSig = dstPt, *srcSig = srcPt;
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
			       MCR_CMP_INTEGRAL(lSigPt->is_dispatch,
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
