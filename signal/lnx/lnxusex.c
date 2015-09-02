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

static Display * _dpy = NULL ;

static XButtonEvent _eEv ;
static mtx_t _eLock ;
static XKeyEvent _kEv ;
static mtx_t _kLock ;
static XButtonEvent _scrEv ;
static mtx_t _scrLock ;

static struct timespec _timeout = { 1, 0 } ;

unsigned int mcr_xMods = 0 ;

void mcr_usex_enable ( int enable )
{
	if ( enable )
	{
		mcr_iHIDEcho.send = mcr_HIDEcho_usex_send ;
		mcr_iKey.send = mcr_Key_usex_send ;
		mcr_iMoveCursor.send = mcr_MoveCursor_usex_send ;
		mcr_iScroll.send = mcr_Scroll_usex_send ;
	}
	else
	{
		mcr_iHIDEcho.send = mcr_Echo_send ;
		mcr_iKey.send = mcr_Key_send ;
		mcr_iMoveCursor.send = mcr_MoveCursor_send ;
		mcr_iScroll.send = mcr_Scroll_send ;
	}
}

int mcr_HIDEcho_usex_send ( mcr_Signal * sigPt )
{
	dassert ( sigPt ) ;
	mcr_HIDEcho * echoPt = sigPt->data.data ;
	if ( ! echoPt )
		return 1 ;
	int status = mtx_timedlock ( & _eLock, & _timeout ) ;
	int revert ;
	_eEv.root = XDefaultRootWindow ( _eEv.display ) ;
	XGetInputFocus ( _eEv.display, & _eEv.window, & revert ) ;
	_eEv.state = mcr_xMods ;

	mcr_Key * keyPt ;
	keyPt = MCR_ARR_AT ( & mcr_echoEvents,
			( unsigned int ) MCR_ECHO_ECHO ( echoPt ) ) ;
	if ( keyPt )
	{
		_eEv.time = CurrentTime ;
		// difference between this btn and first btn key code
		// is also the difference from Xlib Button1
		switch ( MCR_KEY_KEY ( keyPt ) )
		{
		case BTN_LEFT :
		case BTN_0 :
			_eEv.button = Button1 ;
			break ;
		case BTN_MIDDLE :
		case BTN_1 :
			_eEv.button = Button2 ;
			break ;
		case BTN_RIGHT :
		case BTN_2 :
			_eEv.button = Button3 ;
			break ;
		default :
			dmsg ;
			return 0 ;
		}

		if ( MCR_KEY_UP_TYPE ( keyPt ) == MCR_DOWN )
		{
			_eEv.type = ButtonPress ;
			XSendEvent ( _eEv.display, _eEv.window, True,
					ButtonPressMask, ( XEvent * ) & _eEv ) ;
		}
		else
		{
			_eEv.type = ButtonRelease ;
			XSendEvent ( _eEv.display, _eEv.window, True,
					ButtonReleaseMask, ( XEvent * ) & _eEv ) ;
		}
	}
	else
	{ dmsg ; return 0 ; }

	if ( status == thrd_success )
		mtx_unlock ( & _eLock ) ;

	return 1 ;
}

int mcr_Key_usex_send ( mcr_Signal * sigPt )
{
	dassert ( sigPt ) ;
	mcr_Key * keyPt = sigPt->data.data ;
	if ( ! keyPt )
		return 1 ;
	int status = mtx_timedlock ( & _kLock, & _timeout ) ;
	int revert ;
	_kEv.root = XDefaultRootWindow ( _kEv.display ) ;
	XGetInputFocus ( _kEv.display, & _kEv.window, & revert ) ;
	_kEv.state = mcr_xMods ;

	_kEv.keycode = ( unsigned int ) MCR_KEY_KEY ( keyPt ) ;
	if ( MCR_KEY_UP_TYPE ( keyPt ) != MCR_UP )
	{
		_kEv.time = CurrentTime ;
		_kEv.type = KeyPress ;
		XSendEvent ( _kEv.display, _kEv.window, True, KeyPressMask,
				( XEvent * ) & _kEv ) ;
	}
	if ( MCR_KEY_UP_TYPE ( keyPt ) != MCR_DOWN )
	{
		_kEv.time = CurrentTime ;
		_kEv.type = KeyRelease ;
		XSendEvent ( _kEv.display, _kEv.window, True, KeyReleaseMask,
				( XEvent * ) & _kEv ) ;
	}

	if ( status == thrd_success )
		mtx_unlock ( & _kLock ) ;

	return 1 ;
}

