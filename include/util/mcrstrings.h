/*! \file util/mcrstrings.h
 * \brief String functions for macro.
 */

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
