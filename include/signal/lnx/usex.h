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

# ifndef MCR_LNX_USEX_H
# define MCR_LNX_USEX_H
# ifdef MCR_USEX

# include "X11/keysym.h"
# include "X11/keysymdef.h"
# include "signal/lnx/def.h"

/* Mods known by XLib */
extern unsigned int mcr_xMods ;

// Install senders to replace standard ones.
MCR_API void mcr_usex_enable ( int enable ) ;
MCR_API int mcr_HIDEcho_usex_send ( mcr_Signal * sigPt ) ;
MCR_API int mcr_Key_usex_send ( mcr_Signal * sigPt ) ;
MCR_API int mcr_MoveCursor_usex_send ( mcr_Signal * sigPt ) ;
MCR_API int mcr_Scroll_usex_send ( mcr_Signal * sigPt ) ;

MCR_API void mcr_usex_initialize ( ) ;
MCR_API void mcr_usex_cleanup ( void ) ;

# endif // MCR_USEX
# endif // MCR_LNX_USEX_H