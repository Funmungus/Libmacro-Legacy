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

#include "mcr/macro/macro.h"

#include <string.h>

#include "mcr/libmacro.h"

int mcr_ITrigger_init(void *itrigPt)
{
	struct mcr_ITrigger *localPt = itrigPt;
	if (localPt) {
		memset(localPt, 0, sizeof(struct mcr_ITrigger));
		mcr_Interface_init(&localPt->interface);
	}
	return 0;
}

struct mcr_ITrigger mcr_ITrigger_new(mcr_Dispatcher_receive_fnc receiveFnc)
{
	struct mcr_ITrigger ret;
	dassert(receiveFnc);
	mcr_Interface_init(&ret.interface);
	ret.receive = receiveFnc;
	return ret;
}

struct mcr_IRegistry *mcr_ITrigger_reg(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->macro.itriggers;
}

const char *mcr_ITrigger_name(struct mcr_context *ctx,
							  struct mcr_ITrigger *itrigPt)
{
	dassert(ctx);
	return mcr_reg_name(mcr_ITrigger_reg(ctx), mcr_ITrigger_id(itrigPt));
}

struct mcr_ITrigger *mcr_ITrigger_from_id(struct mcr_context *ctx, size_t id)
{
	dassert(ctx);
	return mcr_reg_from_id(mcr_ITrigger_reg(ctx), id);
}

struct mcr_ITrigger *mcr_ITrigger_from_name(struct mcr_context *ctx,
		const char *name)
{
	dassert(ctx);
	return mcr_reg_from_name(mcr_ITrigger_reg(ctx), name);
}
