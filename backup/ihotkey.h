/*! \file hotkey/ihotkey.h
 * \brief IHotkey, Base for hotkeys.
 */

# ifndef MCR_IHOTKEY_H
# define MCR_IHOTKEY_H

# include "hotkey/def.h"
// Mods are often used by IHotkey subclasses, and objects that use hotkeys.
//# include "hotkey/mods.h"

namespace macro
{
	/*!
	 * \brief Callback object for hotkeys which preserves the intercepted values.
	 * This is efficiently preferrable to forcing templated,
	 * inline-triggered hotkeys to preserve virtual signature of
	 * trigger function.
	 **/
	struct ITrigger
	{
		// ! \brief Return true to block given intercepted signal.
		virtual bool trigger ( ISignal * interceptedPt = NULL,
							int * modsPt = NULL ) = 0 ;
	} ;

	/*! IHotkey
	 * Base for all hotkeys.
	 */
	class IHotkey
	{
	public :
		typedef void ( * callback_t ) ( ) ;
		virtual ~IHotkey ( ) { }

		/*!
		 * \brief trigger Receive an intercepted Signal.
		 * \param intercepted The Signal that was intercepted.
		 * \param mod Set of current Mods at time of intercept.
		 * \return True to block intercepted from continuing to OS.
		 */
		virtual MCR_API bool trigger ( ISignal * interceptedPt = NULL,
									int * modsPt = NULL ) = 0 ;

		//
		// Functions to modify how registering is handled.
		//
		// ! \brief Choose whether to register as specific signal, or general.
		virtual MCR_API void specify ( const bool useSpecific ) = 0 ;
		// ! \brief \ret True if using specific signal to register, false otherwise.
		virtual MCR_API bool isSpecified ( ) const = 0 ;
//		/*!
//		 * \brief isRegistered
//		 * \return true if both able to register and enabled.
//		 */
//		virtual MCR_API bool isRegistered ( ) const = 0 ;
//		/*!
//		 * \brief canRegister
//		 * \return true if able to register signals into Dispatcher.
//		 */
//		virtual MCR_API bool canRegister ( ) const = 0 ;

		//
		// Functions to modify how signals are intercepted ( This is logically after hotkeys are registered ).
		//
		// ! \brief Callback object uses \ref ISignal#send ( )
		virtual MCR_API void setCallbackObject ( const ISignal * newCallbackPt ) = 0 ;
		// ! \brief Get the signal to send on trigger.
		virtual MCR_API const ISignal * getCallbackObject ( ) const = 0 ;
		// ! \brief Set the ITrigger to use on trigger.
		virtual MCR_API void setTriggerObject ( ITrigger * newCallbackPt ) = 0 ;
		// ! \brief Get the ITrigger used on trigger.
		virtual MCR_API const ITrigger * getTriggerObject ( ) const = 0 ;
		// ! \brief Set callback as a static function with no
		// ! parameters or out value.
		virtual MCR_API void setCallback ( callback_t newCallback ) = 0 ;
		// ! \brief Get the callback function.
		virtual MCR_API callback_t getCallback ( ) const = 0 ;
		/*!
		 * \brief hasCallback
		 * \return true if either a callback object or static function
		 * has been set.
		 */
		virtual MCR_API bool hasCallback ( ) const = 0 ;
		/*! setIntercept
		 * \brief Set the signal to be used to specify how to register this hotkey.
		 * NULL intercept will always make this hotkey register as generic.
		 */
		virtual MCR_API void setIntercept ( ISignal * interceptPt,
											const int interceptingMods ) = 0 ;
		// ! \brief \ref IHotkey#setIntercept ( const ISignal *, const Mods & )
		virtual MCR_API void setIntercept ( ISignal * interceptPt ) = 0 ;
		// ! \brief \ref IHotkey#setIntercept ( const ISignal *, const Mods & )
		virtual MCR_API void setInterceptMods ( const int interceptMods ) = 0 ;
		/*! setIntercept
		 * \brief Set the signal to be used to specify how to register this hotkey.
		 * Signal will be taken from SignalFactory, and if no signal is returned then a
		 * generic dispatch will be used.
		 *
		 * This function is meant for generic intercept
		 */
		virtual MCR_API void setIntercept ( const std::string & interceptTypeName = "" ) = 0 ;
		/*! getIntercept
		 * \brief Get the signal to be used to specify how to register this hotkey.
		 */
		virtual MCR_API const ISignal * getIntercept ( ) const = 0 ;
		/*!
		 * \brief Get the modifier to be used to specify how to register this hotkey.
		 **/
		virtual MCR_API int getInterceptMods ( ) const = 0 ;
		// ! \brief Free any pointers. This should not disable.
		virtual MCR_API void clear ( ) = 0 ;
		// ! \brief If able to intercept, enable will activate this object
		// ! \brief in the interceptors.
		virtual MCR_API void enable ( const bool enable ) = 0 ;
		// ! \brief If enabled and can intercept, this should be activated
		// ! \brief in the interceptors.
		virtual MCR_API bool isEnabled ( ) const = 0 ;
		// ! \brief Activate blocking of captured signals.
		virtual MCR_API void block ( const bool enable ) = 0 ;
		// ! \brief If blocking, this object drops captured signals from passing
		// ! \brief to other functions and objects.
		virtual MCR_API bool isBlocking ( ) const = 0 ;
	} ;
}

# endif
