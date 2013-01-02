
# ifdef WIN
# include "stdafx.h"


# include "hotkey/hook.h"

using namespace std ;

namespace funlibrary
{
	Hook::Hook ( const int hookType, HOOKPROC hookingProc, bool beginEnabled )
		: _hookType ( hookType ), _hookedProc ( hookingProc ), _hookId ( NULL )
	{
		enable ( beginEnabled ) ;
	}

	Hook::~Hook ( )
	{
		enable ( false ) ;
	}

	void Hook::enable ( bool enable )
	{
		bool isEnabled = this->isEnabled ( ) ;
		if ( enable )
		{
			if ( ! isEnabled )
			{
				try
				{
					HHOOK newHandle = installHook ( ) ;
					setHookId ( newHandle ) ;
				}
				catch ( ... )
				{
					cerr << "Error found while Enabling hook." << endl ;
					this->enable ( false ) ;
				}
			}
			// When enabling and already enabled, reset.
			else
			{
				reset ( ) ;
			}
		}
		// Disabling.  If currently enabled, disable.
		else if ( isEnabled )
		{
			HHOOK mHandle = getHookId ( ) ;
			UnhookWindowsHookEx ( mHandle ) ;
			setHookId ( NULL ) ;
		}
	}

	bool Hook::isEnabled ( )
	{
		return getHookId ( ) != NULL ;
	}

	void Hook::reset ( )
	{
		enable ( false ) ;
		enable ( true ) ;
	}

	HHOOK Hook::installHook ( )
	{
		HMODULE hModule = GetModuleHandle ( NULL ) ;
		//int hookType = getHookType ( ) ;
		//HOOKPROC mProc = getHookedProc ( ) ;
		// This can return NULL if invalid values.
		return SetWindowsHookEx ( _hookType, _hookedProc, hModule, 0 ) ;
	}

	HOOKPROC Hook::getHookedProc ( ) const
	{
		return _hookedProc ;
	}

	void Hook::setHookedProc ( HOOKPROC value )
	{
		_hookedProc = value ;
	}

	HHOOK Hook::getHookId ( ) const
	{
		return _hookId ;
	}

	void Hook::setHookId ( HHOOK value )
	{
		_hookId = value ;
	}
}

//# include "winhook.hxx"
# endif

