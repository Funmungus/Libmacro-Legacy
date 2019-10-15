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

#include "mcr/intercept/linux/p_intercept.h"
#include "mcr/intercept/intercept.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

static int grabber_open(struct mcr_Grabber *grabPt);
static int grabber_close(struct mcr_Grabber *grabPt);
static int grabber_grab(struct mcr_Grabber *grabPt, bool toGrab);

int mcr_Grabber_init(void *grabPt)
{
	struct mcr_Grabber *localPt = grabPt;
	if (localPt) {
		memset(grabPt, 0, sizeof(struct mcr_Grabber));
		localPt->fd = -1;
		mcr_String_init(&localPt->path);
	}
	return 0;
}

int mcr_Grabber_deinit(void *grabPt)
{
	int err = 0;
	struct mcr_Grabber *localPt = grabPt;
	if (localPt) {
		err = mcr_Grabber_set_enabled(localPt, false);
		mcr_String_deinit(&localPt->path);
	}
	return err;
}

bool mcr_Grabber_is_blocking(struct mcr_Grabber *grabPt)
{
	return grabPt && grabPt->blocking;
}

int mcr_Grabber_set_blocking(struct mcr_Grabber *grabPt, bool enable)
{
	if (!grabPt || enable == grabPt->blocking)
		return 0;
	/* Currently running */
	if (grabPt->fd != -1) {
		if (grabber_grab(grabPt, enable))
			return mcr_err;
	}
	grabPt->blocking = enable;
	return 0;
}

const char *mcr_Grabber_path(struct mcr_Grabber *grabPt)
{
	return grabPt ? grabPt->path.array : NULL;
}

int mcr_Grabber_set_path(struct mcr_Grabber *grabPt, const char *path)
{
	bool wasEnabled = mcr_Grabber_is_enabled(grabPt);
	dassert(grabPt);
	if (wasEnabled) {
		if (mcr_Grabber_set_enabled(grabPt, false))
			return mcr_err;
	}
	if (mcr_String_replace(&grabPt->path, path))
		return mcr_err;
	if (wasEnabled) {
		if (mcr_Grabber_set_enabled(grabPt, true))
			return mcr_err;
	}
	return 0;
}

bool mcr_Grabber_is_enabled(struct mcr_Grabber * grabPt)
{
	return grabPt ? grabPt->fd != -1 : false;
}

int mcr_Grabber_set_enabled(struct mcr_Grabber *grabPt, bool enable)
{
	dassert(grabPt);
	if (enable != mcr_Grabber_is_enabled(grabPt))
		return enable ? grabber_open(grabPt) : grabber_close(grabPt);
	return 0;
}

static int grabber_open(struct mcr_Grabber *grabPt)
{
	if (grabPt->fd != -1 || MCR_STR_IS_EMPTY(grabPt->path))
		mset_error_return(ENODEV);
	if (access(grabPt->path.array, R_OK)) {
		mcr_errno(ENODEV);
		return mcr_err;
	}
	if ((grabPt->fd =
			 open(grabPt->path.array,
				  O_RDONLY | O_NONBLOCK)) == -1) {
		mcr_errno(EINTR);
		return mcr_err;
	}
	if (grabPt->blocking)
		return grabber_grab(grabPt, true);
	return 0;
}

static int grabber_close(struct mcr_Grabber *grabPt)
{
	int err;
	if (grabPt->fd == -1)
		return 0;
	if (grabPt->blocking && (err = grabber_grab(grabPt, false))) {
		close(grabPt->fd);
		grabPt->fd = -1;
		return err;
	}
	mcr_err = 0;
	if (close(grabPt->fd) < 0) {
		mcr_errno(EINTR);
	}
	grabPt->fd = -1;
	return mcr_err;
}

static int grabber_grab(struct mcr_Grabber *grabPt, bool toGrab)
{
	if (ioctl(grabPt->fd, EVIOCGRAB, toGrab ? 1 : 0) < 0) {
		mcr_errno(EINTR);
		return mcr_err;
	}
	return 0;
}
