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
 *  \brief Raise a compiler warning.
 *  Usage:
 *  `#include "mcr/warn.h"`
 */

#ifdef __GNUC__
	#warning Warning!
#elif defined(__clang__)
	#pragma GCC warning "Warning!"
#else
	/* MSVC-specific warning, but will at least print a message for
	* unknown compilers.
	*/
	#pragma message "Warning Msg: Warning!"
#endif
