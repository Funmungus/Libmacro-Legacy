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
 *  \brief Intercept reads from hardware, and may block.
 */

#ifndef MCR_INTERCEPT_INTERCEPT_H_
#define MCR_INTERCEPT_INTERCEPT_H_

#include "mcr/intercept/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Set known modifiers from hardware values. */
MCR_API void mcr_intercept_reset_modifiers(struct mcr_context *ctx);
/*! \ref mcr_intercept.blockable */
MCR_API bool mcr_intercept_is_blockable(struct mcr_context *ctx);
/*! \ref mcr_intercept.blockable */
MCR_API void mcr_intercept_set_blockable(struct mcr_context *ctx, bool enable);
/*! If currently enabled this will reset all current hardware intercepts. */
MCR_API int mcr_intercept_reset(struct mcr_context *ctx);

/* platform */
/*! Is any hardware intercepting.
 *
 *  \ref mcr_is_platform
 */
MCR_API bool mcr_intercept_is_enabled(struct mcr_context *ctx);
/*! Enable intercept from hardware.  Refer to platform functions to set which
 *  hardware to intercept.
 *
 *  \ref mcr_is_platform
 */
MCR_API int mcr_intercept_set_enabled(struct mcr_context *ctx, bool enable);
/*! Modifiers reported by hardware or OS.
 *
 *  \ref mcr_is_platform
 */
MCR_API unsigned int mcr_intercept_modifiers(struct mcr_context *ctx);

/*! Intercept module
 *
 *  In cases of extreme complexity, please break glass.
 */
struct mcr_intercept {
	/*! Is hardware intercept grabbing hardware, and is blocking available.
	 *
	 *  Default false for OS compatibility. */
	bool blockable;
	/*! All data reserved for platform definitions */
	void *platform;
};

#ifdef __cplusplus
}
#endif

#endif
