
# ifndef MCR_INLINE_SIMPLEHOTKEY_H
# define MCR_INLINE_SIMPLEHOTKEY_H

# include "hotkey/simplehotkey.h"

namespace macro
{
	bool Hotkey::trigger ( ISignal * interceptedPt, int * modsPt )
	{
//		_ensureMember ( ) ; 	// TODO decide if ensurance required here.
		return _hot->trigger ( interceptedPt, modsPt ) ;
	}
	bool Hotkey::send ( )
	{
		// dispatch may not ensure member
		if ( dispatch ( ) ) return true ;
		// trigger will ensure member
		trigger ( ) ;
		return true ;
	}

	void Hotkey::_ensureMember ( )
	{
		if ( _hot.get ( ) == NULL )
		{
			_hot.reset ( new Hot<ISignal> ( ) ) ;
		}
	}
}

# endif
