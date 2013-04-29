
# ifndef MCR_ECHODISPATCH_H
# define MCR_ECHODISPATCH_H

# include "hotkey/dispatch.h"

namespace macro
{
	/*!
	 * \brief Specialized and specific dispatch to \ref Hot<HIDEcho>.
	 * \ref Dispatch<HIDEcho>
	 */
	struct EchoDispatch
	{
	public :
		//
		// types
		//
		/*! \brief event code + modifier for mapping. Code of -1 means any of that code.
		 * -1 modifier means any modifier.
		 **/
		typedef std::pair < int, int > code_mod_t ;
		/*!
		 * \brief Map code_mod_t to set of hotkeys callbacks.
		 */
		typedef std::map < code_mod_t, Dispatch<HIDEcho>::hotvector_t > codemap_t ;

		//
		// members, maps and vectors of hotkeys.
		//
		/*! \brief Map event + modifier to hotkey.
		 **/
		static codemap_t HotSpecified ;

		//
		// Functions
		//
		// ! \brief Release and restart intercepting.
		static void reset ( ) ;
		// ! \brief Clear all hotkey callback objects.
		static void release ( ) ;

		/*! \brief Add callback for specific trigger based on event code. NULL intercept, or
		 * event code -1 with NULL or -1 modifier will instead be added to
		 * generic dispatch/registration.
		 */
		static void addSpecific ( Hot<HIDEcho> & newHotkey, HIDEcho * interceptPt,
			int * modsPt ) ;
		/*! \brief Remove a callback for specific trigger. This will also remove from
		 * generic dispatch/registration.
		 */
		static void removeSpecific ( Hot<HIDEcho> & delHotkey ) ;

		// And finally, the dispatching function.
		// ! \brief \ref Dispatch::dispatch ( )
		static inline bool dispatch ( HIDEcho & intercepted ) ;
	private :
		// Internal functions for simplicity. Inlined, and reference to blocking for efficiency.
	} ;
}

# include "hotkey/inline/echodispatch.h"

# endif // MCR_ECHODISPATCH_H
