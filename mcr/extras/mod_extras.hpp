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
 * \brief \ref mcr_mod_extras - Extras module
 *
 * In case of extreme complexity, break glass
 */

#ifndef MCR_MOD_EXTRAS_H
#define MCR_MOD_EXTRAS_H

#include "mcr/standard/mod_standard.h"

/*! \brief Extras module */
struct mcr_mod_extras {
	/*! \brief Signal interface for \ref mcr_Command */
	struct mcr_ISignal icommand;
	/*! \brief Signal interface for \ref mcr_StringKey */
	struct mcr_CtxISignal istring_key;
	/*! \brief Each element is an array of \ref mcr_Signal.
	 *
	 * This defines the behaviour of each character found in a
	 * \ref mcr_StringKey */
	struct mcr_Array key_chars;
};

MCR_API int mcr_extras_initialize(struct mcr_context *ctx);
MCR_API int mcr_extras_deinitialize(struct mcr_context *ctx);

/* TODO: Trim */
MCR_API int mcr_extras_load_contract(struct mcr_context *ctx);

/* Implement in platform directory */
/* Platform key definitions. */
MCR_API int mcr_StringKey_load_contract(struct mcr_context *ctx);

MCR_API int mcr_extras_platform_initialize(struct mcr_context *ctx);
MCR_API int mcr_extras_platform_deinitialize(struct mcr_context *ctx);

#endif
