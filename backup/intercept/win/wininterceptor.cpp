
namespace funlibrary
{
	bool Dispatcher::_enable = true ;
	const int Dispatcher::_KEY_TYPE = WH_KEYBOARD_LL ;
	const int Dispatcher::_SPACE_TYPE = WH_MOUSE_LL ;
	const int Dispatcher::_WHEEL_TYPE = WM_MOUSEWHEEL ;
	Hook Dispatcher::_keyHook ( Dispatcher::_KEY_TYPE, Dispatcher::keyCallback, false ) ;
	Hook Dispatcher::_spaceHook ( Dispatcher::_SPACE_TYPE, Dispatcher::spaceCallback, false ) ;
	Hook Dispatcher::_wheelHook ( Dispatcher::_WHEEL_TYPE, Dispatcher::wheelCallback, false ) ;

	// free Util
	// ! \brief Deprecated.
	void createSignal ( WPARAM wParam, LPARAM lParam, KeySignal * outVal ) ;
	/// WPARAM of low keyboard function, determines key up type.
	KeyUpType getKeyUpFor ( WPARAM wParam ) ;
	/// <summary>
	/// postcondition : If keyState_size != 0xFF, or keyState is NULL, then keyState will be assigned a new BYTE [ 0xFF ]
	/// </summary>
	bool verifyKeyStateArr ( PBYTE keyState, size_t keyState_size ) ;
	/// <summary>
	/// TODO : This declaration may be moved into public KeyIntercept
	/// </summary>
	unsigned int getCurrentMods ( ) ;

	Dispatcher::Dispatcher ( )
	{
		InternalMods.set ( getMods ( ) ) ;
	}

	Dispatcher::~Dispatcher ( )
	{
}

	unsigned int Dispatcher::getMods ( )
	{
		unsigned int values = 0 ;
		BYTE keyState [ 0x100 ] ;
		// If cannot get the mod state, return "For all mods"
		if ( ! verifyKeyStateArr ( keyState, 0xFF ) )
			return ( unsigned int ) -1 ;

		static int vkset [ ] = { VK_SHIFT, VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN } ;
		static unsigned int modset [ ] = { Mods::getMod ( FUN_SHIFT ), Mods::getMod ( FUN_CTRL ),
			Mods::getMod ( FUN_ALT ), Mods::getMod ( FUN_META ),
			Mods::getMod ( FUN_META ) } ;
		static unsigned int len = sizeof ( vkset ) / sizeof ( vkset [ 0 ] ) ;

		for ( unsigned int i = 0 ; i < len ; i ++ )
		{
			// high bit is down, lowest byte is toggled.
			// high bit of byte is 0x80, high bit of short is 0x8000
			if ( ( keyState [ vkset [ i ] ] & 0x8080 ) != 0 )
			{
				// vk in keystate is down, and so add the mod value for that key state.
				values |= modset [ i ] ;
			}
		}

		return values ;
	}

	void Dispatcher::setKeyCallback ( const KeyCallback newCallback )
	{
		bool prev = _keyback != NULL ;
		_keyback = newCallback ;
		// When previously enabled, possibly disable.
		if ( prev )
		{
			if ( _keyback == NULL )
			{
				_keyHook.enable ( false ) ;
			}
		}
		// When disabled, possibly enable.
		else if ( _keyback != NULL )
		{
			_keyHook.enable ( true ) ;
		}
	}

	void Dispatcher::setSpaceCallback ( const SpaceCallback newCallback )
	{
		bool prev = _spaceback != NULL ;
		_spaceback = newCallback ;
		// When previously enabled, possibly disable.
		if ( prev )
		{
			if ( _spaceback == NULL )
			{
				_spaceHook.enable ( false ) ;
				_wheelHook.enable ( false ) ;
			}
		}
		// When disabled, possibly enable.
		else if ( _spaceback != NULL )
		{
			_spaceHook.enable ( true ) ;
			_wheelHook.enable ( true ) ;
		}
	}

void Dispatcher::enable ( bool enable )
	{
		bool prev = isEnabled ( ) ;
		if ( prev )
		{
			// Previously enabled, and now disabling all.
			if ( ! enable )
			{
				_keyHook.enable ( false ) ;
				_spaceHook.enable ( false ) ;
				_wheelHook.enable ( false ) ;
				_enable = false ;
			}
		}
		else
		{
			// Previously disabled, and now enabling only those with a callback.
			if ( enable )
			{
				if ( _keyback != NULL )
				{
					_keyHook.enable ( true ) ;
				}
				if ( _spaceback != NULL )
				{
					_spaceHook.enable ( true ) ;
					_wheelHook.enable ( true ) ;
				}
				_enable = true ;
			}
		}
	}

	bool Dispatcher::isEnabled ( )
	{
		if ( ! _enable )
			return false ;

		if ( _keyHook.isEnabled ( ) )
			return true ;
		if ( _spaceHook.isEnabled ( ) )
			return true ;
		if ( _wheelHook.isEnabled ( ) )
			return true ;
		return false ;
	}

	//TODO decide if reset should always end on true, or if reverting to original.
	void Dispatcher::reset ( )
	{
		enable ( false ) ;
		enable ( true ) ;
	}

