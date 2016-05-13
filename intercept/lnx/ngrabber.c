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

#include "mcr/intercept/lnx/nintercept.h"
#include "mcr/intercept/intercept.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

void mcr_Grabber_init(void *grabDataPt)
{
	struct mcr_Grabber *grabPt = grabDataPt;
	dassert(grabDataPt);
	memset(grabDataPt, 0, sizeof(struct mcr_Grabber));
	grabPt->fd = -1;
	mcr_String_init(&grabPt->path);
}

void mcr_Grabber_free(void *grabDataPt)
{
	struct mcr_Grabber *grabPt = grabDataPt;
	dassert(grabDataPt);
	if (grabPt->fd != -1) {
		if (ioctl(grabPt->fd, EVIOCGRAB, 0) < 0) {
			mset_error(errno);
		}
		if (close(grabPt->fd) < 0) {
			mset_error(errno);
		}
		grabPt->fd = -1;
	}
	mcr_Array_free(&grabPt->path);
}

const char *mcr_Grabber_path(struct mcr_Grabber *grabPt)
{
	dassert(grabPt);
	if (MCR_STR_IS_EMPTY(grabPt->path))
		return NULL;
	return grabPt->path.array;
}

int mcr_Grabber_set_path(struct mcr_Grabber *grabPt, const char *path)
{
	int err = 0, wasGrabbing = grabPt->is_grabbing,
		wasEnabled = mcr_Grabber_is_enabled(grabPt);
	dassert(grabPt);
	if (wasEnabled) {
		if ((err = mcr_Grabber_set_enabled(grabPt, false)))
			return err;
	}
	if ((err = mcr_String_replace(&grabPt->path, path)))
		return err;
	if (wasEnabled) {
		if ((err = mcr_Grabber_set_enabled(grabPt, true)))
			return err;
		if (wasGrabbing)
			return mcr_Grabber_set_grabbing(grabPt, true);
	}
	return err;
}

bool mcr_Grabber_is_enabled(struct mcr_Grabber * grabPt)
{
	return grabPt ? grabPt->fd != -1 : false;
}

int mcr_Grabber_set_enabled(struct mcr_Grabber *grabPt, bool enable)
{
	int err = 0;
	if ((err = mcr_set_privileged(true)))
		return err;
	if (grabPt->fd != -1) {
		if ((err = mcr_Grabber_set_grabbing(grabPt, false)))
			return err;
		if (close(grabPt->fd) < 0) {
			err = errno;
			if (!err)
				err = EINTR;
			mset_error(err);
			goto onError;
		}
		grabPt->fd = -1;
		/* We are now no longer grabbing. */
	}
	if (!enable)
		return mcr_set_privileged(false);
	if (MCR_STR_IS_EMPTY(grabPt->path)) {
		err = EPERM;
		mset_error(EPERM);
		goto onError;
	}
	if ((grabPt->fd = open(grabPt->path.array, O_RDWR)) == -1) {
		/*| O_NONBLOCK */
		err = errno;
		if (!err)
			err = EINTR;
		mset_error(err);
		goto onError;
	}
//      struct timespec separator = { 0 };
//      separator.tv_nsec = 10 * 1000 * 1000;
//      if (thrd_sleep(&separator, NULL) != thrd_success) {
//              dmsg;
//              ddo(goto onError);
//      }
	return mcr_set_privileged(false);
 onError:
	if (grabPt->fd != -1) {
		if (ioctl(grabPt->fd, EVIOCGRAB, 0) < 0) {
			mset_error(errno);
		}
		if (close(grabPt->fd) < 0) {
			mset_error(errno);
		}
		grabPt->fd = -1;
	}
	mcr_set_privileged(false);
	return err ? err : EINTR;
}

bool mcr_Grabber_is_grabbing(struct mcr_Grabber * grabPt)
{
	return grabPt ? grabPt->is_grabbing : false;
}

int mcr_Grabber_set_grabbing(struct mcr_Grabber *grabPt, bool grab)
{
	int err = 0;
	dassert(grabPt);
	if (grabPt->is_grabbing == grab)
		return 0;
	/* Valgrind false positive error. */
	if (ioctl(grabPt->fd, EVIOCGRAB, grab) < 0) {
		err = errno;
		if (!err)
			err = EINTR;
		mset_error(err);
		return err;
	}
	return err;
}

int mcr_Grabber_state(struct mcr_Grabber *grabPt,
	char *buffer, const size_t size)
{
	int fd = grabPt->fd, err = 0;
	dassert(grabPt);
	dassert(buffer);
	if (fd == -1) {
		if (MCR_STR_IS_EMPTY(grabPt->path)) {
			err = errno;
			if (!err)
				err = EPERM;
			mset_error(err);
			return err;
		}
		if ((err = mcr_set_privileged(true)))
			return err;
		if ((fd = open(grabPt->path.array, O_RDONLY | O_NONBLOCK))
			== -1) {
			err = errno;
			if (!err)
				err = EPERM;
			mset_error(err);
			goto onError;
		}
	}

	if (ioctl(fd, EVIOCGKEY(size), buffer) < 0) {
		err = errno;
		if (!err)
			err = EPERM;
		mset_error(err);
		goto onError;
	}
	/* If grabber was not enabled, we need to return it back. */
	if (grabPt->fd == -1) {
		if (close(fd) < 0) {
			err = errno;
			if (!err)
				err = EPERM;
			mset_error(err);
			goto onError;
		}
		if ((err = mcr_set_privileged(false))) {
			goto onError;
		}
	}
	return err;
 onError:
	if (grabPt->fd == -1) {
		if (close(fd) < 0) {
			mset_error(errno);
		}
		if (mcr_set_privileged(false)) {
			mset_error(errno);
		}
	}
	return err;
}
