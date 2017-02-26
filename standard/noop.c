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
#include "mcr/modules.h"
#include <errno.h>

void mcr_NoOp_set_all(mcr_NoOp * noopPt, int sec, int nsec)
{
	dassert(noopPt);
	noopPt->tv_sec = sec;
	noopPt->tv_nsec = nsec;
}

int mcr_NoOp_send(struct mcr_Signal *signalData)
{
	dassert(signalData);
	mcr_NoOp *nPt = mcr_NoOp_data(signalData);
	return nPt ? mcr_NoOp_send_data(nPt) : 0;
}

int mcr_NoOp_send_data(mcr_NoOp * dataPt)
{
	dassert(dataPt);
	thrd_sleep(dataPt, NULL);
	int err = errno;
	return err ? err : 0;
}

struct mcr_ISignal *mcr_iNoOp(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.inoop;
}
