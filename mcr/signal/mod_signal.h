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

/*! \file
 * \brief \ref mcr_mod_signal - Signal module
 *
 * In cases of extreme complexity, please break glass.
 */

#ifndef MCR_MOD_SIGNAL_H
#define MCR_MOD_SIGNAL_H

#include "mcr/util/util.h"

struct mcr_ISignal;
struct mcr_Dispatcher;
/*! \brief Signal module */
struct mcr_mod_signal {
	/*! \brief \ref mcr_ISignal registry */
	struct mcr_IRegistry isignals;
	/*! \brief Set of \ref mcr_Dispatcher * for each signal */
	struct mcr_Array dispatchers;
	/*! \brief If enabled, the generic dispatcher will be used for all
	 * signals */
	bool dispatcher_generic_enabled;
	/*! \brief Generic dispatcher which may be used after the specific
	 * dispatcher has been called */
	struct mcr_Dispatcher *dispatcher_generic_pt;
	/*! \brief All modifiers known by Libmacro to be set */
	unsigned int internal_mods;
	/*! \brief Map from modifiers to names */
	struct mcr_Map map_mod_name;
	/*! \brief Map from names to modifiers */
	struct mcr_Map map_name_mod;
};

MCR_API int mcr_signal_initialize(struct mcr_context *ctx);
MCR_API int mcr_signal_deinitialize(struct mcr_context *ctx);
MCR_API int mcr_signal_load_contract(struct mcr_context *ctx);
MCR_API void mcr_signal_trim(struct mcr_context *ctx);

#endif
