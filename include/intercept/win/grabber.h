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

/*! \file intercept/win/grabber.h
 * \brief Hooks are for intercepting events in Windows.
 * */

# ifndef MCR_WIN_GRABBER_H
# define MCR_WIN_GRABBER_H

# include "intercept/win/def.h"

typedef struct mcr_Grabber
{
//	mtx_t lock ; /* Mutex locking broken on windows? */
	HMODULE hModule ;
	HHOOK id ;
	HOOKPROC proc ;
	int type ;
	HANDLE hThread ;
	DWORD dwThread ;
} mcr_Grabber ;

MCR_API void mcr_Grabber_init ( mcr_Grabber * grabPt ) ;
MCR_API void mcr_Grabber_init_with ( mcr_Grabber * grabPt, int type,
		HOOKPROC proc ) ;
MCR_API void mcr_Grabber_free ( mcr_Grabber * grabPt ) ;
MCR_API int mcr_Grabber_enabled ( mcr_Grabber * grabPt ) ;
MCR_API void mcr_Grabber_set_enabled ( mcr_Grabber * grabPt, int enable ) ;

# define MCR_GRABBER_ENABLED( grab ) \
	( ( grab ).id != NULL )
# define mcr_Grabber_enabled_impl( grabPt ) \
	( ( grabPt )->id != NULL )

# endif
