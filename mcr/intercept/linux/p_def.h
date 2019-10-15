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
 *  Linux platform intercept definitions
 */

#ifndef MCR_INTERCEPT_LNX_NDEF_H_
#define MCR_INTERCEPT_LNX_NDEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mcr/standard/linux/p_standard.h"
#include "mcr/intercept/def.h"

#ifndef MCR_GRAB_SET_LENGTH
#define MCR_GRAB_SET_LENGTH 16
#endif

#define MCR_GRAB_SET_SIZE (sizeof(struct input_event) * \
	MCR_GRAB_SET_LENGTH)

#ifdef __cplusplus
}
#endif

#endif
