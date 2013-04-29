
/*! \def MCR_UINPUT_PATH
 * Default value for \ref Device::uinputPath when library is built.
 * */
# ifndef MCR_UINPUT_PATH
# define MCR_UINPUT_PATH /dev/uinput
# endif
/*! \def MCR_EVENT_PATH
 * Default value for \ref Device::eventPath when library is built.
 * */
# ifndef MCR_EVENT_PATH
# define MCR_EVENT_PATH /dev/input
# endif
/*! \def MCR_ABS_RESOLUTION
 * Default value for \ref AbsDevice::resolution when library is built.
 * Default is 65535, or 0xFFFF
 * */
# ifndef MCR_ABS_RESOLUTION
# define MCR_ABS_RESOLUTION 0xFFFF
# endif

# include <dirent.h> // Find uinput dev node after creation.
# include <linux/input.h> // Event definitions.
# include <linux/joystick.h> // EVIO joysticks
# include <linux/limits.h>
# include <linux/uinput.h> // Create udev.
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h> // File control, open, create, read, write.
