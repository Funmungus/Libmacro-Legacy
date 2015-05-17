/* macro.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "macro.h"

void mcr_macro_initialize ( )
{
	mcr_reg_cleanup_filed ( mcr_macro_cleanup, __FILE__ ) ;
	mcr_signal_initialize ( ) ;
	mcr_hotkey_initialize ( ) ;
	mcr_intercept_initialize ( ) ;
}

void mcr_macro_cleanup ( void )
{
	mcr_intercept_cleanup ( ) ;
	mcr_hotkey_cleanup ( ) ;
	mcr_signal_cleanup ( ) ;
}
