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

# ifndef MCR_WIN_UTIL_DEFINES_H
# define MCR_WIN_UTIL_DEFINES_H

# include "util/def.h"

# include <WinSDKVer.h>

// Windows Version
//# define _WIN32_WINNT _WIN32_WINNT_WINXP	// 0x0501

# include <SDKDDKVer.h>

// Exclude rarely-used stuff from Windows headers.
# define WIN32_LEAN_AND_MEAN
# include <windows.h>

# ifdef MCR_STATIC
# define MCR_API
# else
	# ifdef MCR_EXPORTS
	# define MCR_API __declspec ( dllexport )
	# else
	# define MCR_API __declspec ( dllimport )
	# endif
# endif

# define snprintf _snprintf
# define strcasecmp _stricmp
# define strncasecmp _strnicmp

struct timespec
{
    int tv_sec ;
    int tv_nsec ;
};

# endif
