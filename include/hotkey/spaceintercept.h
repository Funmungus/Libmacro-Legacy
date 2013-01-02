/*! \file spaceintercept.h
 * \brief SpaceIntercept, Intercept SpaceSignals and callback to a
 * ICall<bool, SpaceSignal*, Mods*> object.
 */

# ifndef FUN_SPACEINTERCEPT
# define FUN_SPACEINTERCEPT

# include "hotkey/def.h"
# include "hotkey/dispatcher.h"
# include "hotkey/hook.h"

namespace funlibrary
{
	/*! \brief Intercept SpaceSignals and callback to a
	 * ICall<bool, SpaceSignal*, Mods*> object.
	 *
	 * A true value returned from
	 * \ref ICall<bool, SpaceSignal*, Mods*>::call(SpaceSignal *, Mods*)
	 * will block the SpaceSignal from being used elsewhere.
	 */
	class SpaceIntercept
	{
	public:
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
		static void add ( ICall < bool, SpaceSignal *, Mods * > * newCallback ) ;
		//! \brief Remove a callback for unspecific trigger.
		static void remove ( ICall < bool, SpaceSignal *, Mods * > * delCallback ) ;
		// Specifics.
		//! \brief Enable intercepting and callback for specific triggers.
		static void enableSpecified ( const bool enable ) ;
		/*! \brief Add callback for specific trigger based on event.
		 *
		 * \param newCallback The callback to be added.
		 * \param spatial The SpaceSignal with event code to intercept.
		 * \param mod The Mods to intercept.  Will not trigger for different Mods.
		 */
		static void addForEvent ( ICall < bool, SpaceSignal *, Mods * > * newCallback,
								  SpaceSignal spatial, Mods mod ) ;
		/*! \brief Add callback for specific trigger based on cursor justification.
		 *
		 * \param newCallback The callback to be added.
		 * \param cursorJustify The cursor justification to intercept.
		 */
		static void addForJustify ( ICall < bool, SpaceSignal *, Mods * > * newCallback,
									bool cursorJustify ) ;
		/*! \brief Remove a callback for specific trigger based on event code.
		 *
		 * \param delCallback The callback to be removed.
		 * \param spatial The KeySignal with event to remove.
		 * \param mod The Mods to remove.
		 */
		static void removeForEvent ( ICall < bool, SpaceSignal *, Mods * > * delCallback,
									 SpaceSignal spatial, Mods mod ) ;
		/*! \brief Remove a callback for specific trigger based on cursor justification.
		 *
		 * \param delCallback The callback to be removed.
		 * \param cursorJustify The cursor justification to remove for.
		 */
		static void removeForJustify ( ICall < bool, SpaceSignal *, Mods * > * delCallback,
									   bool cursorJustify ) ;

		// And finally, the dispatching function.
		/*! \brief \ref Dispatcher::setSpaceCallback(const SpaceCallback newCallback)
		 * \ref SpaceHook::setSpaceCallback(const SpaceCallback newCallback)
		 */
		static bool trigger ( SpaceSignal * intercepted, Mods * mod ) ;
	protected:
	private:
		static std::mutex _enableLock ;

		static bool _enableUnspecific ;
		static std::vector < ICall < bool, SpaceSignal *, Mods * > * > _unspecifieds ;

		static bool _enableSpecific ;
		//static std::multimap < KeyUpType, int, unsigned int, std::vector < SpaceCallback > > _specifiedKeys ;
		static std::map < std::pair < int, int >, std::vector < ICall < bool, SpaceSignal *, Mods * > * > >
			_specifiedEvents ;
		static std::vector < ICall < bool, SpaceSignal *, Mods * > * > _specifiedJustify [ 2 ] ;
	} ;
}

# endif
