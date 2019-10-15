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

#ifndef MCR_STANDARD_LNX_NSTANDARD_H_
#define MCR_STANDARD_LNX_NSTANDARD_H_

#include "mcr/standard/linux/p_device.h"

#ifdef __cplusplus
extern "C" {
#endif

/* In order to inline we cannot privatize this. */
extern MCR_API struct mcr_Array mcr_echoEvents;
extern MCR_API struct mcr_Map mcr_keyToEcho[2];
extern MCR_API mcr_SpacePosition mcr_cursor;

#ifdef __cplusplus
}
#endif

#endif
