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

#include "mcr/extras/extras.h"
#include "mcr/extras/private.h"
#include "mcr/modules.h"

int mcr_extras_initialize(struct mcr_context *ctx)
{
	int err = mcr_safe_string_initialize(ctx);
	if (err)
		return err;
	if ((err = mcr_signal_extras_initialize(ctx)))
		return err;
	return mcr_extras_native_initialize(ctx);
}

int mcr_extras_deinitialize(struct mcr_context *ctx)
{
	int err = mcr_extras_native_deinitialize(ctx);
	if (err)
		return err;
	if ((err = mcr_signal_extras_deinitialize(ctx)))
		return err;
	return mcr_safe_string_deinitialize(ctx);
}

int mcr_extras_load_contract(struct mcr_context *ctx)
{
	char *cmdNames[] = {
		"Command", "Cmd"
	};
	char *skNames[] = {
		"StringKey", "SK", "string_key"
	};
	struct mcr_IRegistry *isigReg = mcr_ISignal_reg(ctx);
	int err = mcr_reg_set_names(isigReg, mcr_iCommand(ctx),
		(const char *)cmdNames[0], (const char **)cmdNames,
		arrlen(cmdNames));
	if (!err)
		err = mcr_reg_set_names(isigReg, mcr_iStringKey(ctx),
			(const char *)skNames[0], (const char **)skNames,
			arrlen(skNames));
	return err ? err : mcr_StringKey_load_contract(ctx);
}
