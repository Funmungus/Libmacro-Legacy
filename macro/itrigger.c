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

#include "mcr/macro/macro.h"
#include "mcr/modules.h"
#include <string.h>

void mcr_ITrigger_init(void *itrigDataPt)
{
	struct mcr_ITrigger *itrigPt = itrigDataPt;
	if (itrigPt) {
		memset(itrigPt, 0, sizeof(struct mcr_ITrigger));
		mcr_iinit(&itrigPt->iface);
	}
}

struct mcr_IRegistry *mcr_ITrigger_reg(struct mcr_context *ctx)
{
	return &ctx->macro.itriggers;
}

struct mcr_ITrigger *mcr_ITrigger_from_id(struct mcr_context *ctx, size_t id)
{
	return mcr_reg_from_id(mcr_ITrigger_reg(ctx), id);
}

struct mcr_ITrigger *mcr_ITrigger_from_name(struct mcr_context *ctx,
	const char *name)
{
	return mcr_reg_from_name(mcr_ITrigger_reg(ctx), name);
}
