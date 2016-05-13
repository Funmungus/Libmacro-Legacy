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

/*! \file
 * \brief \ref mcr_ModFlags flag enumeration for modifiers.
 */

#ifndef MCR_MOD_FLAGS_H
#define MCR_MOD_FLAGS_H

#include "mcr/signal/def.h"

/*!
 * \brief Enumeration for known and historical modifiers.
 *
 * To emulate how an operating system combines the names of some
 * modifiers, the names of those modifiers will map to the same value.
 * <br>
 * Some of these are actually hardware switches, depending on the
 * keyboard. If relying on keyboard or key codes to set modifiers, some
 * may never be used.
 * <br>
 * These values may change. Other values may also be
 * defined in other modules.
 */
enum mcr_ModFlags {
	/*! Interpreted as no modifiers */
	MCR_MF_NONE = 0,
	/* Alt and option are always the same here. */
	MCR_ALT = 1,
	MCR_OPTION = 1,
	MCR_ALTGR = 1 << 1,
	MCR_AMIGA = 1 << 2,
	MCR_CMD = 1 << 3,
	MCR_CODE = 1 << 4,
	MCR_COMPOSE = 1 << 5,
	MCR_CTRL = 1 << 6,
	MCR_FN = 1 << 7,
	MCR_FRONT = 1 << 8,
	MCR_GRAPH = 1 << 9,
	MCR_HYPER = 1 << 10,
	MCR_META = 1 << 11,
	MCR_SHIFT = 1 << 12,
	MCR_SUPER = 1 << 13,
	MCR_SYMBOL = 1 << 14,
	MCR_TOP = 1 << 15,
	MCR_WIN = 1 << 16,
	MCR_MF_USER = MCR_WIN << 1
};

MCR_API unsigned int mcr_mod_names_mod(struct mcr_context *ctx,
	const char *name);
MCR_API const char *mcr_mod_names_name(struct mcr_context *ctx,
	unsigned int modifier);
MCR_API size_t mcr_mod_names_count(struct mcr_context *ctx);

MCR_API void mcr_mod_names_all(struct mcr_context *ctx,
	unsigned int *modBuffer, size_t bufferLength);
MCR_API void mcr_mod_names_clear(struct mcr_context *ctx);
MCR_API int mcr_mod_names_set_name(struct mcr_context *ctx,
	unsigned int modifier, const char *name);
MCR_API int mcr_mod_names_add(struct mcr_context *ctx, unsigned int modifier,
	const char *name);
MCR_API int mcr_mod_names_map(struct mcr_context *ctx, unsigned int modifier,
	const char *name, const char **addNames, size_t bufferLen);
MCR_API int mcr_mod_names_remod(struct mcr_context *ctx,
	unsigned int modifier, unsigned int newMod);
MCR_API int mcr_mod_names_rename(struct mcr_context *ctx,
	const char *oldName, const char *newName);
MCR_API void mcr_mod_names_trim(struct mcr_context *ctx);

/* Modify modifiers */
MCR_API unsigned int mcr_mod_combine(const unsigned int *modsArray,
	size_t length);
MCR_API unsigned int mcr_mod_add(unsigned int mods, unsigned int newMod);
MCR_API bool mcr_mod_has(unsigned int mods, unsigned int modVal);
MCR_API unsigned int mcr_mod_remove(unsigned int mods, unsigned int delMod);

#define MCR_MOD_ADD( toModify, modifiers ) \
((toModify) | (modifiers))
#define MCR_MOD_HAS( modifiers, hasMod ) \
((modifiers & hasMod) == hasMod)
#define MCR_MOD_REMOVE( toModify, modifiers ) \
((toModify) & (~(modifiers)))

#endif
