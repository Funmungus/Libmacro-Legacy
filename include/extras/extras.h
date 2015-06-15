/* include/extras/extras.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# ifndef MCR_EXTRAS_H
# define MCR_EXTRAS_H

# include "extras/macroset.h"

MCR_API void mcr_extras_initialize ( ) ;
MCR_API void mcr_extras_cleanup ( void ) ;

# ifndef MCR_EXTRAS
// TODO: Shared library initialize function.
# endif

//
// Implement in native directory
//
MCR_API void mcr_extras_native_initialize ( ) ;
MCR_API void mcr_extras_native_cleanup ( void ) ;

# endif // MCR_EXTRAS_H
