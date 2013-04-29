
# ifndef MCR_WIN_SIGNAL_DEFINES_H
# define MCR_WIN_SIGNAL_DEFINES_H

// NO SECURE WARNINGS was not being properly included for windows.
# ifdef _MSC_VER
# define _CRT_SECURE_NO_WARNINGS
# endif

// For some reason hwheel is not always defined.
# ifndef MOUSEEVENTF_HWHEEL
# define MOUSEEVENTF_HWHEEL 0x01000
# endif

# endif
