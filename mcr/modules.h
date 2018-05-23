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
 * \brief Structures used by Libmacro.
 */

#ifndef MCR_MODULES_H_
#define MCR_MODULES_H_

/* Define all module structures */
#include "mcr/signal/mod_signal.h"
#include "mcr/macro/mod_macro.h"
#include "mcr/standard/mod_standard.h"
#include "mcr/intercept/mod_intercept.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Required library context for Libmacro functions */
struct mcr_context {
	struct mcr_mod_signal signal;
	struct mcr_mod_macro macro;
	struct mcr_mod_standard standard;
	struct mcr_mod_intercept intercept;
};

#ifdef __cplusplus
}
#endif

#endif
