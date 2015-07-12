/* include/macro/def.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file macro/def.h
 * \brief Definitions for macros.
 */

# ifndef MCR_MACRO_DEFINES_H
# define MCR_MACRO_DEFINES_H

# include "hotkey/hotkey.h"

/* For better security no macro thread count will ever
 * be greater than this number. EVER!!! */
# ifndef MCR_THREAD_MAX
# define MCR_THREAD_MAX 0x10
# endif

# ifndef MCR_MACRO_JOIN_TIMEOUT
# define MCR_MACRO_JOIN_TIMEOUT 2
# endif

# endif
