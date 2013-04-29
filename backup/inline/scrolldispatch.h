
# ifndef MCR_INLINE_SCROLLDISPATCH_H
# define MCR_INLINE_SCROLLDISPATCH_H

# include "hotkey/scrolldispatch.h"

namespace macro {

	bool ScrollDispatch::dispatch ( Scroll & intercepted )
	{
		bool blocking = false ;
		InternalModLock.lock ( ) ;

		DispatchToMap < int, Scroll > ( HotSpecified, InternalMods,
											& intercepted, & InternalMods,
											blocking ) ;

		blocking = Dispatch < Scroll >::dispatchModified
				( intercepted, InternalMods ) ? true : blocking ;

		InternalModLock.unlock ( ) ;

		return blocking ;
	}

}

# endif // MCR_INLINE_SCROLLDISPATCH_H
