
# ifdef WIN
# include "stdafx.h"
# endif

# include "hotkey/hotkey.h"

using namespace std ;

namespace funlibrary
{
	const string HotkeyStage::name = "hotkeystage" ;
	const string StagedHotkey::name = "stagedhotkey" ;

	HotkeyStage::HotkeyStage ( const ISignal * intercepting, const Mods & interceptingMods,
														 const bool activated, const bool blocking )
		: intercepting ( nullptr ), interceptingMods ( interceptingMods ),
			activated ( activated ), blocking ( blocking )
	{
		const KeySignal * key = dynamic_cast < const KeySignal * > ( intercepting ) ;
		if ( key != NULL )
		{
			this->intercepting.reset ( new KeySignal ( *key ) ) ;
		}
		else
		{
			const SpaceSignal * space = dynamic_cast < const SpaceSignal * > ( intercepting ) ;
			if ( space != NULL )
			{
				this->intercepting.reset ( new SpaceSignal ( *space ) ) ;
			}
		}
	}

	HotkeyStage::HotkeyStage ( const HotkeyStage & copytron )
		: intercepting ( nullptr ), interceptingMods ( copytron.interceptingMods ),
			activated ( copytron.activated ), blocking ( copytron.blocking )
	{
		if ( copytron.intercepting.get ( ) != nullptr )
		{
			KeySignal * key = dynamic_cast < KeySignal * > ( copytron.intercepting.get ( ) ) ;
			if ( key != NULL )
			{
				this->intercepting.reset ( new KeySignal ( *key ) ) ;
			}
			else
			{
				SpaceSignal * space = dynamic_cast < SpaceSignal * > ( copytron.intercepting.get ( ) ) ;
				if ( space != NULL )
				{
					this->intercepting.reset ( new SpaceSignal ( *space ) ) ;
				}
			}
		}
	}

	HotkeyStage & HotkeyStage::operator= ( const HotkeyStage & rhs )
	{
		if ( this != & rhs )
		{
			activated = rhs.activated ;
			blocking = rhs.blocking ;
			if ( rhs.intercepting.get ( ) != nullptr )
			{
				KeySignal * key = dynamic_cast < KeySignal * > ( rhs.intercepting.get ( ) ) ;
				if ( key != NULL )
				{
					intercepting.reset ( new KeySignal ( *key ) ) ;
				}
				else
				{
					SpaceSignal * space = dynamic_cast < SpaceSignal * > ( rhs.intercepting.get ( ) ) ;
					if ( space != NULL )
					{
						intercepting.reset ( new SpaceSignal ( *space ) ) ;
					}
				}
			}
			else
			{
				intercepting.reset ( nullptr ) ;
			}
			interceptingMods = rhs.interceptingMods ;
		}
		return *this ;
	}

	bool HotkeyStage::isMe ( KeySignal * key, Mods * mod )
	{
		KeySignal * mKey = dynamic_cast < KeySignal * > ( intercepting.get ( ) ) ;
		// I am not key signal. Not me, but also does not affect me.
		if ( mKey == NULL )
			return false ;

		// For key, current activation affects how key up is managed.
		if ( ( mKey->getKey ( ) == key->getKey ( ) || mKey->getScan ( ) == key->getScan ( ) )
			&& interceptingMods == *mod )
		{
			if ( activated )
				return true ;

			KeyUpType me = mKey->getKeyUp ( ), them = key->getKeyUp ( ) ;
			if ( me == BOTH || them == BOTH || me == them )
			{
				activated = true ;
				return true ;
			}
		}

		activated = false ;
		return false ;
	}

	bool HotkeyStage::isMe ( SpaceSignal * space, Mods * mod )
	{
		const static int moveType = SpaceSignal::getEventInt ( "move" ) ;
		// In space there is no key up, so current activation does not matter.
		SpaceSignal * mSpace = dynamic_cast < SpaceSignal * > ( intercepting.get ( ) ) ;
		// I am not space signal. Not me, but also does not affect me.
		if ( mSpace == NULL )
			return false ;

		int me = mSpace->getEvent ( ), them = space->getEvent ( ) ;
		if ( me == them && interceptingMods == *mod )
		{
			if ( me != moveType || mSpace->isCursorJustify ( ) == space->isCursorJustify ( ) )
			{
				activated = true ;
				return true ;
			}
		}
		activated = false ;
		return false ;
	}


