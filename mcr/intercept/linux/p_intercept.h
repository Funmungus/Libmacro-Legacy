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

#ifndef MCR_INTERCEPT_LNX_NINTERCEPT_H_
#define MCR_INTERCEPT_LNX_NINTERCEPT_H_

#include "mcr/intercept/linux/p_grabber.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Linux intercept platform structure */
struct mcr_intercept_platform {
	/*! Intercept critical sections */
	mtx_t lock;
	/*! Ordered list so each grab can remove itself */
	struct mcr_Array grab_contexts;
	/*! Set of input event paths to try grabbing */
	mcr_StringSet grab_paths;
	/*! Get key pressed values from a device
	 *
	 *  KEY_CNT / 8 is a floor value, and may have remainder of keys. */
	char bit_retrieval[MCR_EVENTINDEX(KEY_CNT) + 1];
};
/* All use /dev/input/eventX files to read from */
MCR_API int mcr_intercept_add_grab(struct mcr_context *ctx,
								   const char *grabPath);
MCR_API void mcr_intercept_remove_grab(struct mcr_context *ctx,
									   const char *grabPath);
MCR_API int mcr_intercept_set_grabs(struct mcr_context *ctx,
									const char **allGrabPaths, size_t pathCount);

#ifdef __cplusplus
}
#endif

#endif
