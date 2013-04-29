
# ifdef WIN
//# include "stdafx.h"


# include "hotkey/hook.h"

using namespace std ;

namespace funlibrary
{
	Hook::Hook ( const int hookType, HOOKPROC hookingProc, bool beginEnabled )
		: _enable ( false ), _hookType ( hookType ), _hookedProc ( hookingProc ), _hModule ( NULL ), _hookId ( NULL )
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
			_enableLock.lock ( ) ;
			enable_impl ( true ) ;
			_enableLock.unlock ( ) ;
		}
		// Disabling. If currently enabled, disable.
		else if ( isEnabled )
		{
			HANDLE hThread = 0 ;
			DWORD dwThread = 0 ;
			hThread = CreateThread ( NULL, NULL, ( LPTHREAD_START_ROUTINE ) threadDisable,
				( LPVOID ) this, NULL, & dwThread ) ;
		}
	}

	bool Hook::isEnabled ( )
	{
		return _hookId != NULL ;
	}

	void Hook::reset ( )
	{
		HANDLE hThread = 0 ;
		DWORD dwThread = 0 ;
		hThread = CreateThread ( NULL, NULL, ( LPTHREAD_START_ROUTINE ) threadReset,
			( LPVOID ) this, NULL, & dwThread ) ;
	}

	void Hook::installHook ( )
	{
		_hModule = GetModuleHandle ( NULL ) ;
		//int hookType = getHookType ( ) ;
		//HOOKPROC mProc = getHookedProc ( ) ;
		// This can return NULL if invalid values.
		_hookId = SetWindowsHookEx ( _hookType, _hookedProc, _hModule, 0 ) ;
	}

	void Hook::uninstallHook ( )
	{
		if ( _hookId != NULL )
		{
			UnhookWindowsHookEx ( _hookId ) ;
			_hookId = NULL ;
			_hModule = NULL ;
		}
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

	/*void Hook::setHookId ( HHOOK value )
	{
		_hookId = value ;
	}*/

	/*
	HHOOK hMouseHook ;
	volatile bool continuing = true ;

	LRESULT CALLBACK mouseProc ( int nCode, WPARAM wParam, LPARAM lParam )
	{
		MOUSEHOOKSTRUCT * pMouseStruct = ( MOUSEHOOKSTRUCT * ) lParam ;
		if ( pMouseStruct != NULL ) {
			if ( wParam == WM_LBUTTONDOWN )
			{
				printf ( "clicked" ) ;
			}
			printf ( "Mouse position X = %d Mouse Position Y = %d\n", pMouseStruct->pt.x, pMouseStruct->pt.y ) ;
			if ( pMouseStruct->pt.x != 0 )
			{
				continuing = false ;
				printf ( "No longer pausing.\n" ) ;
			}
		}
		return CallNextHookEx ( hMouseHook, nCode, wParam, lParam ) ;
	}
	*/

	DWORD WINAPI Hook::threadReceiveHooking ( LPVOID lpParam )
	{
		//HINSTANCE hInstance = GetModuleHandle ( NULL ) ;
		//
		//DWORD id = GetCurrentThreadId ( ) ;
		//// here I put WH_MOUSE instead of WH_MOUSE_LL
		//hMouseHook = SetWindowsHookEx ( WH_MOUSE_LL, mouseProc, hInstance, 0 ) ;
		//
		//MSG message ;
		//while ( GetMessage ( & message, NULL, 0, 0 ) ) {
		//	TranslateMessage ( & message ) ;
		//	DispatchMessage ( & message ) ;
		//}
		//
		//UnhookWindowsHookEx ( hMouseHook ) ;

		Hook * me = ( Hook * ) lpParam ;
		try
		{
			me->installHook ( ) ;
		}
		catch (... )
		{
			fprintf ( stderr, "Error found while Enabling hook.\n" ) ;
			me->uninstallHook ( ) ;
			return -1 ;
		}

		MSG message ;
		while ( me->_enable && GetMessage ( & message, NULL, 0, 0 ) ) {
			TranslateMessage ( & message ) ;
			DispatchMessage ( & message ) ;
		}

		me->uninstallHook ( ) ;
		return 0 ;
	}

	void Hook::enable_impl ( const bool enable )
	{
		bool isEnabled = this->isEnabled ( ) ;
		if ( enable )
		{
			if ( ! isEnabled )
			{
				_enable = true ;
				_hThread = CreateThread ( NULL, NULL, ( LPTHREAD_START_ROUTINE ) threadReceiveHooking,
					( LPVOID ) this, NULL, & _dwThread ) ;
			}
			//// When enabling and already enabled, reset.
			//else
			//{
			//	reset ( ) ;
			//}
		}
		// Disabling. If currently enabled, disable.
		else if ( isEnabled )
		{
			_enable = false ;

			if ( _hThread )
				WaitForSingleObject ( _hThread, INFINITE ) ;
			_hThread = NULL ;

			if ( _hookId )
				UnhookWindowsHookEx ( _hookId ) ;
			_hookId = NULL ;
		}
	}
	/*
	DWORD WINAPI Hook::threadEnable ( LPVOID lpParam )
	{
		Hook * me = ( Hook * ) lpParam ;
		me->enable_impl ( true ) ;
		return 0 ;
	}
	*/
	DWORD WINAPI Hook::threadDisable ( LPVOID lpParam )
	{
		Hook * me = ( Hook * ) lpParam ;
		try
		{
			me->_enableLock.lock ( ) ;
			me->enable_impl ( false ) ;
			me->_enableLock.unlock ( ) ;
		}
		catch (... )
		{
			fprintf ( stderr, "Error : Disabling thread in Hook\n" ) ;
		}
		return 0 ;
	}
	DWORD WINAPI Hook::threadReset ( LPVOID lpParam )
	{
		Hook * me = ( Hook * ) lpParam ;
		try
		{
			me->_enableLock.lock ( ) ;
			me->enable_impl ( true ) ;
			me->enable_impl ( false ) ;
			me->_enableLock.unlock ( ) ;
		}
		catch (... )
		{
			fprintf ( stderr, "Error : Disabling thread in Hook\n" ) ;
		}
		return 0 ;
	}
}

//# include "winhook.hxx"
# endif
