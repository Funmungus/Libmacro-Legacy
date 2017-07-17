/* Libmacro - A multi-platform, extendable macro and hotkey C library
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

/*!
 * \file
 */

#include "mcr/util/util.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

static int _local_errno = 0;

int mcr_error()
{
	return _local_errno;
}

void mcr_set_error(int errorNumber)
{
	_local_errno = errorNumber < 0 ? -errorNumber : errorNumber;
}

int mcr_set_errno(int defaultErrorNumber, const char *fileString, int line)
{
	_local_errno = errno;
	if (!_local_errno)
		_local_errno = defaultErrorNumber < 0 ? -defaultErrorNumber :
			       defaultErrorNumber;
	ddo(fprintf(stderr, "Error %d: %s(%d) %s: %s.\n", _local_errno,
		    fileString, line, mcr_timestamp(), strerror(_local_errno)));
	return _local_errno;
}
