/*! \file util/def.h
 * \brief Definitions for utilities.
 */

# ifndef FUN_UTIL_DEFINES
# define FUN_UTIL_DEFINES

# ifdef WIN

# include <WinSDKVer.h>

// Windows Version
# define _WIN32_WINNT _WIN32_WINNT_WINXP	// 0x0501
// Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

# include <SDKDDKVer.h>

# define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
# include <windows.h>

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FUN_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FUN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
# ifdef UTIL_STATIC
# define UTIL_API 
# else
	# ifdef UTIL_EXPORTS
	# define UTIL_API __declspec(dllexport)
	# else
	# define UTIL_API __declspec(dllimport)
	# endif
# endif

# endif

# ifndef UTIL_API // If not using windows
					// The result is as if FUN_API is not there on other platforms.
# define UTIL_API 
# endif

# include <cstdint>
# include <cstdio>
# include <iostream>
# include <locale>
# include <memory>
# include <sstream>
# include <string>
# include <string.h>

# ifdef WIN
# include <fstream>
# endif

# ifdef LNX
# include <fcntl.h>
# include <unistd.h>
# endif

# endif

