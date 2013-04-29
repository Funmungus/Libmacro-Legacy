
# ifndef MCR_KEYDISPATCH_H
# define MCR_KEYDISPATCH_H

# include "hotkey/dispatch.h"
# include "hotkey/keymod.h"

namespace macro {

	/*!
	 * \brief Specialized and specific dispatch to \ref Hot<Key>.
	 * \ref Dispatch<Key>
	 */
	struct KeyDispatch
	{
	public :
		//
		// types
		//
		/*! \brief key/scan code + modifier for mapping. Code of 0 means any of that code.
		 * The KeyUpType is translated into an array address. -1 modifier means
		 * any modifier.
		 **/
		typedef std::pair < int, int > code_mod_t ;
		/*!
		 * \brief Map code_mod_t to set of hotkeys callbacks.
		 */
		typedef std::map < code_mod_t, Dispatch<Key>::hotvector_t > codemap_t ;

		//
		// members, maps and vectors of hotkeys.
		//
		/*! \brief Map key + modifier to hotkey. Key up type BOTH is added as both DOWN
		 * and UP. Dispatching both is treated as DOWN, then UP.
		 **/
		static codemap_t HotByKey [ 2 ] ;
		/*! \brief Map scan + modifier to hotkey. Key up type BOTH is added as both DOWN
		 * and UP. Dispatching both is treated as DOWN, then UP.
		 **/
		static codemap_t HotByScan [ 2 ] ;

		//
		// Functions
		//
		// ! \brief Release and restart intercepting.
		static void reset ( ) ;
		// ! \brief Clear all hotkey callback objects.
		static void release ( ) ;

		/*! \brief Add callback for specific trigger based on key code or scan code.
		 * If both key and scan are not 0, only key will be used ( this is a value that can
		 * be named. ) If both are 0, or interceptPt is NULL, and modsPt is also NULL, then
		 * generic dispatch/registration will be used instead.
		 */
		static void addSpecific ( Hot<Key> & newHotkey, Key * interceptPt,
			int * modsPt ) ;
		/*! \brief Remove a callback for specific trigger. This will also remove from
		 * generic dispatch/registration.
		 */
		static void removeSpecific ( Hot<Key> & delHotkey ) ;

		// And finally, the dispatching function.
		// ! \brief \ref Dispatch::dispatch ( )
		static inline bool dispatch ( Key & intercepted ) ;
	private :
		// Internal functions for simplicity. Inlined, and reference to blocking for efficiency.
		static inline void _dispatchPair ( codemap_t & keyContainerMap,
										codemap_t & scanContainerMap,
										code_mod_t & keyed, code_mod_t & scanned,
										ISignal * interceptedPt,
										int * modsPt, bool & blocking ) ;
	} ;
}

# include "hotkey/inline/keydispatch.h"

# endif // MCR_KEYDISPATCH_H