	LRESULT CALLBACK Dispatcher::keyCallback ( int nCode, WPARAM wParam, LPARAM lParam )
	{
		if ( nCode == HC_ACTION )
		{
			KBDLLHOOKSTRUCT * p = ( KBDLLHOOKSTRUCT * ) lParam ;
			KeySignal modori ( p->vkCode, p->scanCode, getKeyUpFor ( wParam ) ) ;
			//Mods inMod ( getCurrentMods ( ) ) ;
			if ( modori.getKeyUp ( ) == DOWN )
			{
				InternalMods.add ( Mods::modFromKey ( modori.getKey ( ) ) ) ;
			}
			else
			{
				InternalMods.remove ( Mods::modFromKey ( modori.getKey ( ) ) ) ;
			}

			//bool isBlocking = call ( & modori, & inMod ) ;
			bool isBlocking = trigger ( & modori, & InternalMods ) ;

			if ( isBlocking )
				return -1 ;
		}

		return CallNextHookEx ( _keyHook.getHookId ( ), nCode, wParam, lParam ) ;
	}

	LRESULT CALLBACK Dispatcher::spaceCallback ( int nCode, WPARAM wParam, LPARAM lParam )
	{
		// TODO dynamically sync with SpaceSignal event button map.
		static DWORD flags [ ] = {
			MOUSEEVENTF_MOVE, MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP,
			MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP, MOUSEEVENTF_WHEEL, MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP
		} ;
		static unsigned int len = sizeof ( flags ) / sizeof ( flags [ 0 ] ) ;

		if ( nCode == HC_ACTION )
		{
			MSLLHOOKSTRUCT * p = ( MSLLHOOKSTRUCT * ) lParam ;
			DWORD flag = 0 ;
			for ( unsigned int i = 0 ; i < len ; i++ )
			{
				if ( ( p->flags & flags [ i ] ) != 0 )
				{
					flag = flags [ i ] ;
					break ;
				}
			}
			SpaceSignal modori ( flag, SpacePosition ( p->pt.x, p->pt.y, 0 ),
				( p->flags & MOUSEEVENTF_ABSOLUTE ) != 0, p->mouseData ) ;

			//bool isBlocking = call ( & modori, & inMod ) ;
			bool isBlocking = trigger ( & modori, & InternalMods ) ;

			if ( isBlocking )
				return -1 ;
		}

		return CallNextHookEx ( _spaceHook.getHookId ( ), nCode, wParam, lParam ) ;
	}

	LRESULT CALLBACK Dispatcher::wheelCallback ( int nCode, WPARAM wParam, LPARAM lParam )
	{
		if ( nCode == HC_ACTION )
		{
			MSLLHOOKSTRUCT * p = ( MSLLHOOKSTRUCT * ) lParam ;
			SpaceSignal modori ( MOUSEEVENTF_WHEEL, SpacePosition ( p->pt.x, p->pt.y, 0 ),
				( p->flags & MOUSEEVENTF_ABSOLUTE ) != 0, p->mouseData ) ;

			//bool isBlocking = call ( & modori, & inMod ) ;
			bool isBlocking = trigger ( & modori, & InternalMods ) ;

			if ( isBlocking )
				return -1 ;
		}

		return CallNextHookEx ( _spaceHook.getHookId ( ), nCode, wParam, lParam ) ;
	}

	// deprecated
	void createSignal ( WPARAM wParam, LPARAM lParam, KeySignal * outVal )
	{
		KBDLLHOOKSTRUCT * p = ( KBDLLHOOKSTRUCT * ) lParam ;
		outVal->setScan ( p->scanCode ) ;
		outVal->setKey ( p->vkCode ) ;
		outVal->setKeyUp ( getKeyUpFor ( wParam ) ) ;
	}

	KeyUpType getKeyUpFor ( WPARAM wParam )
	{
		//int up = WM_KEYUP ;
		//int sys = WM_SYSKEYUP ;

		//switch ( wParam )
		//{
		//case WM_SYSKEYUP:{ // use SYSKEYDOWN
		//	cout << "Sys key up" << endl ;
		//	break ;
		//}
		//case WM_KEYUP: // use regular keyup
		//{
		//	cout << "Key up" << endl ;
		//	break ;
		//}
		//case WM_KEYDOWN:
		//	cout << "KeyDOwn" << endl ;
		//	break ;
		//}

		if ( wParam == WM_KEYUP || wParam == WM_SYSKEYUP )
		{
			return KeyUpType::UP ;
		}
		return KeyUpType::DOWN ;
	}

	bool verifyKeyStateArr ( PBYTE keyState, size_t keyState_size )
	{
		BYTE key = ( BYTE ) VK_RETURN ;
		short result ; // Only used to refresh kb state.
		// Only get the keyboard state if not done already. The only way to
		// check for this is if keyState is in the right format.
		if ( keyState == NULL || keyState_size < 0xFF )
			return false ;

		// sample key state so windows refreshes keyboard state
		result = GetKeyState ( key ) ;

		// Get pressed keys, error if returned false
		if ( ! GetKeyboardState ( keyState ) )
		{
			return false ;
		}

		return true ;
	}
}
