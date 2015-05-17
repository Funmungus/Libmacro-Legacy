/* include/intercept/intercept.h - Module to read or block hardware signals.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

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
MCR_API int mcr_intercept_is_enabled ( ) ;
MCR_API unsigned int mcr_intercept_get_mods ( ) ;
MCR_API void mcr_intercept_native_initialize ( ) ;
MCR_API void mcr_intercept_native_cleanup ( void ) ;

# endif // MCR_INTERCEPT_H