int mcr_MoveCursor_usex_send ( mcr_Signal * sigPt )
{
	dassert ( sigPt ) ;
	mcr_MoveCursor * mcPt = sigPt->data.data ;
	if ( ! mcPt )
		return 1 ;

	if ( MCR_MOVECURSOR_JUSTIFY ( mcPt ) )
	{
		XWarpPointer ( _dpy, None, None, 1, 1, 1, 1,
				MCR_MOVECURSOR_COORDINATE ( mcPt, MCR_X ),
				MCR_MOVECURSOR_COORDINATE ( mcPt, MCR_Y ) ) ;
	}
	else
	{
		Window root = XDefaultRootWindow ( _dpy ) ;
		Window rtRet, chldRet ;
		int rtX, rtY, wX, wY ;
		unsigned int msk ;
		XQueryPointer ( _dpy, root, & rtRet,
				& chldRet, & rtX, & rtY, & wX, & wY, & msk ) ;
		// Amount to justify is the ( final absolute position )
		// minus ( current position )
		wX = MCR_MOVECURSOR_COORDINATE ( mcPt, MCR_X ) - rtX ;
		wY = MCR_MOVECURSOR_COORDINATE ( mcPt, MCR_Y ) - rtY ;
		XWarpPointer ( _dpy, None, None, 1, 1, 1, 1,
				wX, wY ) ;
	}

	return 1 ;
}

int mcr_Scroll_usex_send ( mcr_Signal * sigPt )
{
	dassert ( sigPt ) ;
	mcr_Scroll * scrPt = sigPt->data.data ;
	if ( ! scrPt )
		return 1 ;
	int status = mtx_timedlock ( & _scrLock, & _timeout ) ;
	int revert ;
	_scrEv.root = XDefaultRootWindow ( _scrEv.display ) ;
	XGetInputFocus ( _scrEv.display, & _scrEv.window, & revert ) ;
	_scrEv.state = mcr_xMods ;

	int x = ( int ) MCR_SCROLL_COORDINATE ( scrPt, MCR_X ),
			y = ( int ) MCR_SCROLL_COORDINATE ( scrPt, MCR_Y ) ;

# define scrloop \
	_scrEv.time = CurrentTime ; \
	_scrEv.type = ButtonPress ; \
	XSendEvent ( _scrEv.display, _scrEv.window, True, \
			ButtonPressMask, ( XEvent * ) & _scrEv ) ; \
	_scrEv.time = CurrentTime ; \
	_scrEv.type = ButtonRelease ; \
	XSendEvent ( _scrEv.display, _scrEv.window, True, \
			ButtonReleaseMask, ( XEvent * ) & _scrEv ) ;
	if ( x > 0 )
	{
		_scrEv.button = SCR_UP ;
		while ( x -- > 0 )
		{
			scrloop
		}
	}
	else if ( x < 0 )
	{
		_scrEv.button = SCR_DOWN ;
		while ( x ++ < 0 )
		{
			scrloop
		}
	}

	if ( y > 0 )
	{
		_scrEv.button = SCR_RIGHT ;
		while ( y -- > 0 )
		{
			scrloop
		}
	}
	else if ( y < 0 )
	{
		_scrEv.button = SCR_LEFT ;
		while ( y ++ < 0 )
		{
			scrloop
		}
	}

	if ( status == thrd_success )
		mtx_unlock ( & _scrLock ) ;

	return 1 ;
}

void mcr_usex_initialize ( )
{
	_dpy = _kEv.display = _eEv.display =
			_scrEv.display = XOpenDisplay ( 0 ) ;
	dassert ( _kEv.display ) ;
	_kEv.subwindow = _eEv.subwindow =
			_scrEv.subwindow = None ;
	_kEv.x = _kEv.y = _kEv.x_root = _kEv.y_root =
			_eEv.x = _eEv.y = _eEv.x_root = _eEv.y_root =
			_scrEv.x = _scrEv.y = _scrEv.x_root = _scrEv.y_root = 1 ;
	_kEv.same_screen = _eEv.same_screen =
			_scrEv.same_screen = True ;

	mtx_init ( & _eLock, mtx_timed ) ;
	mtx_init ( & _kLock, mtx_timed ) ;
	mtx_init ( & _scrLock, mtx_timed ) ;
}

void mcr_usex_cleanup ( void )
{
	if ( _kEv.display )
		XCloseDisplay ( _kEv.display ) ;
}
