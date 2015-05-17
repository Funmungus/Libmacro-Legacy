/* include/util/mcrstrings.h - Define missing string functions.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file util/mcrstrings.h
 * \brief String functions for macrolibrary.
 * */

# ifndef MCR_STRINGS_H
# define MCR_STRINGS_H

# include "util/def.h"

/*! \brief Convert a C-string into boolean true/false
 *
 * A string value of false or true is not case sensitive.
 * An integer value will be placed into retVal directly.
 * \param retVal true/false return value of string value.
 * \return 1 on success, 0 on failure ( not valid conversion )
 */
MCR_API int mcr_convert_bool ( const char * value, int * retVal ) ;

# include STRINGIFY(util/MCR_NATIVE_DIR/mcrstrings.h)

# endif
