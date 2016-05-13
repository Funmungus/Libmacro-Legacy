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
#include <string.h>

static const struct mcr_Interface _MCR_SIGNAL_IFACE = {
	.id = ~0,
	.compare = mcr_Signal_compare,
	.copy = mcr_Signal_copy,
	.data_size = sizeof(struct mcr_Signal),
	.init = mcr_Signal_init,
	.free = mcr_Signal_free
};

const struct mcr_Interface *mcr_Signal_iface()
{
	return &_MCR_SIGNAL_IFACE;
}

void mcr_Signal_init(void *sigDataPt)
{
	dassert(sigDataPt);
	memset(sigDataPt, 0, sizeof(struct mcr_Signal));
}

void mcr_Signal_free(void *sigDataPt)
{
	dassert(sigDataPt);
	mcr_inst_free(sigDataPt);
}

int mcr_send(struct mcr_context *ctx, struct mcr_Signal *sigPt)
{
	/* if no interface, or is blocked by dispatch, return success */
	return sigPt->isig ? sigPt->is_dispatch
		&& mcr_dispatch(ctx, sigPt) ? 0 : sigPt->isig->send(sigPt) : 0;
}

int mcr_Signal_copy(void *dstPt, void *srcPt)
{
	struct mcr_Signal *dstSig = dstPt, *srcSig = srcPt;
	int err = mcr_inst_copy(dstPt, srcPt);
	if (err)
		return err;
	if (dstSig)
		dstSig->is_dispatch = srcSig ? srcSig->is_dispatch : false;
	return 0;
}

int mcr_Signal_compare(const void *lhsSigPt, const void *rhsSigPt)
{
	if (rhsSigPt) {
		if (lhsSigPt) {
			if (lhsSigPt == rhsSigPt)
				return 0;
			const struct mcr_Signal *lSigPt = lhsSigPt, *rSigPt =
				rhsSigPt;
			int valHolder = mcr_inst_compare(lhsSigPt, rhsSigPt);
			return valHolder ? valHolder :
				MCR_CMP_INTEGRAL(lSigPt->is_dispatch,
				rSigPt->is_dispatch);
		}
		return -1;
	}
	return ! !lhsSigPt;
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
