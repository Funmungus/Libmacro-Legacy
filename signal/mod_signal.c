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
#include "mcr/signal/private.h"
#include "mcr/modules.h"

int mcr_signal_initialize(struct mcr_context *ctx)
{
	int err = mcr_reg_init(mcr_ISignal_reg(ctx));
	dassert(ctx);
	if (err)
		return err;
	return mcr_dispatcher_initialize(ctx);
}

int mcr_signal_deinitialize(struct mcr_context *ctx)
{
	int err = mcr_dispatcher_deinitialize(ctx);
	dassert(ctx);
	if (err)
		return err;
	return mcr_reg_deinit(mcr_ISignal_reg(ctx));
}

int mcr_signal_load_contract(struct mcr_context *ctx)
{
	return mcr_mod_load_contract(ctx);
}

void mcr_signal_trim(struct mcr_context *ctx)
{
	struct mcr_Map *modNamesPt, *namesModPt;
	mcr_reg_trim(mcr_ISignal_reg(ctx));
	mcr_Dispatcher_trim_all(ctx);
	mcr_ModFlags_maps(ctx, &modNamesPt, &namesModPt);
	mcr_Map_trim(modNamesPt);
	mcr_Map_trim(namesModPt);
}
