/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
  Copyright (C) 2013  Jonathan D. Pelletier

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

# include "intercept/win/intercept.h"

# define GRABCOUNT 3

# define disp( sig ) \
	( sig.type->dispatch ? sig.type->dispatch ( & sig ) : 0 )

# define WPARAM_UPTYPE( wParam ) \
	( ( wParam == WM_KEYUP || wParam == WM_SYSKEYUP ) ? \
			MCR_UP : MCR_DOWN )

static mcr_Grabber * _keyGrabber = NULL ;
static mcr_Grabber * _moveGrabber = NULL ;
static mcr_Grabber * _scrollGrabber = NULL ;
static mcr_Grabber * _allGrabbers [ GRABCOUNT ] ;
static mcr_HIDEcho _echo ;
static mcr_Key _key ;
static mcr_MoveCursor _mc ;
static mcr_Scroll _scr ;
static mcr_Signal _echoSig = { & mcr_iHIDEcho, { & _echo, 0 } },
		_keySig = { & mcr_iKey, { & _key, 0 } },
		_mcSig = { & mcr_iMoveCursor, { & _mc, 0 } },
		_scrSig = { & mcr_iScroll, { & _scr, 0 } } ;
//static int _vkset [ ] =
//{
//	VK_SHIFT, VK_CONTROL, VK_MENU, VK_LWIN, VK_RWIN
//} ;
//static unsigned int _modset [ ] =
//{
//	MCR_SHIFT, MCR_CTRL, MCR_ALT, MCR_WIN, MCR_META
//} ;
//static unsigned int _modsetLen = sizeof ( _vkset ) /
//		sizeof ( int ) ;
//
static DWORD _echoFlags [ ] = {
	MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP, MOUSEEVENTF_MIDDLEDOWN,
	MOUSEEVENTF_MIDDLEUP, MOUSEEVENTF_RIGHTDOWN, MOUSEEVENTF_RIGHTUP,
	MOUSEEVENTF_XDOWN, MOUSEEVENTF_XUP
} ;
static unsigned int _echoLen = sizeof ( _echoFlags ) / sizeof ( DWORD ) ;

static int verify_key_state ( PBYTE keyState, size_t keyState_size ) ;
static LRESULT CALLBACK key_proc ( int nCode, WPARAM wParam,
		LPARAM lParam ) ;
static LRESULT CALLBACK move_proc ( int nCode, WPARAM wParam,
		LPARAM lParam ) ;
static LRESULT CALLBACK scroll_proc ( int nCode, WPARAM wParam,
		LPARAM lParam ) ;

int mcr_intercept_enabled ( )
{
	for ( int i = 0 ; i < GRABCOUNT ; i ++ )
	{
		if ( MCR_GRABBER_ENABLED ( * _allGrabbers [ i ] ) )
			return 1 ;
	}
	return 0 ;
}

void mcr_intercept_set_enabled ( int enable )
{
	for ( int i = 0 ; i < GRABCOUNT ; i ++ )
	{
		mcr_Grabber_set_enabled ( _allGrabbers [ i ], enable ) ;
	}
}

unsigned int mcr_intercept_get_mods ( )
{
	unsigned int values = 0 ;
	BYTE keyState [ 0x100 ] ;
	// If cannot get the mod state, return current
	if ( ! verify_key_state ( keyState, 0xFF ) )
		return mcr_internalMods ;

	size_t modlen = mcr_Mod_key_count ( ) ;
	int * modkeybuffer = malloc ( sizeof ( int ) *
		modlen ) ;
	if ( ! modkeybuffer ) return mcr_internalMods ;

	mcr_Mod_key_all ( modkeybuffer, modlen ) ;

	for ( size_t i = 0 ; i < modlen ; i ++ )
	{
		// high bit is down, lowest byte is toggled.
		// high bit of byte is 0x80, high bit of short is 0x8000
		if ( ( keyState [ modkeybuffer [ i ] ] & 0x8080 ) != 0 )
		{
			values |= mcr_Mod_from_key ( modkeybuffer [ i ] ) ;
		}
	}
	free ( modkeybuffer ) ;
	return values ;
}

void mcr_intercept_enable_key ( int enable )
{
	mcr_Grabber_set_enabled ( _keyGrabber, enable ) ;
}

void mcr_intercept_enable_move ( int enable )
{
	mcr_Grabber_set_enabled ( _moveGrabber, enable ) ;
}

void mcr_intercept_enable_scroll ( int enable )
{
	mcr_Grabber_set_enabled ( _scrollGrabber, enable ) ;
}

static LRESULT CALLBACK key_proc ( int nCode, WPARAM wParam,
		LPARAM lParam )
{
	if ( nCode == HC_ACTION )
	{
		KBDLLHOOKSTRUCT * p = ( KBDLLHOOKSTRUCT * ) lParam ;
		MCR_KEY_SET_KEY ( _key, p->vkCode ) ;
		MCR_KEY_SET_SCAN ( _key, p->scanCode ) ;
		MCR_KEY_SET_UP_TYPE ( _key, ( WPARAM_UPTYPE ( wParam ) ) ) ;
		if ( disp ( _keySig ) )
			return -1 ;
	}
	return CallNextHookEx ( _keyGrabber->id, nCode, wParam, lParam ) ;
}

