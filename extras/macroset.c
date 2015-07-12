/* extras/macroset.c
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# include "extras/macroset.h"

MCR_API const char * mcr_macroOptionName = "macro" ;

static mcr_Map _set ;
static mcr_Map _macroOptions ;

int mcr_MacroSet_option ( int argc, char ** argv,
		int index )
{
	dassert ( argv ) ;
	dassert ( index < argc ) ;

	// Need at least one additional function to do on macro.
	MCR_OPTION_MIN_OPTS ( argc, argv, index, 1 ) ;
	mcr_macro_option_fnc * fPt = mcr_StringMap_get_value
			( & _macroOptions, argv [ index + 1 ] ) ;
	if ( ! fPt )
	{
		dmsg ;
		return index ; 	/* No further input processed. */
	}
	mcr_Array * found = mcr_StringMap_get ( & _set, argv [ index ] ) ;
	mcr_MacroSet * mcrFound = MCR_MAP_VALUE ( & _set, found ) ;
	if ( ! found )
	{
		if ( ! mcr_StringMap_map ( & _set, argv [ index ], NULL ) )
		{
			dmsg ;
			return index + 1 ;
		}
		found = mcr_Map_get ( & _set, argv + index ) ;
		mcrFound = MCR_MAP_VALUE ( & _set, found ) ;
		dassert ( found ) ;
		dassert ( mcrFound ) ;
		mcr_MacroSet_init ( mcrFound ) ;
	}
	return ( * fPt ) ( argc, argv,
			index + 1, found, mcrFound ) ;
}

void mcr_MacroSet_init ( mcr_MacroSet * set )
{
	if ( ! set )
	{
		dmsg ;
		return ;
	}
	memset ( set, 0, sizeof ( mcr_MacroSet ) ) ;
	set->dispatch_mods = MCR_ANY_MOD ;
	mcr_Macro_init ( & set->macro ) ;
}

void mcr_MacroSet_free ( mcr_MacroSet * set )
{
	if ( ! set )
	{
		dmsg ;
		return ;
	}
	mcr_Dispatch_remove_all ( & set->macro.hot ) ;
	mcr_Signal_free ( & set->dispatch_specifier ) ;
	mcr_Macro_free ( & set->macro ) ;
}

void mcr_MacroSet_free_foreach ( mcr_MacroSet * set, ... )
{
	if ( ! set )
	{
		dmsg ;
		return ;
	}
	mcr_Dispatch_remove_all ( & set->macro.hot ) ;
	mcr_Signal_free ( & set->dispatch_specifier ) ;
	mcr_Macro_free ( & set->macro ) ;
}

void mcr_macroset_initialize ( )
{
	mcr_StringMap_init ( & _set, sizeof ( mcr_MacroSet ) ) ;
	mcr_StringMap_init ( & _macroOptions, sizeof ( void * ) ) ;

	mcr_defaultOption = mcr_MacroSet_option ;
	mcr_Script_set_option ( mcr_macroOptionName, mcr_MacroSet_option ) ;
}

void mcr_macroset_cleanup ( void )
{
	/* Avoid unmapping everything separately. */
	mcr_Dispatch_clear_all ( ) ;
	MCR_MAP_FOR_EACH_VALUE ( & _set, mcr_MacroSet_free_foreach, 0 ) ;
	MCR_MAP_FOR_EACH ( & _set, mcr_Array_free_foreach, 0 ) ;
	mcr_Map_free ( & _set ) ;
	MCR_MAP_FOR_EACH ( & _macroOptions, mcr_Array_free_foreach, 0 ) ;
	mcr_Map_free ( & _macroOptions ) ;
}
