
# ifndef MCR_INLINE_ECHODISPATCH_H
# define MCR_INLINE_ECHODISPATCH_H

# include "hotkey/echodispatch.h"

namespace macro {

	bool EchoDispatch::dispatch ( HIDEcho & intercepted )
	{
		bool blocking = false ;
		InternalModLock.lock ( ) ;

		int evMem = intercepted.getEvent ( ) ;
		code_mod_t mapping ( evMem, InternalMods ) ;

		// specific
		DispatchToMap < code_mod_t, HIDEcho > ( HotSpecified, mapping,
												& intercepted, & InternalMods,
												blocking ) ;

		// event unspecific
		if ( mapping.first != -1 )
		{
			mapping.first = -1 ;
			DispatchToMap < code_mod_t, HIDEcho > ( HotSpecified, mapping,
													& intercepted, & InternalMods,
													blocking ) ;
			mapping.first = evMem ;
		}

		// modifier unspecific
		if ( mapping.second != -1 )
		{
			mapping.second = -1 ;
			DispatchToMap < code_mod_t, HIDEcho > ( HotSpecified, mapping,
													& intercepted, & InternalMods,
													blocking ) ;
		}

		// always generic
		blocking = Dispatch < HIDEcho >::dispatchModified
				( intercepted, InternalMods ) ? true : blocking ;

		Mods::mod_t m = EchoToMod.second ( intercepted.getEvent ( ) ) ;
		Mods::modify ( InternalMods, m.first, m.second ) ;
		InternalModLock.unlock ( ) ;

		return blocking ;
	}

}

# endif // MCR_INLINE_ECHODISPATCH_H
