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
