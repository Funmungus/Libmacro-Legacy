/* macro.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "macro.h"

void mcr_library_initialize ( int loadContracts )
{
	mcr_set_stdio ( ) ;
	mcr_reg_cleanup ( mcr_library_cleanup ) ;
	mcr_set_stdio ( ) ;
	mcr_signal_initialize ( ) ;
	mcr_hotkey_initialize ( ) ;
	mcr_intercept_initialize ( ) ;
# ifdef MCR_EXTRAS
	mcr_extras_initialize ( ) ;
	if ( loadContracts )
		mcr_load_contracts ( ) ;
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
# ifdef MCR_EXTRAS
	mcr_extras_cleanup ( ) ;
# endif
	mcr_intercept_cleanup ( ) ;
	mcr_hotkey_cleanup ( ) ;
	mcr_signal_cleanup ( ) ;
}
