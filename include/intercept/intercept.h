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

/*! \file intercept/intercept.h
 * Intercept reads from hardware, and may block.
 * */

# ifndef MCR_INTERCEPT_H
# define MCR_INTERCEPT_H

# include "intercept/def.h"

MCR_API void mcr_intercept_reset_mods ( ) ;

MCR_API void mcr_intercept_initialize ( ) ;
MCR_API void mcr_intercept_cleanup ( void ) ;

//
// Defined in native source.
//
MCR_API int mcr_intercept_enabled ( ) ;
MCR_API void mcr_intercept_set_enabled ( int enable ) ;
MCR_API unsigned int mcr_intercept_get_mods ( ) ;
MCR_API void mcr_intercept_native_initialize ( ) ;
MCR_API void mcr_intercept_native_cleanup ( void ) ;

# endif // MCR_INTERCEPT_H
