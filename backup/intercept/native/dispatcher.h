/*! \file hotkey/native/dispatcher.h
 * \brief Important, please implement these functions for given
 * programming environment to dispatch native hardware to signal dispatchers.
 * <br><br>
 * \ref \file hotkey/nativeless/dispatcher.h <br>
 * \ref \file hotkey/lnx/dispatcher.h <br>
 * \ref \file hotkey/win/dispatcher.h
 * */

# ifndef MCR_NATIVE_DISPATCHER_H
# define MCR_NATIVE_DISPATCHER_H

# include "hotkey/def.h"

namespace macro
{
	// ! ctor has no arguments
	struct NativeDispatcher ;
	// ! \brief Enable the native dispatcher.
	void enable ( NativeDispatcher &, bool enable ) ;
		// ! \brief True if intercepting events natively.
	bool isEnabled ( NativeDispatcher & ) ;
		// ! \brief Destroy resources and restarts intercepting if enabled.
	void reset ( NativeDispatcher & ) ;
}

# ifdef LNX
# include "hotkey/lnx/dispatcher.h"
# elif WIN
# include "hotkey/win/dispatcher.h"
# else
# pragma WARNING(Native undefined: Dispatcher will not be usable.)
# include "hotkey/nativeless/dispatcher.h"
# endif

# endif
