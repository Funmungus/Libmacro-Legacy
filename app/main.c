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

# include "macronsole.h"

static mtx_t _mtx ;
static cnd_t _cnd ;
static int _continue = 1 ;
static int _pause = 0 ;

static void main_initialize ( ) ;

int main ( int argc, char ** argv )
{
	main_initialize ( ) ;
	if ( argc )
	{
		mcr_Script_option ( argc, argv ) ;
	}
	// May begin paused from script.
	mtx_lock ( & _mtx ) ;
	while ( _pause && _continue )
	{
		cnd_wait ( & _cnd, & _mtx ) ;
	}
	mtx_unlock ( & _mtx ) ;
	while ( _continue )
	{
		// If input is received always consume it.
		if ( ! _pause )
		{
			get_input ( ) ;
			filter_input ( ) ;
			do_input ( ) ;
		}
		mtx_lock ( & _mtx ) ;
		while ( _pause && _continue )
		{
			cnd_wait ( & _cnd, & _mtx ) ;
		}
		mtx_unlock ( & _mtx ) ;
	}
}

void pause_input ( )
{
	_pause = 1 ;
}

void continue_input ( )
{
	mtx_lock ( & _mtx ) ;
	_pause = 0 ;
	cnd_broadcast ( & _cnd ) ;
	mtx_unlock ( & _mtx ) ;
}

void end_input ( )
{
	mtx_lock ( & _mtx ) ;
	_pause = _continue = 0 ;
	cnd_broadcast ( & _cnd ) ;
	mtx_unlock ( & _mtx ) ;
}

static void main_cleanup ( void )
{
	main_native_cleanup ( ) ;
}

static void main_initialize ( )
{
	mcr_reg_cleanup ( main_cleanup ) ;
	mcr_library_initialize ( ) ;
	mcr_load_contracts ( ) ;
	mcr_standard_enable ( 1 ) ;
	main_native_initialize ( ) ;
	size_t ids [ ] =
	{
		-1, mcr_iHIDEcho.interface.id,
		mcr_iKey.interface.id, mcr_iMoveCursor.interface.id
	} ;
	mcr_Dispatch * dispPt ;
	for ( size_t i = 0 ; i < sizeof ( ids ) / sizeof ( size_t ) ; i ++ )
	{
		dispPt = mcr_Dispatch_get ( ids [ i ] ) ;
		dassert ( dispPt ) ;
		dispPt->enable_specific = dispPt->enable_unspecific = 1 ;
		mcr_Dispatch_enable ( dispPt, 1 ) ;
	}
	if ( mtx_init ( & _mtx, mtx_plain ) != thrd_success )
	{
		dmsg ;
		exit ( 1 ) ;
	}
	if ( cnd_init ( & _cnd ) != thrd_success )
	{
		dmsg ;
		exit ( 1 ) ;
	}
}
