/*! \file macro/def.h
 * \brief Definitions for macros.
 */

# ifndef FUN_MACRO_DEFINES
# define FUN_MACRO_DEFINES

# ifdef WIN

# include <WinSDKVer.h>

// Windows Version
# define _WIN32_WINNT _WIN32_WINNT_WINXP	// 0x0501

# include <SDKDDKVer.h>

# define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
# include <windows.h>

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FUN_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FUN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

# ifdef MACRO_STATIC
# define MACRO_API
# else
	# ifdef MACRO_EXPORTS
	# define MACRO_API __declspec(dllexport)
	# else
	# define MACRO_API __declspec(dllimport)
	# endif
# endif

# endif

# ifndef MACRO_API				// If not using windows, or define FUN_API as nothing.
					// The result is as if FUN_API is not there on other platforms.
# define MACRO_API
# endif

# include "hotkey/hotkey.h"

# endif

