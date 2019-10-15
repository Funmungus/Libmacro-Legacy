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

#include "mcr/libmacro.h"

int mcr_ISignal_init(void *isigPt)
{
	if (isigPt) {
		memset(isigPt, 0, sizeof(struct mcr_ISignal));
		mcr_Interface_init(isigPt);
	}
	return 0;
}

struct mcr_ISignal mcr_ISignal_new(struct mcr_Dispatcher *dispPt,
								   mcr_signal_fnc sender)
{
	struct mcr_ISignal ret = {
		.dispatcher = dispPt,
		.send = sender
	};
	return ret;
}

void mcr_ISignal_set_all(struct mcr_ISignal *isigPt,
						 struct mcr_Dispatcher *dispPt, mcr_signal_fnc sender)
{
	dassert(isigPt);
	isigPt->dispatcher = dispPt;
	isigPt->send = sender;
}

struct mcr_IRegistry *mcr_ISignal_reg(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->signal.isignals;
}

const char *mcr_ISignal_name(struct mcr_context *ctx,
							 struct mcr_ISignal *isigPt)
{
	dassert(ctx);
	return mcr_reg_name(mcr_ISignal_reg(ctx), mcr_ISignal_id(isigPt));
}

struct mcr_ISignal *mcr_ISignal_from_id(struct mcr_context *ctx, size_t id)
{
	dassert(ctx);
	return mcr_reg_from_id(mcr_ISignal_reg(ctx), id);
}

struct mcr_ISignal *mcr_ISignal_from_name(struct mcr_context *ctx,
		const char *name)
{
	dassert(ctx);
	return mcr_reg_from_name(mcr_ISignal_reg(ctx), name);
}
