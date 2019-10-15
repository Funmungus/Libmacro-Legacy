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
 *  Set current operating permissions.
 */

#ifndef MCR_UTIL_PRIV_H_
#define MCR_UTIL_PRIV_H_

#include "mcr/util/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Disable privileges, and remove the ability to
 *  enable them.
 *
 *  \ref mcr_is_platform
 *  \return \ref reterr
 */
MCR_API int mcr_privilege_deactivate();

#ifdef __cplusplus
}
#endif

#endif
