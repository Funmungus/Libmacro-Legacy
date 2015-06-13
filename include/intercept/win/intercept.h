/* include/intercept/win/intercept.h - Dispatch from grabbers.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

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
