/*! \file hotkey/dispatcher.h
 * \brief Dispatcher, Interpret hardware signals and dispatch them.
 */

# ifndef MCR_DISPATCHER_H
# define MCR_DISPATCHER_H

# include "hotkey/native/dispatcher.h"

namespace macro
{
/*! \brief Interprets values from \ref NativeDispatcher.
	 * Signals use \ref ISignal#dispatch ( ) before letting
	 * the system use the signal, or blocking before the system signal is continued.
	 **/
	class Dispatcher
	{
	public:
		// ! \brief Enabling or disabling stops intercepting, but does not change callbacks.
		static inline void enable ( bool enable )
		{
			enable ( & _nDispatcher, enable ) ;
		}
		// ! \brief True if intercepting events from OS.
		static inline bool isEnabled ( )
		{
			return isEnabled ( & _nDispatcher ) ;
		}
		// ! \brief Destroy threads, and restarts them.
		static inline void reset ( )
		{
			reset ( & _nDispatcher ) ;
		}
	private:
		static NativeDispatcher _nDispatcher ;
		Dispatcher ( ) ;
		~Dispatcher ( ) ;
	} ;
}

# endif
