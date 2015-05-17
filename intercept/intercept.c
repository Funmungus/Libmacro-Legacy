/* intercept/intercept.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "intercept/intercept.h"

void mcr_reset_mods ( )
{
	mcr_internalMods = mcr_intercept_get_mods ( ) ;
}

void mcr_intercept_initialize ( )
{
	mcr_intercept_native_initialize ( ) ;
}

void mcr_intercept_cleanup ( void )
{
	mcr_intercept_native_cleanup ( ) ;
}
