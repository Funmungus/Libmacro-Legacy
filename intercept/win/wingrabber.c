/* intercept/win/wingrabber.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "intercept/win/grabber.h"

static void enable_impl ( mcr_Grabber * grabPt, int enable ) ;
static void grab_hook ( mcr_Grabber * grabPt ) ;
static void grab_unhook ( mcr_Grabber * grabPt ) ;
static DWORD WINAPI thrd_receive_hooking ( LPVOID lpParam ) ;
static DWORD WINAPI thrd_disable ( LPVOID lpParam ) ;

void mcr_Grabber_init ( mcr_Grabber * grabPt )
{
	if ( ! grabPt )
	{
		dmsg ;
		return ;
	}
	memset ( grabPt, 0, sizeof ( mcr_Grabber ) ) ;
	mtx_init ( & grabPt->lock, mtx_plain ) ;
}

void mcr_Grabber_init_with ( mcr_Grabber * grabPt, int type,
		HOOKPROC proc )
{
	if ( ! grabPt )
	{
		dmsg ;
		return ;
	}
	mcr_Grabber_init ( grabPt ) ;
	grabPt->type = type ;
	grabPt->proc = proc ;
}

void mcr_Grabber_free ( mcr_Grabber * grabPt )
{
	if ( ! grabPt )
	{
		dmsg ;
		return ;
	}
	mtx_lock ( & grabPt->lock ) ;
	grab_unhook ( grabPt ) ;
	mtx_unlock ( & grabPt->lock ) ;
	if ( grabPt->hThread )
	{
		WaitForSingleObject ( grabPt->hThread, INFINITE ) ;
		CloseHandle ( grabPt->hThread ) ;
		grabPt->hThread = NULL ;
	}
	mtx_destroy ( & grabPt->lock ) ;
}

void mcr_Grabber_enable ( mcr_Grabber * grabPt, int enable )
{
	dassert ( grabPt ) ;
	mtx_lock ( & grabPt->enable_lock ) ;
	enable_impl ( grabPt, enable ) ;
	mtx_unlock ( & grabPt->enable_lock ) ;
}

int mcr_Grabber_is_enabled ( mcr_Grabber * grabPt )
{
	dassert ( grabPt ) ;
	return MCR_GRABBER_ENABLED ( grabPt ) ;
}

/*
HHOOK hMouseHook ;

LRESULT CALLBACK mouseProc ( int nCode, WPARAM wParam, LPARAM lParam )
{
	MOUSEHOOKSTRUCT * pMouseStruct = ( MOUSEHOOKSTRUCT * ) lParam ;
	if ( pMouseStruct != NULL ) {
		if ( wParam == WM_LBUTTONDOWN )
		{
			fprintf ( "clicked" ) ;
		}
		fprintf ( "Mouse position X = %d Mouse Position Y = %d\n", pMouseStruct->pt.x, pMouseStruct->pt.y ) ;
		if ( pMouseStruct->pt.x != 0 )
		{
			continuing = false ;
			fprintf ( "No longer pausing.\n" ) ;
		}
	}
	return CallNextHookEx ( hMouseHook, nCode, wParam, lParam ) ;
}
*/

static void enable_impl ( mcr_Grabber * grabPt, int enable )
{
	dassert ( grabPt ) ;
	int isEnabled = MCR_GRABBER_ENABLED ( grabPt ) ;
	if ( enable )
	{
		/* Not already enabled, can enable, and does not have
		 * an intercepting thread. */
		if ( ! isEnabled && ! grabPt->hThread )
		{
			grabPt->hThread = CreateThread ( NULL, NULL,
					( LPTHREAD_START_ROUTINE ) thrd_receive_hooking,
					( LPVOID ) grabPt, NULL, & grabPt->dwThread ) ;
		}
	}
	else if ( isEnabled )
	{
		/* Disabling an enabled thread. In order to disable from
		 * a currently intercepting thread, we disable from another
		 * thread. */
		DWORD dwThread = 0 ;
		HANDLE hThread = CreateThread ( NULL, NULL,
				( LPTHREAD_START_ROUTINE ) thrd_disable,
				( LPVOID ) grabPt, NULL, & grabPt->dwThread ) ;
		CloseHandle ( hThread ) ;
	}
}

static void grab_hook ( mcr_Grabber * grabPt )
{
	dassert ( grabPt ) ;
	if ( ! MCR_GRABBER_ENABLED ( grabPt ) )
	{
		grabPt->hModule = GetModuleHandle ( NULL ) ;
		// This can return NULL if invalid values.
		grabPt->id = SetWindowsHookEx ( grabPt->type, grabPt->proc,
				grabPt->hModule, 0 ) ;
	}
}

static void grab_unhook ( mcr_Grabber * grabPt )
{
	dassert ( grabPt ) ;
	if ( MCR_GRABBER_ENABLED ( grabPt ) )
	{
		UnhookWindowsHookEx ( grabPt->id ) ;
		grabPt->id = NULL ;
		grabPt->hModule = NULL ;
	}
}

static DWORD WINAPI thrd_receive_hooking ( LPVOID lpParam )
{
	mcr_Grabber * me = ( mcr_Grabber * ) lpParam ;
	mtx_lock ( & me->lock ) ;
	mcr_Grabber_hook ( me ) ;
	mtx_unlock ( & me->lock ) ;

	MSG message ;
	while ( MCR_GRABBER_ENABLED ( me ) &&
			GetMessage ( & message, NULL, 0, 0 ) )
	{
		TranslateMessage ( & message ) ;
		DispatchMessage ( & message ) ;
	}

	mtx_lock ( & me->lock ) ;
	mcr_Grabber_unhook ( me ) ;
	mtx_unlock ( & me->lock ) ;
	return 0 ;
}

static DWORD WINAPI thrd_disable ( LPVOID lpParam )
{
	mcr_Grabber * grabPt = ( mcr_Grabber * ) lpParam ;
	mtx_lock ( & grabPt->lock ) ;
	grab_unhook ( grabPt ) ;
	mtx_unlock ( & grabPt->lock ) ;

	if ( grabPt->hThread )
	{
		WaitForSingleObject ( grabPt->hThread, INFINITE ) ;
		CloseHandle ( grabPt->hThread ) ;
		grabPt->hThread = NULL ;
	}
	return 0 ;
}

# endif
