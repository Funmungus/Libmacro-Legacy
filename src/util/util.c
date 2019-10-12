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

#include "mcr/util/util.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mcr/util/c11threads.h"

/* thread_local not available for MSVC.  Check new versions? */
#ifdef _MSC_VER
	/* Dll workaround, mcr_err is not dll exported. */
	#undef mcr_err
	extern int mcr_err;
#else
	/* Shared and exported thread_local.  Fast and thread-safe access. */
	MCR_API
	#ifdef __GNUC__
		__thread
	#else
		thread_local
	#endif
#endif
int mcr_err;

int *mcr_err_tls()
{
	return &mcr_err;
}

int mcr_read_err()
{
	int ret = mcr_err;
	if (ret)
		mcr_err = 0;
	return ret;
}

const char *mcr_timestamp()
{
	time_t curtime;
	time(&curtime);
	return ctime(&curtime);
}

size_t mcr_bit_index(uintmax_t bitval)
{
	/* No on bit gives no index information.
	 * If not 0, there will definitely be some bit value contained */
	size_t index = 0;
	if (bitval == 0)
		return (size_t)-1;
	while ((bitval & 0x01) == 0) {
		++index;
		bitval >>= 1;
	}
	return index;
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
		return ETIMEDOUT;
	case thrd_error:
		return EINTR;
	}
	return -1;
}
/*! \todo If thread created but not detached, calling function may free
 * resources currently in use by new thread. */
int mcr_thrd(thrd_start_t func, void *arg)
{
	int thrdErr;
	thrd_t trd;
	mcr_err = 0;
	if ((thrdErr = thrd_create(&trd, func, arg)) == thrd_success &&
		(thrdErr = thrd_detach(trd)) == thrd_success) {
		return 0;
	}
	mcr_err = mcr_thrd_errno(thrdErr);
#ifdef MCR_DEBUG
	dexit;
#else
	return mcr_err;
#endif
}
