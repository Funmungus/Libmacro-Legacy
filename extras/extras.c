/* extras/extras.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "extras/extras.h"

void mcr_extras_initialize ( )
{
	mcr_safestring_initialize ( ) ;
	mcr_signalextras_initialize ( ) ;
	mcr_script_initialize ( ) ;
	mcr_macroset_initialize ( ) ;
	mcr_extras_native_initialize ( ) ;
}

void mcr_extras_cleanup ( void )
{
	mcr_extras_native_cleanup ( ) ;
	mcr_macroset_cleanup ( ) ;
	mcr_script_cleanup ( ) ;
	mcr_signalextras_cleanup ( ) ;
	mcr_safestring_cleanup ( ) ;
}
