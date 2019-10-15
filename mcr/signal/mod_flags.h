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

/*! \file
 *  \brief \ref mcr_ModFlags - Flag enumeration for modifiers
 */

#ifndef MCR_SIGNAL_MOD_FLAGS_H_
#define MCR_SIGNAL_MOD_FLAGS_H_

#include "mcr/signal/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Enumeration for known and historical modifiers.
 *
 *  To emulate how an operating system combines the names of some
 *  modifiers, the names of those modifiers will map to the same value.\n
 *  Some of these are actually hardware switches, depending on the
 *  keyboard. If relying on keyboard or key codes to set modifiers, some
 *  may never be used.\n
 *  These values may change. Other values may also be
 *  defined in other modules.
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

/*! Get the modifier from its name
 *
 *  \param name \ref opt
 *  \return Modifiers
 */
MCR_API unsigned int mcr_ModFlags_modifier(struct mcr_context *ctx,
		const char *name);
/*! Get the name of a modifier
 *
 *  \param modifier Modifiers
 *  \return Name of the modifier, or null if not found
 */
MCR_API const char *mcr_ModFlags_name(struct mcr_context *ctx,
									  unsigned int modifier);
/*! Get the number of modifiers that are mapped to a name
 *
 *  \param \ref retind
 */
MCR_API size_t mcr_ModFlags_count(struct mcr_context *ctx);

/* Modify modifiers */
/*! Combine an array of flags
 *
 *  \param modsArray \ref opt Array of flags
 *  \param length Number of flags to combine
 *  \return Combined flags
 */
MCR_API unsigned int mcr_ModFlags_combine(const unsigned int *modsArray,
		size_t length);
/*! Combine two flags
 *
 *  \param mods First set of flags
 *  \param newMod Additional set of flags
 *  \return Combined flags
 */
MCR_API unsigned int mcr_ModFlags_add(unsigned int mods, unsigned int newMod);
/*! True if flags contain a set of other flags
 *
 *  \param mods Current set of flags
 *  \param modVal Flags to be contained or not
 *  \return True if flags contain a set of other flags
 */
MCR_API bool mcr_ModFlags_has(unsigned int mods, unsigned int modVal);
/*! Remove one set of flags from another
 *
 *  \param mods Current set of flags
 *  \param delMod Flags to remove
 *  \return Current mods with the others removed
 */
MCR_API unsigned int mcr_ModFlags_remove(unsigned int mods,
		unsigned int delMod);

/*! Get the naming maps for modifiers and names.
 *
 *  Mostly for internal use
 *  \param mapModName \ref opt Map from modifiers to names
 *  \param mapNameMod \ref opt Map from names to modifiers
 */
MCR_API void mcr_ModFlags_maps(struct mcr_context *ctx,
							   struct mcr_Map **mapModName, struct mcr_Map **mapNameMod);

/*! See \ref mcr_ModFlags_add */
#define MCR_MOD_ADD(toModify, modifiers) \
((toModify) | (modifiers))
/*! See \ref mcr_ModFlags_has */
#define MCR_MOD_HAS(modifiers, hasMod) \
((modifiers & hasMod) == hasMod)
/*! See \ref mcr_ModFlags_remove */
#define MCR_MOD_REMOVE(toModify, modifiers) \
((toModify) & (~(modifiers)))

#ifdef __cplusplus
}
#endif

#endif
