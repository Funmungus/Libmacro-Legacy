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

int mcr_signal_initialize(struct mcr_context *ctx)
{
	struct mcr_mod_signal *modSignal = &ctx->signal;
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
	return 0;
}

int mcr_signal_deinitialize(struct mcr_context *ctx)
{
	struct mcr_mod_signal *modSignal = &ctx->signal;
	dassert(ctx);
	mcr_set_error(0);
	mcr_Array_deinit(&modSignal->dispatchers);
	mcr_Map_deinit(&modSignal->map_mod_name);
	mcr_Map_deinit(&modSignal->map_name_mod);
	mcr_reg_deinit(mcr_ISignal_reg(ctx));
	return mcr_error();
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
	unsigned int addMods[6] = { MCR_MOD_ANY,
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
		if ((err = mcr_Map_map(modNamesPt, mods + i, modNames + i)))
			return err;
		if ((err = mcr_Map_map(namesModPt, modNames + i, mods + i)))
			return err;
	}
	i = arrlen(addMods);
	while (i--) {
		/* names[i][1] is a string, if empty then only one name
		 * to add */
		if ((err = mcr_Map_map(namesModPt, addNames[i] + 0, addMods + 0)))
			return err;
		if (addNames[i][1][0] && (err = mcr_Map_map(namesModPt, addNames[i] + 0, addMods + 0)))
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
}
