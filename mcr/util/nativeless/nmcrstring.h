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

#ifndef MCR_NATIVELESS_STRINGS_H
#define MCR_NATIVELESS_STRINGS_H

#include "mcr/util/nativeless/ndef.h"

/* case functions implemented in util/nativeless/mcrstring.c */
MCR_API int strcasecmp(const char *s1, const char *s2);
MCR_API int strncasecmp(const char *s1, const char *s2, size_t n);
MCR_API int stricmp(const char *lhs, const char *rhs);
MCR_API int strnicmp(const char *lhs, const char *rhs, size_t count);

#endif
