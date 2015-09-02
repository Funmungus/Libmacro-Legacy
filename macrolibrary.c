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

# include "macro.h"

void mcr_library_initialize ( int loadContracts )
{
	mcr_set_stdio ( ) ;
	mcr_dclock ( clock_t start ) ;
	mcr_reg_cleanup ( mcr_library_cleanup ) ;
	mcr_profile ( start ) ;
	mcr_dclock ( start ) ;
	mcr_signal_initialize ( ) ;
	mcr_profile ( start ) ;
	mcr_dclock ( start ) ;
	mcr_hotkey_initialize ( ) ;
	mcr_profile ( start ) ;
	mcr_dclock ( start ) ;
	mcr_intercept_initialize ( ) ;
	mcr_profile ( start ) ;
# ifdef MCR_EXTRAS
	mcr_dclock ( start ) ;
	mcr_extras_initialize ( ) ;
	mcr_profile ( start ) ;
	if ( loadContracts )
	{
		mcr_dclock ( start ) ;
		mcr_load_contracts ( ) ;
		mcr_profile ( start ) ;
	}
# else
	if ( loadContracts )
	{
		mcr_Signal_load_contract ( ) ;
		mcr_Key_load_contract ( ) ;
		mcr_Echo_load_contract ( ) ;
		mcr_Hot_load_contract ( ) ;
		mcr_Mods_load_contract ( ) ;
	}
# endif
}

void mcr_library_cleanup ( void )
{
	mcr_dclock ( clock_t start ) ;
# ifdef MCR_EXTRAS
	mcr_extras_cleanup ( ) ;
	mcr_profile ( start ) ;
	mcr_dclock ( start ) ;
# endif
	mcr_intercept_cleanup ( ) ;
	mcr_profile ( start ) ;
	mcr_dclock ( start ) ;
	mcr_hotkey_cleanup ( ) ;
	mcr_profile ( start ) ;
	mcr_dclock ( start ) ;
	mcr_signal_cleanup ( ) ;
	mcr_profile ( start ) ;
}
