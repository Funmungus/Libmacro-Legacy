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
 *  \brief Libmacro modifiers
 */

#ifndef MCR_SIGNAL_MODIFIERS_H_
#define MCR_SIGNAL_MODIFIERS_H_

#include "mcr/signal/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Reference to Libmacro internal modifiers. */
MCR_API unsigned int *mcr_modifiers(struct mcr_context *ctx);
/*! Add modifiers to current
 *
 *  \param addMods Modifiers to add
 */
MCR_API void mcr_add_modifiers(struct mcr_context *ctx, unsigned int addMods);
/*! Remove modifiers from current
 *
 *  \param remMods Modifiers to remove
 */
MCR_API void mcr_remove_modifiers(struct mcr_context *ctx,
								  unsigned int remMods);

#ifdef __cplusplus
}
#endif

#endif
