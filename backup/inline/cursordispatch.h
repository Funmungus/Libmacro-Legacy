
# ifndef MCR_INLINE_CURSORDISPATCH_H
# define MCR_INLINE_CURSORDISPATCH_H

# include "hotkey/cursordispatch.h"

namespace macro {

	bool CursorDispatch::dispatch ( MoveCursor & intercepted )
	{
		bool blocking = false ;
		InternalModLock.lock ( ) ;

		int mapping = InternalMods ;

		if ( intercepted.isCursorJustify ( ) )
		{
			DispatchToMap < int, MoveCursor > ( HotJustified, mapping,
												& intercepted, & InternalMods,
												blocking ) ;
		}
		else
		{
			DispatchToMap < int, MoveCursor > ( HotAbsolute, mapping,
												& intercepted, & InternalMods,
												blocking ) ;
		}

		blocking = Dispatch < MoveCursor >::dispatchModified
				( intercepted, InternalMods ) ? true : blocking ;

		InternalModLock.unlock ( ) ;

		return blocking ;
	}

}

# endif // MCR_INLINE_CURSORDISPATCH_H
