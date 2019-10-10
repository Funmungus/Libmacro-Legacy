/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

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

#include "mcr/util/none/p_util.h"
#include "mcr/util/util.h"

int mcr_casecmp(const char *s1, const char *s2)
{
	while (*s1 != '\0' && *s2 != '\0') {
		if (tolower(*s1) != tolower(*s2))
			return (*s1 < *s2) ? -1 : 1;
		++s1;
		++s2;
	}
	/* At least one of the two is now at null-termination. */
	/* If both, they are equal. */
	if (*s1 == *s2)
		return 0;
	/* Confirmed one is at null-termination, and the other is not. */
	/* Whichever is NULL is the lesser. */
	return (*s1 == '\0') ? -1 : 1;
}

int mcr_ncasecmp(const char *s1, const char *s2, size_t n)
{
	size_t i = 0;
	while (s1[i] != '\0' && s2[i] != '\0' && i < n) {
		if (tolower(s1[i]) != tolower(s2[i]))
			return (s1[i] < s2[i]) ? -1 : 1;
		++i;
	}
	/* Either null found, or count finished. If both null, */
	/* or count finished they are equal. */
	if (i == n || s1[i] == s2[i])
		return 0;
	/* One is lesser than the other. */
	return (s1[i] == '\0') ? -1 : 1;
}
