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

#ifndef MCR_SCRIPT_H
#define MCR_SCRIPT_H

#include "mcr/extras/safe_string.h"

typedef struct mcr_Script {
	struct mcr_Array scripts;
} mcr_Script;

typedef int (*mcr_option_fnc) (int argc, char **argv, int index);

MCR_API extern struct mcr_ISignal mcr_iScript;
MCR_API extern const char *mcr_helpOptionName;
MCR_API extern mcr_option_fnc mcr_defaultOption;

MCR_API void mcr_Script_init(void *scriptPt);
MCR_API void mcr_Script_free(void *scriptPt);
MCR_API void mcr_Script_copy(void *dstPt, void *srcPt);
MCR_API int mcr_Script_compare(const void *lhs, const void *rhs);
MCR_API int mcr_Script_send(struct mcr_Signal *sigPt);

MCR_API void mcr_Script_option(int argc, char **argv);
MCR_API int mcr_Script_do_option(int argc, char **argv, int index);
MCR_API void mcr_Script_set_option(const char *key, mcr_option_fnc optFnc);
MCR_API int mcr_Script_short_help(int argc, char **argv, int index);
MCR_API void mcr_Script_parse_error();

MCR_API void mcr_script_initialize();
MCR_API void mcr_script_cleanup();

#define MCR_OPTION_MIN_OPTS( argc, argv, index, minOpts ) \
if ( index + minOpts >= argc ) \
{ \
	mcr_dmsg; \
	fprintf ( stderr, "Not enough options.\n" ); \
	mcr_Script_parse_error (); \
	return index; \
}

#endif