	StagedHotkey::StagedHotkey ( ICall < void > *callback, const std::vector < HotkeyStage > & stages,
															 const bool enable, const BlockingStyle style )
		: _callObj ( NULL ), _callFunc ( NULL ),
			_stages ( ), _enable ( false ), _blockingStyle ( style )
	{
		setStages ( stages ) ;
		setCallbackObject ( callback ) ;
		this->enable ( enable ) ;
	}

	StagedHotkey::StagedHotkey ( void (*callback) ( ), const std::vector < HotkeyStage > & stages,
								 const bool enable, const BlockingStyle style )
		: _callObj ( NULL ), _callFunc ( NULL ),
			_stages ( ), _enable ( false ), _blockingStyle ( style )
	{
		setStages ( stages ) ;
		setCallback ( callback ) ;
		this->enable ( enable ) ;
	}

	StagedHotkey::StagedHotkey ( const StagedHotkey &copytron )
		: _callObj ( NULL ), _callFunc ( NULL ),
			_stages ( ), _enable ( false ), _blockingStyle ( copytron._blockingStyle )
	{
		setStages ( copytron._stages ) ;

		if ( copytron._callObj != NULL )
		{
			setCallbackObject ( copytron._callObj ) ;
		}
		else if ( copytron._callFunc != NULL )
		{
			setCallback ( copytron._callFunc ) ;
		}

		this->enable ( copytron._enable ) ;
	}

	StagedHotkey & StagedHotkey::operator = ( const StagedHotkey &copytron )
	{
		if ( &copytron != this )
		{
			setBlockStyle ( copytron._blockingStyle ) ;
			setStages ( copytron._stages ) ;

			if ( copytron._callObj != NULL )
			{
				setCallbackObject ( copytron._callObj ) ;
			}
			else if ( copytron._callFunc != NULL )
			{
				setCallback ( copytron._callFunc ) ;
			}

			this->enable ( copytron._enable ) ;
		}

		return *this ;
	}

	string StagedHotkey::type ( ) const
	{
		return name ;
	}

	void StagedHotkey::setCallbackObject ( ICall < void > *newCallback )
	{
		// If changing from no callback, to having callback, activate in KeyIntercept.
		bool toAdd = _enable && ! _stages.empty ( ) && _callObj == NULL
				&& _callFunc == NULL && newCallback != NULL ;
		// If changing from having callback, to no callback, deactivate in KeyIntercept.
		bool toDisable = ! _enable && _stages.empty ( ) &&
				( _callObj != NULL || _callFunc != NULL ) && newCallback == NULL ;

		if ( toAdd )
			add ( ) ;
		else if ( toDisable )
			remove ( ) ;

		_callObj = newCallback ;
		_callFunc = NULL ;
	}

	void StagedHotkey::setCallback ( void (* newCallback) ( ) )
	{
		// If changing from no callback, to having callback, activate in KeyIntercept.
		bool toAdd = _enable && ! _stages.empty ( ) && _callObj == NULL
				&& _callFunc == NULL && newCallback != NULL ;
		// If changing from having callback, to no callback, deactivate in KeyIntercept.
		bool toDisable = ! _enable && _stages.empty ( ) &&
				( _callObj != NULL || _callFunc != NULL ) && newCallback == NULL ;

		if ( toAdd )
			add ( ) ;
		else if ( toDisable )
			remove ( ) ;

		_callObj = NULL ;
		_callFunc = newCallback ;
	}

	void StagedHotkey::clear ( )
	{
		setCallbackObject ( NULL ) ;
		setStages ( vector < HotkeyStage > ( ) ) ;
	}

	void StagedHotkey::enable ( const bool enable )
	{
		// If enabling, activate in KeyIntercept
		bool toAdd = _callObj != NULL && ! _stages.empty ( ) && ! _enable && enable ;
		// If disabling, deactivate in KeyIntercept
		bool toDisable = _callObj == NULL && _stages.empty ( ) && _enable && ! enable ;

		if ( toAdd )
			add ( ) ;
		else if ( toDisable )
			remove ( ) ;

		_enable = enable ;
	}

