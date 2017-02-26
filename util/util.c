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

#include "mcr/util/util.h"
#include "mcr/util/c11threads.h"
#include <errno.h>

unsigned int mcr_bit_index(unsigned int bitval)
{
	if (bitval == 0)
		return -1;	/* No on bit gives no index information.
				   If not 0, there will definitely be some bit value contained */
	unsigned int index = 0;
	while ((bitval & 0x01) == 0) {
		++index;
		bitval >>= 1;
	}
	return index;
}

const char *mcr_timestamp()
{
	time_t curtime;
	time(&curtime);
	return ctime(&curtime);
}

int mcr_thrd_errno(int thrdError)
{
	switch (thrdError) {
	case thrd_success:
		return 0;
	case thrd_nomem:
		return ENOMEM;
	case thrd_busy:
		return EBUSY;
	case thrd_timedout:
	case thrd_error:
		break;
	}
	return EINTR;
}
