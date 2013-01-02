/*! \file funstrings.h
 * \brief String functions for funlibrary.
 */

# ifndef FUN_STRINGS_H
# define FUN_STRINGS_H

# include "util/def.h"

namespace funlibrary
{
	/*! \brief Check in the container string for a string value at given position.
	 *
	 * \param container Container to check in.
	 * \param pos Position to check from.
	 * \param value Value to check for in container.
	 * \param ignoreCase If true, characters will be checked with tolower.
	 * Else this function is case sensitive.
	 * \return True if value is in container at index of pos.
	*/
	UTIL_API bool containsAt ( const char * container, unsigned int pos, const char * value,
		bool ignoreCase ) ;


	/*! \brief Convert a C-string into boolean true/false
	 *
	 * A string value of false or true is not case sensitive.
	 * An integer value 0 is false, otherwise an integer value is
	 * always true.
	 */
	UTIL_API bool convertBool ( const std::string & value ) ;
}

# endif
