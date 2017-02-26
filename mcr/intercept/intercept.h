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
 * Intercept reads from hardware, and may block.
 */

#ifndef MCR_INTERCEPT_H
#define MCR_INTERCEPT_H

#include "mcr/intercept/def.h"

MCR_API void mcr_intercept_reset_modifiers(struct mcr_context *ctx);

/* Defined in native source. */
MCR_API bool mcr_intercept_is_enabled(struct mcr_context *ctx);
MCR_API int mcr_intercept_set_enabled(struct mcr_context *ctx, bool enable);
MCR_API unsigned int mcr_intercept_modifiers(struct mcr_context *ctx);

#endif
