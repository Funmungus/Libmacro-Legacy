
# ifndef MCR_INLINE_KEYDISPATCH_H
# define MCR_INLINE_KEYDISPATCH_H

# include "hotkey/keydispatch.h"

namespace macro {

	bool KeyDispatch::dispatch ( Key & intercepted )
	{
		bool blocking = false ;
		InternalModLock.lock ( ) ;

		// Record key code, scan code, and modifiers to search through maps.
		code_mod_t keyed ( intercepted.getKey ( ), InternalMods ) ;
		code_mod_t scanned ( intercepted.getScan ( ), InternalMods ) ;

		if ( intercepted.getKeyUp ( ) != UP )
		{
			// This will use _dispatchModified for completely specific, unspecific modifiers,
			// and unspecific codes.
			_dispatchPair ( HotByKey [ 0 ], HotByScan [ 0 ], keyed, scanned,
					& intercepted, & InternalMods, blocking ) ;

			Mods::modify ( InternalMods, KeyMod::KeyToMod.second
						( intercepted.getKey ( ) ), DOWN ) ;
			keyed.second = scanned.second = InternalMods ;
		}

		if ( intercepted.getKeyUp ( ) != DOWN )
		{
			// This will use _dispatchModified for completely specific, unspecific modifiers,
			// and unspecific codes.
			_dispatchPair ( HotByKey [ 1 ], HotByScan [ 1 ], keyed, scanned,
					& intercepted, & InternalMods, blocking ) ;

			Mods::modify ( InternalMods, KeyMod::KeyToMod.second
						( intercepted.getKey ( ) ), UP ) ;
		}

		blocking = Dispatch < Key >::dispatchModified ( intercepted, InternalMods )
				? true : blocking ;

		InternalModLock.unlock ( ) ;

		return blocking ;
	}

	void KeyDispatch::_dispatchPair ( codemap_t & keyContainerMap,
											codemap_t & scanContainerMap,
											code_mod_t & keyed, code_mod_t & scanned,
											ISignal * interceptedPt,
											int * modsPt, bool & blocking )
	{
		// Edit: Unspecific code with specific modifier is only added to the key map, not
		// to scan map. This reduces the number of searches by 1.
		int key = keyed.first ; //, scan = scanned.first ;
		// specified
		DispatchToMap < code_mod_t, Key > ( keyContainerMap, keyed,
											interceptedPt, modsPt, blocking ) ;
		DispatchToMap < code_mod_t, Key > ( scanContainerMap, scanned,
											interceptedPt, modsPt, blocking ) ;

		// unspecific code
		if ( keyed.first != 0 )
		{
			keyed.first = 0 ;
			DispatchToMap < code_mod_t, Key > ( keyContainerMap, keyed,
												interceptedPt, modsPt, blocking ) ;
			keyed.first = key ;
		}

		// unspecific modifier
		// Assume modifier is same for both.
		if ( keyed.second != -1 )
		{
			keyed.second = scanned.second = -1 ;
			DispatchToMap < code_mod_t, Key > ( keyContainerMap, keyed,
												interceptedPt, modsPt, blocking ) ;
			DispatchToMap < code_mod_t, Key > ( scanContainerMap, scanned,
												interceptedPt, modsPt, blocking ) ;
		}
	}
}

# endif // MCR_INLINE_KEYDISPATCH_H
