/* Libmacro - A multi-platform, extendable macro and hotkey C library.
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

#include "mcr/extras/extras.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MCR_API const char *mcr_macroOptionName = "macro";

static struct mcr_Map _set;
static struct mcr_Map _macroOptions;

int mcr_MacroSet_option(int argc, char **argv, int index)
{
	dassert(argv);
	dassert(index < argc);

	/* Need at least one additional function to do on macro. */
	MCR_OPTION_MIN_OPTS(argc, argv, index, 1);
	mcr_macro_option_fnc *fPt = mcr_StringMap_value
		(&_macroOptions, argv[index + 1]);
	if (!fPt) {
		dmsg;
		return index;	/* No further input processed. */
	}
	struct mcr_Array *found = mcr_StringMap_element(&_set, argv[index]);
	struct mcr_MacroSet *mcrFound = MCR_MAP_ELEMENT(_set, found);
	mcrFound = MCR_MAP_VALUEOF(_set, mcrFound);
	if (!found) {
		if (!mcr_StringMap_map(&_set, argv[index], NULL)) {
			dmsg;
			return index + 1;
		}
		found = mcr_Map_element(&_set, argv + index);
		mcrFound = MCR_MAP_ELEMENT(_set, found);
		mcrFound = MCR_MAP_VALUEOF(_set, mcrFound);
		dassert(found);
		dassert(mcrFound);
		mcr_MacroSet_init(mcrFound);
	}
	return (*fPt) (argc, argv, index + 1, found, mcrFound);
}

void mcr_MacroSet_init(struct mcr_MacroSet *set)
{
	if (!set) {
		dmsg;
		return;
	}
	memset(set, 0, sizeof(struct mcr_MacroSet));
	set->dispatch_mods = MCR_MOD_ANY;
	mcr_Macro_set_all(&set->macro, false, true, 1, false, NULL);
}

void mcr_MacroSet_free(struct mcr_MacroSet *set)
{
	if (!set) {
		dmsg;
		return;
	}
	mcr_Dispatcher_remove_all(NULL, &set->macro.trigger);
	mcr_Signal_free(&set->dispatch_specifier);
	mcr_Macro_free(&set->macro);
}

void mcr_MacroSet_free_foreach(struct mcr_MacroSet *set, ...)
{
	if (!set) {
		dmsg;
		return;
	}
	mcr_Dispatcher_remove_all(NULL, &set->macro.trigger);
	mcr_Signal_free(&set->dispatch_specifier);
	mcr_Macro_free(&set->macro);
}

void mcr_macro_set_initialize()
{
	mcr_StringMap_init(&_set);
	mcr_StringMap_set_all(&_set, sizeof(struct mcr_MacroSet), NULL);
	mcr_StringMap_init(&_macroOptions);
	mcr_StringMap_set_all(&_macroOptions, sizeof(void *), NULL);

	mcr_defaultOption = mcr_MacroSet_option;
	mcr_Script_set_option(mcr_macroOptionName, mcr_MacroSet_option);
}

void mcr_macro_set_cleanup(void)
{
	/* Avoid unmapping everything separately. */
	mcr_Dispatcher_clear_all(NULL);
	MCR_MAP_FOR_EACH_VALUE(_set, mcr_MacroSet_free_foreach, 0);
	MCR_MAP_FOR_EACH(_set, MCR_EXP(mcr_Array_free_foreach),);
	mcr_Map_free(&_set);
	MCR_MAP_FOR_EACH(_macroOptions, MCR_EXP(mcr_Array_free_foreach),);
	mcr_Map_free(&_macroOptions);
}
