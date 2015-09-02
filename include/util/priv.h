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

/*! \file util/priv.h
 * Set current operating permissions.
 * */

# ifndef MCR_PRIV_H
# define MCR_PRIV_H

# include "util/def.h"

//
// Defined for natively per OS
//
/*! \brief Disable privileges, and remove the ability to
 * enable them. */
MCR_API int mcr_privilege_deactivate ( ) ;
//! \brief \return 0 for unprivileged, otherwise application has privileges
MCR_API int mcr_privileged ( ) ;
/*! \brief Enable privileges */
MCR_API int mcr_set_privileged ( int enable ) ;

# endif