	bool StagedHotkey::isEnabled ( ) const
	{
		return _enable && _callObj != NULL ;
	}

	void StagedHotkey::setBlocking ( const bool blocking )
	{
		vector < HotkeyStage >::iterator it ;
		if ( blocking )
		{
			switch ( _blockingStyle )
			{
			case UNMANAGED : break ;
			case NO_BLOCK :
				for ( it = _stages.begin ( ) ; it != _stages.end ( ) ; it ++ )
				{
					it->blocking = false ;
				}
				break ;
			case BLOCK_FINAL :
				for ( it = _stages.begin ( ) ; it != _stages.end ( ) - 1 ; it ++ )
				{
					it->blocking = false ;
				}
				it->blocking = true ;
				break ;
			case BLOCK_ALL :
				for ( it = _stages.begin ( ) ; it != _stages.end ( ) ; it ++ )
				{
					it->blocking = true ;
				}
				break ;
			}
		}
		else
		{
			if ( ! _blockingStyle == UNMANAGED )
			{
				for ( it = _stages.begin ( ) ; it != _stages.end ( ) ; it ++ )
				{
					it->blocking = false ;
				}
			}
		}
	}

	bool StagedHotkey::isBlocking ( ) const
	{
		return _blockingStyle >= BLOCK_FINAL ;
	}

	bool StagedHotkey::call ( KeySignal * intercepted, Mods * mod )
	{
		bool hasBlocking = false ;
		// point to last
		auto it = _stages.rbegin ( ) ;
		// If no stages to check, do not block.
		if ( it == _stages.rend ( ) ) 
			return false ; 

		// If this is last item, it is the only one to check.
		// If not last, then next must be activated
		// Logically if it->activated also, but triggering clears all, and it->activated also means trigger.
		if ( it + 1 == _stages.rend ( ) || ( it +1 )->activated )
		{
			if ( it->isMe ( intercepted, mod ) )
			{
				trigger ( ) ;
				return it->blocking ;
			}
		}

		++ it ;
		if ( it == _stages.rend ( ) )
			return false ;

		// it + 1 is for checking final. it + 2 is for checking it + 1
		for ( ; it + 1 != _stages.rend ( ) ; it ++ )
		{
			if ( it->activated || ( it + 1 )->activated )
			{
				if ( ( it )->isMe ( intercepted, mod ) && ! hasBlocking
					 && ( it )->blocking )
						hasBlocking = true ;
			}
		}
		// Checked all but first item.  Always check first item.
		// at end, go back to one before
		if ( it->isMe ( intercepted, mod ) && ! hasBlocking
			 && it->blocking )
			hasBlocking = true ;
		return hasBlocking ;
	}

	bool StagedHotkey::call ( SpaceSignal * intercepted, Mods * mod )
	{
		bool hasBlocking = false ;
		// point to last
		auto it = _stages.rbegin ( ) ;
		// If no stages to check, do not block.
		if ( it == _stages.rend ( ) ) 
			return false ; 

		// If this is last item, it is the only one to check.
		// If not last, then next must be activated
		// Logically if it->activated also, but triggering clears all, and it->activated also means trigger.
		if ( it + 1 == _stages.rend ( ) || ( it +1 )->activated )
		{
			if ( it->isMe ( intercepted, mod ) )
			{
				trigger ( ) ;
				return it->blocking ;
			}
		}

		++ it ;
		if ( it == _stages.rend ( ) )
			return false ;

		// it + 1 is for checking final. it + 2 is for checking it + 1
		for ( ; it + 1 != _stages.rend ( ) ; it ++ )
		{
			if ( it->activated || ( it + 1 )->activated )
			{
				if ( ( it )->isMe ( intercepted, mod ) && ! hasBlocking
					 && ( it )->blocking )
						hasBlocking = true ;
			}
		}
		// Checked all but first item.  Always check first item.
		// at end, go back to one before
		if ( it->isMe ( intercepted, mod ) && ! hasBlocking
			 && it->blocking )
			hasBlocking = true ;
		return hasBlocking ;
	}

	// Staged Hotkey specific.
	BlockingStyle StagedHotkey::getBlockStyle ( ) const
	{
		return _blockingStyle ;
	}

