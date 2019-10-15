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

#include "mcr/util/linux/p_util.h"
#include "mcr/util/util.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

static int _uid = 1000;
void mcr_setuid(int uid)
{
	if (uid) {
		_uid = uid;
	} else {
		dmsg;
	}
}

int mcr_privilege_deactivate()
{
	/* Full user, attempt set lower permission user, */
	/* do not fail on higher permission user. */
	if (getuid() && geteuid()) {
		dassert(_uid);
		if (setuid(_uid) == -1) {
			mcr_errno(-1);
			return mcr_err;
		}
	}
	/* Full root, use _uid in memory, and fail if not able. */
	else if (!getuid() && !geteuid()) {
		dassert(_uid);
		if (setuid(_uid) == -1) {
			mcr_errno(-1);
			return mcr_err;
		}
	}
	/* We have a user, so try to set to that user, fail if not able. */
	else if (setuid(getuid()? getuid() : geteuid()) == -1) {
		mcr_errno(-1);
		return mcr_err;
	}
	dassert(getuid() && geteuid());
	return 0;
}
