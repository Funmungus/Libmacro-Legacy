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
 *  \brief Read from grabbers and dispatch signals. This may block
 *  incoming events.
 */

#ifndef MCR_INTERCEPT_WIN_NINTERCEPT_H_
#define MCR_INTERCEPT_WIN_NINTERCEPT_H_

#include "mcr/intercept/windows/p_grabber.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MCR_GRAB_COUNT 2

struct mcr_intercept_platform {
	struct mcr_Grabber *grab_key;
	struct mcr_Grabber *grab_mouse;
	struct mcr_Grabber *all_grabbers[MCR_GRAB_COUNT];
};

MCR_API bool mcr_intercept_key_is_enabled(struct mcr_context *ctx);
MCR_API int mcr_intercept_key_set_enabled(struct mcr_context *ctx, bool enable);
MCR_API bool mcr_intercept_mouse_is_enabled(struct mcr_context *ctx);
MCR_API int mcr_intercept_move_set_enabled(struct mcr_context *ctx,
		bool enable);

#ifdef __cplusplus
}
#endif

#endif
