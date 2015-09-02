/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
  Copyright (C) 2013  Jonathan D. Pelletier

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

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
# include <dirent.h> // Find uinput dev node after creation
# include <linux/input.h> // Event definitions
# include <linux/joystick.h> // EVIO joysticks
# include <linux/limits.h>
# include <linux/uinput.h> // Create udev
# include <sys/stat.h>
# include <fcntl.h> // File control, open, create, read, write

# endif // MCR_LNX_SIGNAL_DEFINES_H
