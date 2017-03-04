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
 * \brief In cases of extreme complexity, please break glass.
 */

#ifndef MCR_STANDARD_PRIVATE_H
#define MCR_STANDARD_PRIVATE_H

#include "mcr/util/util.h"

MCR_API int mcr_standard_initialize(struct mcr_context *ctx);
MCR_API int mcr_standard_deinitialize(struct mcr_context *ctx);
MCR_API int mcr_standard_load_contract(struct mcr_context *ctx);
MCR_API void mcr_standard_trim(struct mcr_context *ctx);
/* Key-related, but not interface */
MCR_API int mcr_Key_initialize(struct mcr_context *ctx);
MCR_API int mcr_Key_deinitialize(struct mcr_context *ctx);
/* Echo-related, but not interface */
MCR_API int mcr_HidEcho_initialize(struct mcr_context *ctx);
MCR_API int mcr_HidEcho_deinitialize(struct mcr_context *ctx);

/* Native directory */
#define MCR_STANDARD_NATIVE_INC \
MCR_STR(mcr/standard/MCR_NATIVE_DIR/nstandard.h)
/*! \brief Initialize native requirements for standard signals.
 *
 * Called by \ref mcr_standard_initialize
 */
MCR_API int mcr_standard_native_initialize(struct mcr_context *ctx);
/*! \brief Clean up native requirements for standard signals.
 *
 * Called by \ref mcr_standard_deinitialize
 */
MCR_API int mcr_standard_native_deinitialize(struct mcr_context *ctx);
/*! \brief Load native key names and key <=> modifier mapping. */
MCR_API int mcr_Key_load_contract(struct mcr_context *ctx);
/*! \brief Load native echo names. */
MCR_API int mcr_HidEcho_load_contract(struct mcr_context *ctx);

/* Native signal */
MCR_API int mcr_HidEcho_send_data(struct mcr_HidEcho *dataPt);
MCR_API int mcr_Key_send_data(struct mcr_Key *dataPt);
MCR_API int mcr_MoveCursor_send_data(struct mcr_MoveCursor *dataPt);
MCR_API int mcr_Scroll_send_data(struct mcr_Scroll *dataPt);

#endif