static LRESULT CALLBACK move_proc ( int nCode, WPARAM wParam,
		LPARAM lParam )
{
	if ( nCode == HC_ACTION )
	{
		MSLLHOOKSTRUCT * p = ( MSLLHOOKSTRUCT * ) lParam ;
		// Check for movecursor
		if ( ( p->flags & MOUSEEVENTF_MOVE ) )
		{
			MCR_MOVECURSOR_SET_COORDINATE ( _mc, MCR_X, p->pt.x ) ;
			MCR_MOVECURSOR_SET_COORDINATE ( _mc, MCR_Y, p->pt.y ) ;
			// 0 absolute, 1 justify/relative
			MCR_MOVECURSOR_SET_JUSTIFY ( _mc,
					! ( p->flags & MOUSEEVENTF_ABSOLUTE ) ) ;
			if ( disp ( _mcSig ) )
				return -1 ;
		}
		for ( unsigned int i = 0 ; i < _echoLen ; i++ )
		{
			if ( ( p->flags & _echoFlags [ i ] ) )
			{
				MCR_ECHO_SET_ECHO ( _echo, i ) ;
				if ( disp ( _echoSig ) )
					return -1 ;
			}
		}
	}
	return CallNextHookEx ( _moveGrabber->id, nCode, wParam, lParam ) ;
}

static LRESULT CALLBACK scroll_proc ( int nCode, WPARAM wParam,
		LPARAM lParam )
{
	if ( nCode == HC_ACTION )
	{
		MSLLHOOKSTRUCT * p = ( MSLLHOOKSTRUCT * ) lParam ;
		WORD delta = GET_WHEEL_DELTA_WPARAM
				( ( p->mouseData >> sizeof ( WORD ) ) ) ;
		switch ( wParam )
		{
		case WM_MOUSEWHEEL :
			MCR_SCROLL_SET_COORDINATE ( _scr, MCR_X, 0 ) ;
			MCR_SCROLL_SET_COORDINATE ( _scr, MCR_Y, delta ) ;
			break ;
		case WM_MOUSEHWHEEL :
			MCR_SCROLL_SET_COORDINATE ( _scr, MCR_X, delta ) ;
			MCR_SCROLL_SET_COORDINATE ( _scr, MCR_Y, 0 ) ;
			break ;
		}

		if ( disp ( _scrSig ) )
			return -1 ;
	}
	return CallNextHookEx ( _scrollGrabber->id, nCode, wParam, lParam ) ;
}

static int verify_key_state ( PBYTE keyState, size_t keyState_size )
{
	BYTE key = ( BYTE ) VK_RETURN ;
	short result ; // Only used to refresh kb state.
	// Only get the keyboard state if not done already. The only way to
	// check for this is if keyState is in the right format.
	if ( keyState == NULL || keyState_size < 0xFF )
		return 0 ;

	// sample key state so windows refreshes keyboard state
	result = GetKeyState ( key ) ;

	// Get pressed keys, error if returned false
	if ( ! GetKeyboardState ( keyState ) )
	{
		dmsg ;
		return 0 ;
	}
	return 1 ;
}

void mcr_intercept_native_initialize ( )
{
	int i ;
	for ( i = 0 ; i < GRABCOUNT ; i ++ )
	{
		for ( int j = 0 ; ! _allGrabbers [ i ] && j < 10 ; j ++ )
		{
			_allGrabbers [ i ] = malloc ( sizeof ( mcr_Grabber ) ) ;
		}
		if ( ! _allGrabbers [ i ] )
		{
			dmsg ;
			for ( i = 0 ; i < GRABCOUNT ; i ++ )
			{
				free ( _allGrabbers [ i ] ) ;
			}
			exit ( thrd_nomem ) ;
		}
	}
	i = 0 ;
	_keyGrabber = _allGrabbers [ i ++ ] ;
	_moveGrabber = _allGrabbers [ i ++ ] ;
	_scrollGrabber = _allGrabbers [ i ++ ] ;
	mcr_Grabber_init_with ( _keyGrabber, WH_KEYBOARD_LL, key_proc ) ;
	mcr_Grabber_init_with ( _moveGrabber, WH_MOUSE_LL, move_proc ) ;
	mcr_Grabber_init_with ( _scrollGrabber, WM_MOUSEWHEEL, scroll_proc ) ;

	mcr_Echo_init_with ( & _echo, MCR_ANY_MOD ) ;
	mcr_Key_init_with ( & _key, MCR_ANY_KEY, MCR_ANY_KEY, MCR_BOTH ) ;
	mcr_SpacePosition nopos = { 0 } ;
	mcr_Scroll_init_with ( & _scr, nopos ) ;
	mcr_cursor_position ( nopos ) ;
	mcr_MoveCursor_init_with ( & _mc, nopos, 0 ) ;
}

void mcr_intercept_native_cleanup ( void )
{
	for ( int i = 0 ; i < GRABCOUNT ; i ++ )
	{
		// Will block while waiting to finish.
		mcr_Grabber_free ( _allGrabbers [ i ] ) ;
		free ( _allGrabbers [ i ] ) ;
		_allGrabbers [ i ] = NULL ;
	}
	_keyGrabber = _moveGrabber = _scrollGrabber = NULL ;
}
