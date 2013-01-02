
# ifdef WIN
# include "stdafx.h"
# endif

# include "hotkey/hotkey.h"

using namespace std ;

namespace funlibrary
{
	const string IHotkey::name = "hotkey" ;
	const string Hotkey::name = "simplehotkey" ;

	Hotkey::Hotkey ( ICall < void > *callback, const ISignal * intercept,
									 const Mods & interceptMods, const bool enable, const bool blocking )
		: _callObj ( NULL ), _callFunc ( NULL ), _intercept ( nullptr ), _interceptMod ( ),
			_enable ( false ), _blocking ( blocking ),  _interceptType ( NONE )
	{
		setIntercept ( intercept, interceptMods ) ;
		setCallbackObject ( callback ) ;
		this->enable ( enable ) ;
	}

	Hotkey::Hotkey ( void (*callback) ( ), const ISignal * intercept,
									 const Mods & interceptMods, const bool enable, const bool blocking )
		: _callObj ( NULL ), _callFunc ( NULL ), _intercept ( nullptr ), _interceptMod ( ),
			_enable ( false ), _blocking ( blocking ), _interceptType ( NONE )
	{
		setIntercept ( intercept, interceptMods ) ;
		setCallback ( callback ) ;
		this->enable ( enable ) ;
	}

