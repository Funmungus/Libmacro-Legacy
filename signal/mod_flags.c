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

/* Names */
unsigned int mcr_ModFlags_mod(struct mcr_context *ctx, const char *name)
{
	unsigned int *found = mcr_Map_value(&ctx->signal.map_name_mod, &name);
	return found ? *found : MCR_MOD_ANY;
}

const char *mcr_ModFlags_name(struct mcr_context *ctx, unsigned int modifier)
{
	mcr_String *ret = mcr_Map_value(&ctx->signal.map_mod_name, &modifier);
	return ret ? ret->array : NULL;
}

size_t mcr_ModFlags_count(struct mcr_context * ctx)
{
	return ctx->signal.map_mod_name.set.used;
}

unsigned int mcr_ModFlags_combine(const unsigned int *modsArray, size_t length)
{
	unsigned int ret = 0;
	if (!modsArray || length == (size_t) ~ 0 || !length)
		return MCR_MF_NONE;
	while (length--) {
		ret |= modsArray[length];
	}
	return ret;
}

unsigned int mcr_ModFlags_add(unsigned int mods, unsigned int newMod)
{
	return mods | newMod;
}

bool mcr_ModFlags_has(unsigned int mods, unsigned int modVal)
{
	return MCR_MOD_HAS(mods, modVal);
}

unsigned int mcr_ModFlags_remove(unsigned int mods, unsigned int delMod)
{
	return mods & (~delMod);
}

void mcr_ModFlags_maps(struct mcr_context *ctx, struct mcr_Map **mapModName,
	struct mcr_Map **mapNameMod)
{
	dassert(ctx);
	if (mapModName)
		*mapModName = &ctx->signal.map_mod_name;
	if (mapNameMod)
		*mapNameMod = &ctx->signal.map_name_mod;
}

int mcr_mod_load_contract(struct mcr_context *ctx)
{
	unsigned int mods[] = { MCR_MOD_ANY, MCR_MF_NONE,
		MCR_ALT, MCR_ALTGR, MCR_AMIGA, MCR_CMD,
		MCR_CODE, MCR_COMPOSE, MCR_CTRL, MCR_FN, MCR_FRONT,
		MCR_GRAPH, MCR_HYPER, MCR_META, MCR_SHIFT, MCR_SUPER,
		MCR_SYMBOL, MCR_TOP, MCR_WIN, MCR_MF_USER
	};
	const char *modNames[] = { "Any", "None",
		"Alt", "AltGr", "Amiga", "Cmd",
		"Code", "Compose", "Ctrl", "Fn", "Front",
		"Graph", "Hyper", "Meta", "Shift", "Super",
		"Symbol", "Top", "Win", "User"
	};
	unsigned int addMods[5] = {
		MCR_ALT, MCR_ALTGR, MCR_CMD, MCR_CTRL, MCR_WIN
	};
	const char *addNames[][2] = {
		{"Option", ""}, {"Alt Gr", "alt_gr"}, {"Command", ""},
		{"Control", ""}, {"Window", "Windows"}
	};
	size_t addLens[5] = { 1, 2, 1, 1, 2 };
	int i = arrlen(mods), err;
	struct mcr_Map *modNamesPt, *namesModPt;
	mcr_ModFlags_maps(ctx, &modNamesPt, &namesModPt);
	while (i--) {
		if ((err = mcr_Map_map(modNamesPt, mods + i, modNames + i)))
			return err;
		if ((err = mcr_Map_map(namesModPt, modNames + i, mods + i)))
			return err;
	}
	i = arrlen(addMods);
	while (i--) {
		if ((err = mcr_Map_fill(namesModPt, addNames[i], addLens[i],
					addMods + i)))
			return err;
	}
	return 0;
}