
# ifndef MCR_INLINE_STAGEDHOTKEY_H
# define MCR_INLINE_STAGEDHOTKEY_H

# include "hotkey/stagedhotkey.h"

namespace macro
{
	bool StagedHotkey::trigger ( ISignal * interceptedPt = NULL, int * modsPt = NULL )
	{
		bool blocking = false ;
		_lock.lock ( ) ;
		// Even if we have no callback, we want to activate stages, and possibly block.
		bool activated = _stageContainer.activateMe ( interceptedPt,
												modsPt == NULL ? 0 : * modsPt,
												blocking ) ;
		if ( ! hasCallback ( ) )
		{
			_lock.unlock ( ) ;
			return blocking ;
		}
		if ( activated )
		{
			// Sending starts new thread.
			std::thread ( & StagedHotkey<ISignal>::_sendThread, this ).detach ( ) ;
			// Still not completely sure the thread will take lock first, but there is a good chance it will.
			_lock.unlock ( ) ;
			// We have stored blocking already. Return that value.
			return blocking ;
		}
		_lock.unlock ( ) ;
		return false ;
	}

	bool StagedHotkey::send ( )
	{
		_lock.lock ( ) ;
		// We have one of the two callbacks, so we may be blocking.
		// Get currently block style while we still have lock.
		std::thread ( & StagedHotkey<ISignal>::_sendThread, this ).detach ( ) ;
		// Still not completely sure the thread will take lock first, but there is a good chance it will.
		_lock.unlock ( ) ;
		return true ;
	}

	bool StagedHotkey::operator ( ) ( )
	{
		return send ( ) ;
	}

	bool StagedHotkey::dispatch ( )
	{
		if ( Dispatcher != NULL )
		{
			if ( Dispatcher ( * this ) )
				return true ;
		}
		return ISignal::dispatch ( ) ;
	}

	void StagedHotkey::_register ( )
	{
		if ( _unreg == NULL ) return ;
		_unreg ( this ) ;
	}

	void StagedHotkey::_unregister ( )
	{
		if ( ! _enable ) return ;
		if ( _reg != NULL )
		{
			_reg ( this ) ;
		}
	}
}

# endif // MCR_INLINE_STAGEDHOTKEY_H
