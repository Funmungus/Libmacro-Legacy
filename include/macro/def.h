/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
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

/*! \file macro/def.h
 * \brief Definitions for macros
 */

# ifndef MCR_MACRO_DEFINES_H
# define MCR_MACRO_DEFINES_H

# include "hotkey/hotkey.h"

/* For better security no macro thread count will ever
 * be greater than this number, EVER!!! */
# ifndef MCR_THREAD_MAX
# define MCR_THREAD_MAX 0x10
# endif

# ifndef MCR_MACRO_JOIN_TIMEOUT
# define MCR_MACRO_JOIN_TIMEOUT 2
# endif

# endif