	Hotkey::Hotkey ( const Hotkey & copytron )
		: _callObj ( NULL ), _callFunc ( NULL ), _intercept ( nullptr ),	_interceptMod ( copytron._interceptMod),
			_enable ( false ), _blocking ( copytron._blocking ),
			_interceptType ( copytron._interceptType )
	{
		setIntercept ( copytron._intercept.get ( ) ) ;

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

	Hotkey & Hotkey::operator = ( const Hotkey & copytron )
	{
		if ( &copytron != this )
		{
			setBlocking ( copytron._blocking ) ;

			setIntercept ( copytron._intercept.get ( ), copytron._interceptMod ) ;

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

	string Hotkey::type ( ) const
	{
		return name ;
	}

	void Hotkey::setCallbackObject ( ICall < void > *newCallback )
	{
		// If changing from no callback, to having callback, activate in KeyIntercept.
		bool toAdd = _enable && _callObj == NULL && _callFunc == NULL
				&& newCallback != NULL ;
		// If changing from having callback, to no callback, deactivate in KeyIntercept.
		bool toDisable = ! _enable && ( _callObj != NULL || _callObj != NULL )
				&& newCallback == NULL ;

		if ( toAdd )
			add ( ) ;
		else if ( toDisable )
			remove ( ) ;

		_callObj = newCallback ;
		_callFunc = NULL ;
	}

	void Hotkey::setCallback ( void (*newCallback) ( ) )
	{
		// If changing from no callback, to having callback, activate in KeyIntercept.
		bool toAdd = _enable && _callObj == NULL && _callFunc == NULL
				&& newCallback != NULL ;
		// If changing from having callback, to no callback, deactivate in KeyIntercept.
		bool toDisable = ! _enable && ( _callObj == NULL || _callFunc != NULL )
				&& newCallback == NULL ;

		if ( toAdd )
			add ( ) ;
		else if ( toDisable )
			remove ( ) ;

		_callObj = NULL ;
		_callFunc = newCallback ;
	}

	void Hotkey::clear ( )
	{
		setCallbackObject ( NULL ) ;
	}

	void Hotkey::enable ( const bool enable )
	{
		// If enabling, activate in KeyIntercept
		bool toAdd = _callObj != NULL && ! _enable && enable ;
		// If disabling, deactivate in KeyIntercept
		bool toDisable = _callObj == NULL && _enable && ! enable ;

		if ( toAdd )
			add ( ) ;
		else if ( toDisable )
			remove ( ) ;

		_enable = enable ;
	}

	bool Hotkey::isEnabled ( ) const
	{
		return _enable && _callObj != NULL ;
	}

	void Hotkey::setBlocking ( const bool blocking )
	{
		_blocking = blocking ;
	}

	bool Hotkey::isBlocking ( ) const
	{
		return _blocking ;
	}

	// Simple Hotkey specific.
	void Hotkey::setIntercept ( const ISignal *intercept, const Mods & interceptMods )
	{
		if ( intercept == NULL ) return ;

		if ( intercept->type ( ) == KeySignal::name )
		{
			const KeySignal * key = dynamic_cast < const KeySignal * > ( intercept ) ;
			if ( key != NULL )
			{
				setIntercept ( *key, interceptMods ) ;
			}
		}
		else if ( intercept->type ( ) == SpaceSignal::name )
		{
			const SpaceSignal * space = dynamic_cast < const SpaceSignal * > ( intercept ) ;
			if ( space != NULL )
			{
				setIntercept ( *space, interceptMods ) ;
			}
		}
	}

	void Hotkey::setIntercept ( const ISignal *intercept )
	{
		if ( intercept == NULL ) return ;

		if ( intercept->type ( ) == KeySignal::name )
		{
			const KeySignal * key = dynamic_cast < const KeySignal * > ( intercept ) ;
			if ( key != NULL )
			{
				setIntercept ( *key ) ;
			}
		}
		else if ( intercept->type ( ) == SpaceSignal::name )
		{
			const SpaceSignal * space = dynamic_cast < const SpaceSignal * > ( intercept ) ;
			if ( space != NULL )
			{
				setIntercept ( *space ) ;
			}
		}
	}

	void Hotkey::setInterceptMod ( const Mods & interceptMods )
	{
		setIntercept ( _intercept.get ( ), interceptMods ) ;
	}

	void Hotkey::setIntercept ( const KeySignal & intercept, const Mods & interceptMods )
	{
		if ( isEnabled ( ) )
			remove ( ) ;

		_interceptType = KEY ;
		_intercept.reset ( new KeySignal ( intercept ) ) ;
		_interceptMod = interceptMods ;

		if ( isEnabled ( ) )
			add ( ) ;
	}

	void Hotkey::setIntercept ( const SpaceSignal & intercept, const Mods & interceptMods )
	{
		if ( isEnabled ( ) )
			remove ( ) ;

		_interceptType = SPACE ;
		_intercept.reset ( new SpaceSignal ( intercept ) ) ;
		_interceptMod = interceptMods ;

		if ( isEnabled ( ) )
			add ( ) ;
	}

	void Hotkey::setIntercept ( const KeySignal & intercept )
	{
		setIntercept ( intercept, _interceptMod );
	}

	void Hotkey::setIntercept ( const SpaceSignal & intercept )
	{
		setIntercept ( intercept, _interceptMod );
	}

	void Hotkey::getIntercept ( unique_ptr < ISignal > * interceptedSignal,
															Mods * interceptedMods ) const
	{
		if ( interceptedSignal != NULL )
			interceptedSignal->reset
					( SignalFactory::copy ( _intercept.get ( ) ) ) ;
		if ( interceptedMods != NULL )
			*interceptedMods = _interceptMod ;
	}

	InterceptType Hotkey::getInterceptType ( ) const
	{
		return _interceptType ;
	}

	bool Hotkey::call ( KeySignal * intercepted, Mods * mod )
	{
		if ( intercepted == NULL && mod == NULL )
		{
			cout << "Something is fishy... Hotkey is triggering from a NULL KeySignal." << endl ;
		}
		if ( _callObj != NULL )
		{
			_callObj->call ( ) ;
		}
		return _blocking ;
	}

	bool Hotkey::call ( SpaceSignal * intercepted, Mods * mod )
	{
		if ( intercepted == NULL && mod == NULL )
		{
			cout << "Something is fishy... Hotkey is triggering from a NULL SpaceSignal." << endl ;
		}
		if ( _callObj != NULL )
		{
			_callObj->call ( ) ;
		}
		return _blocking ;
	}

	void Hotkey::add ( )
	{
		switch ( _interceptType )
		{
		case NONE : break ;
		case KEY :
		{
			if ( ! KeyIntercept::isEnabled ( ) )
				KeyIntercept::enable ( true ) ;
			KeySignal * mKey = dynamic_cast < KeySignal * > ( _intercept.get ( ) ) ;
			if ( mKey != NULL )
			{
				if ( mKey->getKey ( ) != 0 )
				{
					KeyIntercept::addForKeycode ( this, *mKey, _interceptMod ) ;
				}
				if ( mKey->getScan ( ) != 0 )
				{
					KeyIntercept::addForScancode ( this, *mKey, _interceptMod ) ;
				}

				if ( mKey->getKey ( ) == 0 && mKey->getScan ( ) == 0 )
				{
					KeyIntercept::add ( this ) ;
				}
			}
			break ;
		}
		case SPACE :
		{
			if ( ! SpaceIntercept::isEnabled ( ) )
				SpaceIntercept::enable ( true ) ;
			SpaceSignal * mSpace = dynamic_cast < SpaceSignal * > ( _intercept.get ( ) ) ;
			const static int moveType = SpaceSignal::getEventInt ( "move" ) ;
			if ( mSpace != NULL )
			{
				if ( mSpace->getEvent ( ) == moveType )
					SpaceIntercept::addForJustify ( this, mSpace->isCursorJustify ( ) ) ;
				else
					SpaceIntercept::addForEvent ( this, *mSpace, _interceptMod ) ;
			}
		}
		}
	}

	void Hotkey::remove ( )
	{
		switch ( _interceptType )
		{
		case NONE : break ;
		case KEY :
		{
			KeySignal * mKey = dynamic_cast < KeySignal * > ( _intercept.get ( ) ) ;
			if ( mKey != NULL )
			{
				if ( mKey->getKey ( ) != 0 )
				{
					KeyIntercept::removeForKeycode ( this, *mKey, _interceptMod ) ;
				}
				if ( mKey->getScan ( ) != 0 )
				{
					KeyIntercept::removeForScancode ( this, *mKey, _interceptMod ) ;
				}

				if ( mKey->getKey ( ) == 0 && mKey->getScan ( ) == 0 )
				{
					KeyIntercept::remove ( this ) ;
				}
			}
			break ;
		}
		case SPACE :
		{
			SpaceSignal * mSpace = dynamic_cast < SpaceSignal * > ( _intercept.get ( ) ) ;
			const static int moveType = SpaceSignal::getEventInt ( "move" ) ;
			if ( mSpace != NULL )
			{
				if ( mSpace->getEvent ( ) == moveType )
					SpaceIntercept::removeForJustify ( this, mSpace->isCursorJustify ( ) ) ;
				else
					SpaceIntercept::removeForEvent ( this, *mSpace, _interceptMod ) ;
			}
		}
		}
	}
}
