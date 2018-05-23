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
 * \brief \ref mcr_mod_macro - Macro module
 */

#ifndef MCR_MACRO_MOD_MACRO_H_
#define MCR_MACRO_MOD_MACRO_H_

#include "mcr/util/util.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Macro module */
struct mcr_mod_macro {
	/*! \ref mcr_ITrigger registry */
	struct mcr_IRegistry itriggers;
};

MCR_API int mcr_macro_initialize(struct mcr_context *ctx);
MCR_API int mcr_macro_deinitialize(struct mcr_context *ctx);
MCR_API void mcr_macro_trim(struct mcr_context *ctx);

#ifdef __cplusplus
}
#endif

#endif
