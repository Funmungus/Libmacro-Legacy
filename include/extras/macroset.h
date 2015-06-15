/* include/extras/set.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# ifndef MCR_MACROSET_H
# define MCR_MACROSET_H

# include "extras/signaloption.h"

typedef struct mcr_MacroSet
{
	/* NULL type for generic dispatch. */
	mcr_Signal dispatch_specifier ;
	unsigned int dispatch_mods ;
	mcr_Macro macro ;
} mcr_MacroSet ;

typedef int ( * mcr_macro_option_fnc ) ( int argc, char ** argv,
		int index, mcr_Array * mcrNamePt, mcr_MacroSet * mcrPt ) ;

MCR_API void mcr_MacroSet_init ( mcr_MacroSet * set ) ;
MCR_API void mcr_MacroSet_free ( mcr_MacroSet * set ) ;
MCR_API void mcr_MacroSet_free_foreach ( mcr_MacroSet * set, ... ) ;

MCR_API int mcr_MacroSet_option ( int argc, char ** argv,
		int index ) ;

MCR_API void mcr_macroset_initialize ( ) ;
MCR_API void mcr_macroset_cleanup ( void ) ;



# endif
