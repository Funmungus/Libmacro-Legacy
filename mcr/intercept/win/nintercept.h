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

#ifndef MCR_WIN_INTERCEPT_H
#define MCR_WIN_INTERCEPT_H

#include "mcr/intercept/win/ngrabber.h"

#define MCR_GRAB_COUNT 3

struct mcr_mod_intercept_platform {
	struct mcr_Grabber *grab_key;
	struct mcr_Grabber *grab_move;
	struct mcr_Grabber *grab_scroll;
	struct mcr_Grabber *all_grabbers[MCR_GRAB_COUNT];
};

MCR_API bool mcr_intercept_key_is_enabled(struct mcr_context *ctx);
MCR_API int mcr_intercept_key_set_enabled(struct mcr_context *ctx, bool enable);
MCR_API bool mcr_intercept_move_is_enabled(struct mcr_context *ctx);
MCR_API int mcr_intercept_move_set_enabled(struct mcr_context *ctx,
	bool enable);
MCR_API bool mcr_intercept_scroll_is_enabled(struct mcr_context *ctx);
MCR_API int mcr_intercept_scroll_set_enabled(struct mcr_context *ctx,
	bool enable);

#endif
