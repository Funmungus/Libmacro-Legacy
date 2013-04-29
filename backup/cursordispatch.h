
# ifndef MCR_CURSORDISPATCH_H
# define MCR_CURSORDISPATCH_H

# include "hotkey/dispatch.h"

namespace macro {

	/*!
	 * \brief Specialized and specific dispatch to \ref Hot<MoveCursor>.
	 * \ref Dispatch<MoveCursor>
	 *
	 * Dimensions are mapped by dimension type. Adding and dispatch determined by positive or
	 * negative value. -1 negative value, 0 any, 1 positive value.
	 */
	struct CursorDispatch
	{
	public :
		//
		// types
		//
		/*!
		 * \brief Map modifier to set of hotkeys callbacks.
		 */
		typedef std::map < int, Dispatch<MoveCursor>::hotvector_t > map_t ;

		//
		// members, maps and vectors of hotkeys.
		//
		/*!
		 * \brief Map cursor justified modifier to hotkey callback.
		 * -1 modifier is any modifier.
		 */
		static map_t HotJustified ;
		/*!
		 * \brief Map absolute movement modifier to hotkey callback.
		 * -1 modifier is any modifier.
		 */
		static map_t HotAbsolute ;

		//
		// Functions
		//
		// ! \brief Release and restart intercepting.
		static void reset ( ) ;
		// ! \brief Clear all hotkey callback objects.
		static void release ( ) ;

		/*! \brief Add callback for specific trigger based on cursor justification value and
		 * modifier. If interceptPt is NULL, and modsPt is also NULL or -1, then
		 * generic dispatch/registration will be used instead.
		 */
		static void addSpecific ( Hot<MoveCursor> & newHotkey, MoveCursor * interceptPt,
			int * modsPt ) ;
		/*! \brief Remove a callback for specific trigger. This will also remove from
		 * generic dispatch/registration.
		 */
		static void removeSpecific ( Hot<MoveCursor> & delHotkey ) ;

		// And finally, the dispatching function.
		// ! \brief \ref Dispatch::dispatch ( )
		static inline bool dispatch ( MoveCursor & intercepted ) ;
	private :
		// Internal functions for simplicity. Inlined, and reference to blocking for efficiency.
	} ;
}

# include "hotkey/inline/cursordispatch.h"

# endif // MCR_CURSORDISPATCH_H
