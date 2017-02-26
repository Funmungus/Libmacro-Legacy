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
 * \brief Read from grabbers and dispatch signals. This may block
 * incoming events.
 */

#ifndef MCR_LNX_INTERCEPT_H
#define MCR_LNX_INTERCEPT_H

#include "mcr/intercept/lnx/ngrabber.h"

struct mcr_mod_intercept_native {
	mtx_t lock;
	cnd_t cnd;
	struct mcr_Array grab_completes;
	mcr_StringSet grab_paths;
	/* KEY_CNT / 8 is a floor value, and may have remainder of keys. */
	char bit_retrieval[MCR_EVENTINDEX(KEY_CNT) + 1];
};

MCR_API int mcr_intercept_add_grab(struct mcr_context *ctx,
	const char *grabPath);
MCR_API void mcr_intercept_remove_grab(struct mcr_context *ctx,
	const char *grabPath);
MCR_API int mcr_intercept_set_grabs(struct mcr_context *ctx,
	const char **allGrabPaths, size_t pathCount);

#endif
