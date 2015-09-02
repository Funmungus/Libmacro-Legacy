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
