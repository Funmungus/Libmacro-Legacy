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

#include "mcr/intercept/lnx/nintercept.h"
#include "mcr/intercept/intercept.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

static int grabber_open(struct mcr_Grabber *grabPt);
static int grabber_close(struct mcr_Grabber *grabPt);
static int grabber_grab(struct mcr_Grabber *grabPt, bool toGrab);

void mcr_Grabber_init(void *grabDataPt)
{
	struct mcr_Grabber *grabPt = grabDataPt;
	dassert(grabDataPt);
	memset(grabDataPt, 0, sizeof(struct mcr_Grabber));
	grabPt->fd = -1;
	mcr_String_init(&grabPt->path);
}

void mcr_Grabber_deinit(void *grabDataPt)
{
	struct mcr_Grabber *grabPt = grabDataPt;
	dassert(grabDataPt);
	mcr_Grabber_set_enabled(grabPt, false);
	mcr_Array_deinit(&grabPt->path);
}

const char *mcr_Grabber_path(struct mcr_Grabber *grabPt)
{
	dassert(grabPt);
	return grabPt->path.array;
}

int mcr_Grabber_set_path(struct mcr_Grabber *grabPt, const char *path)
{
	int err = 0, wasEnabled = mcr_Grabber_is_enabled(grabPt);
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
	dassert(grabPt);
	if (enable != mcr_Grabber_is_enabled(grabPt)) {
		/* No permission do not continue */
		if ((err = mcr_set_privileged(true)))
			return err;

		if (enable)
			err = grabber_open(grabPt);
		else
			err = grabber_close(grabPt);

		/* If we have an error, keep.  Otherwise get new err code,
		 * which is most likely success at this point */
		if (err)
			mcr_set_privileged(false);
		else
			err = mcr_set_privileged(false);
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

static int grabber_open(struct mcr_Grabber *grabPt)
{
	int err;
	if (grabPt->fd != -1 || MCR_STR_IS_EMPTY(grabPt->path)) {
		mset_error(EPERM);
		return EPERM;
	}
	if ((grabPt->fd =
			open(grabPt->path.array,
				O_RDONLY | O_NONBLOCK)) == -1) {
		/*| O_NONBLOCK */
		err = errno;
		if (!err)
			err = EINTR;
		mset_error(err);
		return err;
	}
	return grabber_grab(grabPt, true);
}

static int grabber_close(struct mcr_Grabber *grabPt)
{
	int err;
	if (grabPt->fd == -1)
		return 0;
	if ((err = grabber_grab(grabPt, false))) {
		close(grabPt->fd);
		grabPt->fd = -1;
		return err;
	}
	if (close(grabPt->fd) < 0) {
		err = errno;
		if (!err)
			err = EINTR;
		mset_error(err);
	}
	grabPt->fd = -1;
	return err;
}

static int grabber_grab(struct mcr_Grabber *grabPt, bool toGrab)
{
	int err = 0;
	if (ioctl(grabPt->fd, EVIOCGRAB, toGrab) < 0) {
		err = errno;
		if (!err)
			err = EINTR;
		mset_error(err);
	}
	return err;
}
