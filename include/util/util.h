/*! \file util/util.h
 * \brief Utilities for macro.
 */

# ifndef MCR_UTIL_H
# define MCR_UTIL_H

# include "util/mcrstrings.h"
# include "util/map.h"
# include "util/threads.h"

/*! \brief If cleaner cannot be registered in at_exit
 * program will be exited immediately.
 *
 * \param cleaner Will be called before program ends.
 * */
MCR_API void mcr_reg_cleanup ( void ( * cleaner ) ( void ) ) ;
/*! \brief \ref mcr_reg_cleanup With debug message for
 * offending file name on failure.
 *
 * \param cleaner Will be called before program ends.
 * */
MCR_API void mcr_reg_cleanup_filed ( void ( * cleaner ) ( void ),
		const char * fileName ) ;

/*! \brief For given bit return the corresponding array index.
 * Requires at least one "on" bit.
 *
 * \return Array index of bit value.
 * */
MCR_API unsigned int mcr_bit_index ( unsigned int bitval ) ;

/*! \brief For given array index return the corresponding bitwise value.
 *
 * \return Bit value of array index.
 * */
MCR_API unsigned int mcr_index_bit ( const unsigned int index ) ;

# endif
