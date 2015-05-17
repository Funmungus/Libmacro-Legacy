/* include/intercept/lnx/intercept.h - Read or block from grabbers and dispatch.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file intercept/lnx/intercept.h
 * \brief Read from grabbers and dispatch signals. This may block
 * incoming events.
 * */

# ifndef MCR_LNX_INTERCEPT_H
# define MCR_LNX_INTERCEPT_H

# include "intercept/lnx/grabber.h"

MCR_API void mcr_intercept_grab ( const char * grabPath ) ;
MCR_API void mcr_intercept_set_grabs ( const char ** allGrabPaths,
		size_t pathCount ) ;

# endif // MCR_LNX_INTERCEPT_H
