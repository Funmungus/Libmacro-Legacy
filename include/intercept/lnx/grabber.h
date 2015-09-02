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

/*! \file intercept/lnx/grabber.h
 * \brief Grabber, Take exclusive access to a /dev/input event.
 * */

# ifndef MCR_LNX_GRABBER_H
# define MCR_LNX_GRABBER_H

# include "intercept/lnx/def.h"

//! \brief Take exclusive access to a /dev/input event.
typedef struct mcr_Grabber
{
	//! \brief File of the created input event
	int fd ;
	//! \brief File path of input event
	mcr_Array path ;
} mcr_Grabber ;

//! \brief ctor
MCR_API void mcr_Grabber_init ( mcr_Grabber * grabPt ) ;
MCR_API void mcr_Grabber_init_with ( mcr_Grabber * grabPt,
		const char * path, int enable ) ;
MCR_API void mcr_Grabber_free ( mcr_Grabber * grabPt ) ;

MCR_API const char * mcr_Grabber_path ( mcr_Grabber * grabPt ) ;
MCR_API void mcr_Grabber_set_path ( mcr_Grabber * grabPt,
		const char * path ) ;
/*! \brief Get ensured enabled state, and set the grabber enabled
 * state to the same. */
MCR_API int mcr_Grabber_enabled ( mcr_Grabber * grabPt ) ;
/*! \brief Allow or disallow this object to function.
 *
 * \return 0 if unsuccessful, otherwise successful.
 * */
MCR_API int mcr_Grabber_set_enabled ( mcr_Grabber * grabPt, int enable ) ;
/*! \brief Obtain the current key state of this device.
 *
 * \param size bytesize of buffer.
 * */
MCR_API void mcr_Grabber_state ( mcr_Grabber * grabPt,
		char * buffer, const unsigned int size ) ;

# define MCR_EVENTINDEX( keyCode ) \
	( ( keyCode ) / 8 )

# define MCR_EVENTBIT( keyCode ) \
	( 1 << ( ( keyCode ) % 8 ) )

# define MCR_GRABBER_ENABLED( grab ) \
	( ( grab ).fd != -1 )
# define mcr_Grabber_enabled_impl( grabPt ) \
	( ( grabPt )->fd != -1 )

# endif
