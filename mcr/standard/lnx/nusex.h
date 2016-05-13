/* Libmacro - A multi-platform, extendable macro and hotkey C library.
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

#ifndef MCR_LNX_USEX_H
#define MCR_LNX_USEX_H

#include "mcr/standard/lnx/ndef.h"

/* Install senders to replace standard ones. */
MCR_API int mcr_enable_x(struct mcr_context *ctx, bool enable);
MCR_API int mcr_HidEcho_send_x(mcr_Signal * sigPt);
MCR_API int mcr_Key_send_x(mcr_Signal * sigPt);
MCR_API int mcr_MoveCursor_send_x(mcr_Signal * sigPt);
MCR_API int mcr_Scroll_send_x(mcr_Signal * sigPt);

MCR_API int mcr_usex_initialize(struct mcr_context *ctx);
MCR_API void mcr_usex_cleanup(struct mcr_context *ctx);

#endif
