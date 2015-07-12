/* include/intercept/lnx/def.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file intercept/lnx/def.h
 * */

# ifndef MCR_LNX_INTERCEPT_DEFINES_H
# define MCR_LNX_INTERCEPT_DEFINES_H

# include "intercept/def.h"
# include "signal/lnx/signal.h"
# include <poll.h>

# ifndef MCR_GRAB_SET_LENGTH
# define MCR_GRAB_SET_LENGTH 16
# endif

# define MCR_GRAB_SET_SIZE ( sizeof ( struct input_event ) * \
		MCR_GRAB_SET_LENGTH )

# endif // MCR_LNX_INTERCEPT_DEFINES_H
