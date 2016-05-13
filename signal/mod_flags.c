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
#include "mcr/signal/private.h"
#include "mcr/modules.h"

/* Names */
unsigned int mcr_mod_names_mod(struct mcr_context *ctx, const char *name)
{
	unsigned int *found = mcr_NameBiMap_value(&ctx->signal.mod_names,
		name);
	return found ? *found : MCR_MOD_ANY;
}

const char *mcr_mod_names_name(struct mcr_context *ctx, unsigned int modifier)
{
	return mcr_NameBiMap_name(&ctx->signal.mod_names, &modifier);
}

size_t mcr_mod_names_count(struct mcr_context * ctx)
{
	return ctx->signal.mod_names.second.set.used;
}

/* Names */
void mcr_mod_names_all(struct mcr_context *ctx, unsigned int *modBuffer,
	size_t bufferLength)
{
	size_t i;
	unsigned int *modPt;
	if (!modBuffer)
		return;
	i = mcr_mod_names_count(ctx);
	if (bufferLength < i)
		i = bufferLength;
	while (i--) {
		modPt = MCR_ARR_ELEMENT(ctx->signal.mod_names.second.set, i);
		modBuffer[i] = *modPt;
	}
}

void mcr_mod_names_clear(struct mcr_context *ctx)
{
	mcr_Map_clear(&ctx->signal.mod_names.first);
	mcr_Map_clear(&ctx->signal.mod_names.second);
}

int mcr_mod_names_set_name(struct mcr_context *ctx, unsigned int modifier,
	const char *name)
{
	return mcr_NameBiMap_setvalue(&ctx->signal.mod_names, &modifier, name);
}

int mcr_mod_names_add(struct mcr_context *ctx, unsigned int modifier,
	const char *name)
{
	return mcr_NameBiMap_addname(&ctx->signal.mod_names, name, &modifier);
}

int mcr_mod_names_map(struct mcr_context *ctx, unsigned int modifier,
	const char *name, const char **addNames, size_t bufferLen)
{
	return mcr_NameBiMap_map(&ctx->signal.mod_names, name, addNames,
		bufferLen, &modifier);
}

int mcr_mod_names_remod(struct mcr_context *ctx, unsigned int modifier,
	unsigned int newMod)
{
	int ret = mcr_mod_names_set_name(ctx, newMod,
		mcr_mod_names_name(ctx, modifier));
	if (!ret)
		ret = mcr_mod_names_set_name(ctx, modifier, NULL);
	return ret;
}

int mcr_mod_names_rename(struct mcr_context *ctx, const char *oldName,
	const char *newName)
{
	int ret;
	unsigned int *found =
		mcr_NameBiMap_value(&ctx->signal.mod_names, oldName);
	if (found) {
		if ((ret = mcr_mod_names_set_name(ctx, *found, newName)))
			return ret;
		mcr_NameBiMap_remname(&ctx->signal.mod_names, oldName, false);
	}
	return 0;
}

void mcr_mod_names_trim(struct mcr_context *ctx)
{
	mcr_Map_trim(&ctx->signal.mod_names.first);
	mcr_Map_trim(&ctx->signal.mod_names.second);
}

unsigned int mcr_mod_combine(const unsigned int *modsArray, size_t length)
{
	unsigned int ret = 0;
	if (!modsArray || length == (size_t) ~ 0 || !length)
		return MCR_MOD_ANY;
	while (length--) {
		ret |= modsArray[length];
	}
	return ret;
}

unsigned int mcr_mod_add(unsigned int mods, unsigned int newMod)
{
	return mods | newMod;
}

bool mcr_mod_has(unsigned int mods, unsigned int modVal)
{
	return MCR_MOD_HAS(mods, modVal);
}

unsigned int mcr_mod_remove(unsigned int mods, unsigned int delMod)
{
	return mods & (~delMod);
}

int mcr_mod_load_contract(struct mcr_context *ctx)
{
	const unsigned int mods[] = {
		MCR_ALT, MCR_ALTGR, MCR_AMIGA, MCR_CMD,
		MCR_CODE, MCR_COMPOSE, MCR_CTRL, MCR_FN, MCR_FRONT,
		MCR_GRAPH, MCR_HYPER, MCR_META, MCR_SHIFT, MCR_SUPER,
		MCR_SYMBOL, MCR_TOP, MCR_WIN
	};
	const char *modNames[] = {
		"Alt", "AltGr", "Amiga", "Cmd",
		"Code", "Compose", "Ctrl", "Fn", "Front",
		"Graph", "Hyper", "Meta", "Shift", "Super",
		"Symbol", "Top", "Win"
	};
	const char *addNames[][2] = {
		{"Option", ""}, {"Alt Gr", "alt_gr"}, {"", ""}, {"Command", ""},
		{"", ""}, {"", ""}, {"Control", ""}, {"", ""}, {"", ""},
		{"", ""}, {"", ""}, {"", ""}, {"", ""}, {"", ""},
		{"", ""}, {"", ""}, {"Window", "Windows"}
	};
	size_t addLens[17] = {
		1, 2, 0, 1,
		0, 0, 1, 0, 0,
		0, 0, 0, 0, 0,
		0, 0, 2
	};
	int i = arrlen(mods), ret;
	while (i--) {
		if ((ret = mcr_mod_names_map(ctx, mods[i], modNames[i],
					addNames[i], addLens[i])))
			return ret;
	}
	return 0;
}
