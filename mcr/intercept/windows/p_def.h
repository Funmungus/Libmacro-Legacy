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
 */

#ifndef MCR_INTERCEPT_WIN_NDEF_H_
#define MCR_INTERCEPT_WIN_NDEF_H_

#include "mcr/standard/windows/p_standard.h"
#include "mcr/intercept/def.h"

#ifndef WM_MOUSEHWHEEL
	#define WM_MOUSEHWHEEL 0x020E
#endif

#ifndef MCR_INTERCEPT_WAIT_MILLIS
	#define MCR_INTERCEPT_WAIT_MILLIS 5000
#endif

#endif
