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

/*! \file macro.h
 * \brief Include all release, or test-release macrolibrary
 * functionality. <br>
 * \namespace mcr
 *
 * Macrolibrary is created by Jonathan Pelletier. It will be released
 * as a public, open source project when it is ready.
 * */

# ifdef __cplusplus
extern "C" {
# endif

# include "intercept/intercept.h"
# include "macro/macro.h"
# ifdef MCR_EXTRAS
# include "extras/extras.h"
# endif

MCR_API void mcr_library_initialize ( int loadContracts ) ;
MCR_API void mcr_library_cleanup ( void ) ;

# ifdef __cplusplus
}
# endif
