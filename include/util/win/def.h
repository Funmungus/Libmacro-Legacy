/* include/util/win/def.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file util/win/def.h
 * */

# ifndef MCR_WIN_UTIL_DEFINES_H
# define MCR_WIN_UTIL_DEFINES_H

struct timespec
{
	time_t tv_sec ;
	long tv_nsec ;
} ;

# ifdef _MSC_VER
# define _CRT_SECURE_NO_WARNINGS
# endif

# include <WinSDKVer.h>

// Windows Version
# define _WIN32_WINNT _WIN32_WINNT_WINXP	// 0x0501

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

# endif
