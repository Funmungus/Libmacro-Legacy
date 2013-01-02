/*! \file keyintercept.h
 * \brief KeyIntercept, Intercept KeySignals and callback to a
 * ICall<bool, KeySignal*, Mods*> object.
 */

# ifndef FUN_KEYINTERCEPT_H
# define FUN_KEYINTERCEPT_H

# include "hotkey/def.h"
# include "hotkey/dispatcher.h"
# include "hotkey/hook.h"

namespace funlibrary
{
	/*! \brief Intercept KeySignals and callback to a
	 * ICall<bool, KeySignal*, Mods*> object.
	 *
	 * A true value returned from
	 * \ref ICall<bool, KeySignal*, Mods*>::call(KeySignal *, Mods*)
	 * will block the KeySignal from being used elsewhere.
	 */
	class KeyIntercept
	{
	public:
		//! \brief Return the real-time modifiers of known subsystems.
		static unsigned int getMods ( ) ;

		// Intercepting
		//! \brief Begin intercepting.  Requires a callback object to be added.
		static void enable ( const bool enable ) ;
		//! \brief True if enabled.
		static bool isEnabled ( ) ;
		//! \brief Release and restart intercepting.
		static void reset ( ) ;
		//! \brief Clear all callback objects.
		static void release ( ) ;

		// Trigger and dispatch.
		// Unspecifics.
		//! \brief Enable intercepting and callback for unspecific triggers.
		static void enableUnspecified ( const bool enable ) ;
		//! \brief Add callback for unspecific trigger.
		static void add ( ICall < bool, KeySignal *, Mods * > * newCallback ) ;
		//! \brief Remove a callback for unspecific trigger.
		static void remove ( ICall < bool, KeySignal *, Mods * > * delCallback ) ;
		// Specifics.
		//! \brief Enable intercepting and callback for specific triggers.
		static void enableSpecified ( const bool enable ) ;
		/*! \brief Add callback for specific trigger based on key code.
		 *
		 * \param newCallback The callback to be added.
		 * \param key The KeySignal with key code to intercept.
		 * \param mod The Mods to intercept.  Will not trigger for different Mods.
		 */
		static void addForKeycode ( ICall < bool, KeySignal *, Mods * > * newCallback,
									KeySignal key, Mods mod ) ;
		/*! \brief Add callback for specific trigger based on scan code.
		 *
		 * \param newCallback The callback to be added.
		 * \param scan The KeySignal with scan code to intercept.
		 * \param mod The Mods to intercept.  Will not trigger for different Mods.
		 */
		static void addForScancode ( ICall < bool, KeySignal *, Mods * > * newCallback,
									 KeySignal scan, Mods mod ) ;
		/*! \brief Remove a callback for specific trigger based on key code.
		 *
		 * \param delCallback The callback to be removed.
		 * \param key The KeySignal with key code to remove.
		 * \param mod The Mods to remove.
		 */
		static void removeForKeycode ( ICall < bool, KeySignal *, Mods * > * delCallback,
									   KeySignal key, Mods mod ) ;
		/*! \brief Remove a callback for specific trigger based on scan code.
		 *
		 * \param delCallback The callback to be removed.
		 * \param scan The KeySignal with scan code to remove.
		 * \param mod The Mods to remove.
		 */
		static void removeForScancode ( ICall < bool, KeySignal *, Mods * > * delCallback,
										KeySignal scan, Mods mod ) ;

		// And finally, the dispatching function.
		/*! \brief \ref Dispatcher::setKeyCallback(const KeyCallback newCallback)
		 * \ref KeyHook::setKeyCallback(const KeyCallback newCallback)
		 */
		static bool trigger ( KeySignal * intercepted, Mods * mod ) ;
	protected:
	private:
		static std::mutex _enableLock ;

		static bool _enableUnspecific ;
		static std::vector < ICall < bool, KeySignal *, Mods * > * > _unspecifieds ;

		static bool _enableSpecific ;
		//static std::multimap < KeyUpType, int, unsigned int, std::vector < KeyCallback > > _specifiedKeys ;
		static std::map < std::pair < int, int >, std::vector < ICall < bool, KeySignal *, Mods * > * > >
			_specifiedKeys [ 2 ] ;
		static std::map < std::pair < int, int >, std::vector < ICall < bool, KeySignal *, Mods * > * > >
			_specifiedScans [ 2 ] ;
	} ;
}

# endif

