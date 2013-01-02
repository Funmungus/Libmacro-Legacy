/*! \file hotkey/def.h
 * \brief Definitions for hotkeys.
 */

# ifndef FUN_HOTKEY_DEFINES
# define FUN_HOTKEY_DEFINES

# ifdef WIN

/* WIN SDK defined in util.
# include <WinSDKVer.h>

// Windows Version
# define _WIN32_WINNT _WIN32_WINNT_WINXP	// 0x0501

# include <SDKDDKVer.h>

# define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers.
# include <windows.h>
*/
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FUN_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FUN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

# ifdef HOTKEY_STATIC
# define HOTKEY_API 
# else
	# ifdef HOTKEY_EXPORTS
	# define HOTKEY_API __declspec(dllexport)
	# else
	# define HOTKEY_API __declspec(dllimport)
	# endif
# endif

# endif

# ifndef HOTKEY_API				// If not using windows, or define FUN_API as nothing.
					// The result is as if FUN_API is not there on other platforms.
# define HOTKEY_API 
# endif

# include <utility> // for move
# include "signal/signal.h"

namespace funlibrary
{
	//! \brief Callback for KeySignals, return true for blocking/dropping the signal.
	typedef bool ( * KeyCallback ) ( KeySignal *, Mods * ) ;
	//! \brief Callback for SpaceSignals, return true for blocking/dropping the signal.
	typedef bool ( * SpaceCallback ) ( SpaceSignal *, Mods * ) ;
}

# endif