	void StagedHotkey::setBlockStyle ( const BlockingStyle style )
	{
		_blockingStyle = style ;
	}

	std::vector < HotkeyStage > StagedHotkey::getStages ( ) const
	{
		return _stages ;
	}

	void StagedHotkey::setStages ( const std::vector < HotkeyStage > & stageIn )
	{
		// If changing from no callback, to having callback, activate in KeyIntercept.
		bool toAdd = _enable && _callObj != NULL && _stages.empty ( ) && ! stageIn.empty ( ) ;
		// If changing from having callback, to no callback, deactivate in KeyIntercept.
		bool toDisable = ! _enable && _callObj == NULL && ! _stages.empty ( ) && stageIn.empty ( ) ;

		if ( toAdd )
			add ( ) ;
		else if ( toDisable )
			remove ( ) ;

		_stages = stageIn ;
	}

	size_t StagedHotkey::stageLength ( ) const
	{
		return _stages.size ( ) ;
	}

	HotkeyStage StagedHotkey::getStage ( const size_t index ) const
	{
		if ( index < _stages.size ( ) )
			return _stages [ index ] ;
		else
			throw ( out_of_range ( "Not that many stages in Hotkey." ) ) ;
		return HotkeyStage ( ) ;
	}

	void StagedHotkey::setStage ( const HotkeyStage & stage, const size_t index )
	{
		if ( index == _stages.size ( ) )
		{
			_stages.push_back ( stage ) ;
		}
		else if ( index < _stages.size ( ) )
		{
			_stages [ index ] = stage ;
		}
	}

	void StagedHotkey::addStage ( const HotkeyStage & stage )
	{
		// If changing from no callback, to having callback, activate in KeyIntercept.
		bool toAdd = _enable && _callObj != NULL && _stages.empty ( ) ;

		if ( toAdd )
			add ( ) ;

		_stages.push_back ( stage ) ;
	}

	void StagedHotkey::removeStage ( const size_t index )
	{
		bool hadItem = ! _stages.empty ( ) ;
		if ( index < _stages.size ( ) )
		{
			_stages.erase ( _stages.begin ( ) + index ) ;
		}
		// If changing from having callback, to no callback, deactivate in KeyIntercept.
		bool toDisable = ! _enable && _callObj == NULL && hadItem && _stages.empty ( ) ;

		if ( toDisable )
			remove ( ) ;
	}

	void StagedHotkey::add ( )
	{
		if ( hasKey ( ) )
		{
			if ( ! KeyIntercept::isEnabled ( ) )
				KeyIntercept::enable ( true ) ;
			KeyIntercept::add ( this ) ;
		}

		if ( hasSpace ( ) )
		{
			if ( ! SpaceIntercept::isEnabled ( ) )
				SpaceIntercept::enable ( true ) ;
			SpaceIntercept::add ( this ) ;
		}
	}

	void StagedHotkey::remove ( )
	{
		if ( hasKey ( ) )
		{
			KeyIntercept::remove ( this ) ;
		}

		if ( hasSpace ( ) )
		{
			SpaceIntercept::remove ( this ) ;
		}
	}

	bool StagedHotkey::hasKey ( ) const
	{
		for ( auto it = _stages.begin ( ) ; it != _stages.end ( ) ; it ++ )
		{
			KeySignal * key = dynamic_cast < KeySignal * > ( it->intercepting.get ( ) ) ;
			if ( key != NULL )
				return true ;
		}
		return false ;
	}

	bool StagedHotkey::hasSpace ( ) const
	{
		for ( auto it = _stages.begin ( ) ; it != _stages.end ( ) ; it ++ )
		{
			SpaceSignal * space = dynamic_cast < SpaceSignal * > ( it->intercepting.get ( ) ) ;
			if ( space != NULL )
				return true ;
		}
		return false ;
	}

	void StagedHotkey::trigger ( )
	{
		if ( _callObj != NULL )
		{
			_callObj->call ( ) ;
		}
		else if ( _callFunc != NULL )
		{
			_callFunc ( ) ;
		}
		for ( auto it = _stages.begin ( ) ; it != _stages.end ( ) ; it ++ )
		{
			it->activated = false ;
		}
	}
}
