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
 *  In cases of extreme complexity, please break glass.
 */

#ifndef MCR_SIGNAL_PRIVATE_H_
#define MCR_SIGNAL_PRIVATE_H_

#include "mcr/signal/def.h"

#ifdef __cplusplus
extern "C" {
#endif

MCR_API int mcr_signal_initialize(struct mcr_context *ctx);
MCR_API int mcr_signal_deinitialize(struct mcr_context *ctx);
MCR_API int mcr_signal_load_contract(struct mcr_context *ctx);
MCR_API void mcr_signal_trim(struct mcr_context *ctx);

#ifdef __cplusplus
}
#endif

#endif
