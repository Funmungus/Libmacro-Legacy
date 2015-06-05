/* include/util/util.h - Module for helper function utilities.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file util/util.h
 * \brief Utilities for macrolibrary.
 * */

# ifndef MCR_UTIL_H
# define MCR_UTIL_H

# include "util/mcrstrings.h"
# include "util/interface.h"
# include "util/threads.h"

MCR_API void mcr_set_stdio ( ) ;
/*! \brief If cleaner cannot be registered in at_exit
 * program will be exited immediately.
 *
 * \param cleaner Will be called before program ends.
 * */
MCR_API void mcr_reg_cleanup ( void ( * cleaner ) ( void ) ) ;

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
