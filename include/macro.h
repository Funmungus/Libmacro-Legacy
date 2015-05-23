/* include/macro.h - Include all macrolibrary modules.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file macro.h
 * \brief Include all release, or test-release macrolibrary
 * functionality. <br>
 * \namespace mcr
 *
 * Macrolibrary is created by Jonathan Pelletier. It will be released
 * as a public, open source project when it is ready.
 * */

# ifdef __cplusplus
extern "C" {
# endif

# include "intercept/intercept.h"
# include "macro/macro.h"
# ifdef MCR_EXTRAS
# include "extras/extras.h"
# endif

MCR_API void mcr_library_initialize ( int loadContracts ) ;
MCR_API void mcr_library_cleanup ( void ) ;

# ifdef __cplusplus
}
# endif
