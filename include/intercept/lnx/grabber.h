/* include/intercept/lnx/grabber.h - Take exclusive access to a /dev/input event.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file intercept/lnx/grabber.h
 * \brief Grabber, Take exclusive access to a /dev/input event.
 * */

# ifndef MCR_LNX_GRABBER_H
# define MCR_LNX_GRABBER_H

# include "intercept/lnx/def.h"

//! \brief Take exclusive access to a /dev/input event.
typedef struct mcr_Grabber
{
	//! \brief File of the created input event.
	int fd ;
	//! \brief File path of input event.
	mcr_Array path ;
} mcr_Grabber ;

//! \brief ctor
MCR_API void mcr_Grabber_init ( mcr_Grabber * grabPt ) ;
MCR_API void mcr_Grabber_init_with ( mcr_Grabber * grabPt,
		const char * path, int enable ) ;
MCR_API void mcr_Grabber_free ( mcr_Grabber * grabPt ) ;

MCR_API const char * mcr_Grabber_get_path ( mcr_Grabber * grabPt ) ;
MCR_API void mcr_Grabber_set_path ( mcr_Grabber * grabPt,
		const char * path ) ;
/*! \brief Get ensured enabled state, and set the grabber enabled
 * state to the same. */
MCR_API int mcr_Grabber_is_enabled ( mcr_Grabber * grabPt ) ;
/*! \brief Allow or disallow this object to function.
 *
 * \return 0 if unsuccessful, otherwise successful.
 * */
MCR_API int mcr_Grabber_enable ( mcr_Grabber * grabPt, int enable ) ;
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

# define MCR_GRABBER_ENABLED( grabPt ) \
	( ( grabPt )->fd != -1 )

# endif
