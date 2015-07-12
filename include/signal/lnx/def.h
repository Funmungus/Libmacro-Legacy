/* include/signal/lnx/def.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# ifndef MCR_LNX_SIGNAL_DEFINES_H
# define MCR_LNX_SIGNAL_DEFINES_H

/*! \def MCR_UINPUT_PATH
 * Default value for uinput node when library is built.
 * */
# ifndef MCR_UINPUT_PATH
# define MCR_UINPUT_PATH /dev/uinput
# endif
/*! \def MCR_EVENT_PATH
 * Default value for device input event path when library is built.
 * */
# ifndef MCR_EVENT_PATH
# define MCR_EVENT_PATH /dev/input
# endif
/*! \def MCR_ABS_RESOLUTION
 * Default value for \ref mcr_AbsDev resolution when library is built.
 * Default is 4095, or 0x0FFF
 * */
# ifndef MCR_ABS_RESOLUTION
# define MCR_ABS_RESOLUTION 0x0FFF
# endif

# include "util/lnx/util.h"
# include "signal/def.h"
# include <dirent.h> // Find uinput dev node after creation.
# include <linux/input.h> // Event definitions.
# include <linux/joystick.h> // EVIO joysticks
# include <linux/limits.h>
# include <linux/uinput.h> // Create udev.
# include <sys/stat.h>
# include <fcntl.h> // File control, open, create, read, write.

# endif // MCR_SIGNAL_DEFINES_H
