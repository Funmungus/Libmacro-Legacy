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

/*! \file intercept/win/intercept.h
 * \brief Read from grabbers and dispatch signals. This may block
 * incoming events.
 * */

# ifndef MCR_WIN_INTERCEPT_H
# define MCR_WIN_INTERCEPT_H

# include "intercept/intercept.h"
# include "intercept/win/grabber.h"

MCR_API void mcr_intercept_enable_key ( int enable ) ;
MCR_API void mcr_intercept_enable_move ( int enable ) ;
MCR_API void mcr_intercept_enable_scroll ( int enable ) ;

# endif // MCR_WIN_INTERCEPT_H
