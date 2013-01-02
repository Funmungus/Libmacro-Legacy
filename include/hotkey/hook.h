/*! \file hook.h
 * \brief Hooks are for intercepting events in Windows.
 */

# ifndef FUN_AHOOK_H
# define FUN_AHOOK_H

# ifdef WIN

# include "hotkey/def.h"

namespace funlibrary
{
	/*! \brief Registers a hook for specific hardware type, and sends events to given callbacks for that type.
	*
	* If the callback returns true, then the signal is dropped and has no more effect.<br>
	* If the callback returns false, then the signal is continued on, to be used by processes and
	* the operating system.
	*/
	class Hook
	{
	public:
		/*! \brief ctor
		 *
		 * Bothe hookType and hookingProc must be defined to begin enabled.
		 * \param beginEnabled If true, this hook will begin intercepting immediately.
		 * Otherwise enable ( true ) must be called explicitly.
		 */
		Hook ( const int hookType = 0, HOOKPROC hookingProc = NULL, bool beginEnabled = false ) ;
		//! \brief dtor
		~Hook ( ) ;

		//! \brief Get the type of hooking for current interception.
		int getHookType ( ) const ;
		//! \brief Set the type of hooking that takes place.
		void setHookType ( const int val ) ;
		//! \brief Get the procedure which handles hooking.
		HOOKPROC getHookedProc ( ) const ;
		//! \brief Set the hooking procedure to a new function.
		void setHookedProc ( HOOKPROC hookingProc ) ;
		//! \brief Begins intercepting.
		//! Postcondition: _hookID will be nullified if disabling.
		void enable ( bool enable ) ;
		//! \brief Return true if currently intercepting.
		bool isEnabled ( ) ;
		//! \brief same as "enable ( false ) ; enable ( true ) ;"
		void reset ( ) ;

		//! \brief Hook id for this application
		HHOOK getHookId ( ) const ;

	protected:
		/// Procedure to call on hook receipt. Protected to prevent security leak.
		//HOOKPROC getHookedProc ( ) const ;
		//( int nCode, WPARAM wParam, LPARAM lParam ) = 0 ;
		/// <summary>
		/// Given wParam and lParam, taken from a LowLevelProc, return arguments
		/// to fire an event.
		/// </summary>
		/// <returns>IHookArgs, derived from wParam and lParam</returns>
		/*virtual funlibrary::signal::ISignal sigFromHookIntercept
			( WPARAM wParam, LPARAM lParam ) = 0 ;*/
	private:
		int _hookType ;
		// First function called at hook, before event thrown
		HOOKPROC _hookedProc ;
		HHOOK _hookId ;

		/// <summary>
		/// Constructor initialization
		/// </summary>
		/// <param name="hookingProc">Procedure to call and decide what to do
		/// with hoo</param>
		/// <param name="hookType">Type of input coupled to hook. Refer to User32.lib
		/// for enumerated values</param>
		//void init ( HOOKPROC hookingProc, bool beginEnabled ) ;
		/// <summary>
		/// Hook id for this application. Private to prevent changing process
		/// coupling.
		/// </summary>
		void setHookId ( HHOOK value ) ;
		/// <summary>
		/// Procedure to call on hook receipt.
		/// </summary>
		//void setHookedProc ( HOOKPROC value ) ;

		/// <summary>
		/// Install a hook with current process, and HookedProc is the procedure or
		/// method to call
		/// </summary>
		/// <returns>Id of this hook, which is required for unhooking</returns>
		HHOOK installHook ( ) ;
	} ;
}

# endif

# endif

