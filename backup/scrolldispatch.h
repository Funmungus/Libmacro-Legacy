
# ifndef MCR_SCROLLDISPATCH_H
# define MCR_SCROLLDISPATCH_H

# include "hotkey/dispatch.h"

namespace macro {

	/*!
	 * \brief Specialized and specific dispatch to \ref Hot<Scroll>.
	 * \ref Dispatch<Scroll>
	 */
	struct ScrollDispatch
	{
	public :
		//
		// types
		//
		/*!
		 * \brief Map modifier to set of hotScrolls callbacks.
		 */
		typedef std::map < int, Dispatch<Scroll>::hotvector_t > map_t ;

		//
		// members, maps and vectors of hotkeys.
		//
		/*! \brief Map modifier to hotkey.
		 **/
		static map_t HotSpecified ;

		//
		// Functions
		//
		// ! \brief Release and restart intercepting.
		static void reset ( ) ;
		// ! \brief Clear all hotScroll callback objects.
		static void release ( ) ;

		/*! \brief Add callback for specific trigger based on modifier.
		 * NULL or -1 modifier will use
		 * generic dispatch/registration.
		 */
		static void addSpecific ( Hot<Scroll> & newHotkey, Scroll * interceptPt,
			int * modsPt ) ;
		/*! \brief Remove a callback for specific trigger. This will also remove from
		 * generic dispatch/registration.
		 */
		static void removeSpecific ( Hot<Scroll> & delHotkey ) ;

		// And finally, the dispatching function.
		// ! \brief \ref Dispatch::dispatch ( )
		static inline bool dispatch ( Scroll & intercepted ) ;
	private :
		// Internal functions for simplicity. Inlined, and reference to blocking for efficiency.
	} ;
}

# include "hotkey/inline/scrolldispatch.h"

# endif // MCR_SCROLLDISPATCH_H
