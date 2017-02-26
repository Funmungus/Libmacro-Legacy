/* Libmacro - A multi-platform, extendable macro and hotkey C library
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

#ifndef MCR_UTIL_WIN_DEF_H
#define MCR_UTIL_WIN_DEF_H

#include "mcr/util/def.h"

/*# include <WinSDKVer.h> */

/* Windows Version */
/*# define _WIN32_WINNT _WIN32_WINNT_WINXP      // 0x0501 */

#include <SDKDDKVer.h>

/* Exclude rarely-used stuff from Windows headers. */
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#define mcr_snprintf _snprintf
#define mcr_casecmp _stricmp
#define mcr_ncasecmp _strnicmp

#ifndef __timespec_defined
#define __timespec_defined 1
#ifndef _TIMESPEC_DEFINED
#define _TIMESPEC_DEFINED
struct timespec {
	int tv_sec;
	int tv_nsec;
};
#endif
#endif

/* _Pragma undefined before 2015 */
#undef fixme
#define fixme

#endif