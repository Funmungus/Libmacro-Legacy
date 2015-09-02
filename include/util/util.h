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

/*! \file util/util.h
 * \brief Utilities for macrolibrary
 * */

# ifndef MCR_UTIL_H
# define MCR_UTIL_H

# include "util/mcrstrings.h"
# include "util/interface.h"
# include "util/threads.h"
# include "util/priv.h"

/*! \brief Load mcr_in, mcr_out, and mcr_err with stdout, stdin,
 * and stderr.
 * */
MCR_API void mcr_set_stdio ( ) ;
/*! \brief If cleaner cannot be registered in at_exit, the
 * program will be exited immediately.
 *
 * \param cleaner Will be called before program ends
 * */
MCR_API void mcr_reg_cleanup ( void ( * cleaner ) ( void ) ) ;

/*! \brief For given bit return the corresponding array index.
 * Requires at least one "on" bit
 *
 * \return Array index of bit value
 * */
MCR_API unsigned int mcr_bit_index ( unsigned int bitval ) ;

/*! \brief For given array index return the corresponding bitwise value.
 *
 * \return Bit value of array index
 * */
MCR_API unsigned int mcr_index_bit ( const unsigned int index ) ;

# endif
