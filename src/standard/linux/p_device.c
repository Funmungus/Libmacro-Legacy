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

#include "mcr/standard/linux/p_device.h"
#include "mcr/standard/standard.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/joystick.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

MCR_API struct mcr_Device mcr_genDev;
MCR_API struct mcr_Device mcr_absDev;

const MCR_API struct input_event mcr_syncer = {
	.type = EV_SYN,
	.code = SYN_REPORT
};

MCR_API __s32 mcr_abs_resolution = MCR_ABS_RESOLUTION;

static mcr_String _uinputPath;
static mcr_String _eventPath;
static mtx_t _deviceLock;

/* Open uinput ( devPt->fd ) and write device to it. */
static int device_open(struct mcr_Device *devPt);
/* Open event device ( devPt->eventFd ). */
static int device_open_event(struct mcr_Device *devPt);
/* Destroy uinput device ( devPt->fd ) and close devPt->fd. */
static int device_close(struct mcr_Device *devPt);
/* Close devPt->eventFd. */
static int device_close_event(struct mcr_Device *devPt);
/* Open event device ( devPt->eventFd ) from current working directory. */
static int device_open_event_wd(struct mcr_Device *devPt);
/* Verify devPt->eventFd is correct for given device. */
static bool device_is_event_me(struct mcr_Device *devPt,
							   char *nameBuffer, bool isJoy);
/* Write event to device for all values in bits. */
static int device_write_bits(struct mcr_Device *devPt, int event,
							 struct mcr_Array *bits);
/* Write all UI_SET_EVBIT values for given device. */
static int device_write_evbit(struct mcr_Device *devPt);
/* Write all non-UI_SET_EVBIT values for given device. */
static int device_write_non_evbit(struct mcr_Device *devPt);
/* Write UI_DEV_CREATE after writing all evbits to device. */
static int device_create(struct mcr_Device *devPt);

static int genDevice_init();
static int absDevice_init();

static int init_IntArray(void *data)
{
	if (data) {
		mcr_Array_init(data);
		((struct mcr_Array *)data)->element_size = sizeof(int);
	}
	return 0;
}

/* Initialize and deinit array of integers */
static const struct mcr_Interface _iIntArray = {
	.data_size = sizeof(struct mcr_Array),
	.init = init_IntArray,
	.deinit = mcr_Array_deinit
};

int mcr_Device_set_uinput_path(const char *path)
{
	if (!path || path[0] == '\0')
		return ENODEV;
	return mcr_String_replace(&_uinputPath, path);
}

int mcr_Device_set_event_path(const char *directoryPath)
{
	if (!directoryPath || directoryPath[0] == '\0')
		return ENODEV;
	return mcr_String_replace(&_eventPath, directoryPath);
}

int mcr_Device_set_absolute_resolution(__s32 resolution)
{
	bool wasEnabled = mcr_absDev.enabled;
	mcr_abs_resolution = resolution;
	mcr_Device_deinit(&mcr_absDev);
	if (absDevice_init())
		return mcr_err;
	if (wasEnabled)
		return mcr_Device_enable(&mcr_absDev, true);
	return 0;
}

int mcr_Device_init(void *dataPt)
{
	struct mcr_Device *localPt = dataPt;
	if (localPt) {
		memset(dataPt, 0, sizeof(struct mcr_Device));
		localPt->fd = -1;
		localPt->event_fd = -1;
		mcr_Map_init(&localPt->type_value_map);
		mcr_Map_set_all(&localPt->type_value_map, sizeof(int),
						sizeof(struct mcr_Array), mcr_int_compare, NULL,
						&_iIntArray);
	}
	return 0;
}

int mcr_Device_deinit(void *dataPt)
{
	struct mcr_Device *devPt = dataPt;
	if (devPt) {
		mcr_Device_enable(devPt, false);
		mcr_Map_deinit(&devPt->type_value_map);
		mcr_Device_init(devPt);
	}
	return 0;
}

static inline int mtx_unlock_error(int mtxError)
{
	if (mtxError == thrd_success)
		mtx_unlock(&_deviceLock);
	return mcr_err;
}

int mcr_Device_enable(struct mcr_Device *devPt, bool enable)
{
	int mtxErr = thrd_success;
	dassert(devPt);
	devPt->enabled = false;
	if (device_close_event(devPt))
		return mcr_err;
	/* If already enabled, need to close and restart. */
	if (device_close(devPt))
		return mcr_err;
	/* disable complete */
	if (!enable)
		return 0;
	/* Cannot have device with no evbits to set.
	 * Must have at least 1 UI_SET_EVBIT. */
	if (!mcr_Device_has_evbit(devPt))
		mset_error_return(EPERM);
	if ((mtxErr = mtx_lock(&_deviceLock)) != thrd_success) {
		mtxErr = mcr_thrd_errno(mtxErr);
		mset_error_return(mtxErr);
	}
	mcr_err = 0;
	/* Start by opening. */
	if (device_open(devPt))
		return mtx_unlock_error(mtxErr);
	/* Force evbit satisfaction. */
	if (device_write_evbit(devPt))
		return mtx_unlock_error(mtxErr);
	/* Then write all other bits. Ensured at least one mapping pair. */
	if (device_write_non_evbit(devPt))
		return mtx_unlock_error(mtxErr);
	if (device_create(devPt))
		return mtx_unlock_error(mtxErr);
	/* Created and ready. valid true is all good, false means some */
	/* non-UI_SET_EVBIT did not work. */
	devPt->enabled = true;
	device_open_event(devPt);
	if (mtxErr == thrd_success)
		mtx_unlock(&_deviceLock);
	return mcr_err;
}

int mcr_Device_enable_all(bool enable)
{
	if (mcr_Device_enable(&mcr_genDev, enable) ||
		mcr_Device_enable(&mcr_absDev, enable))
		return mcr_err;
	return 0;
}

int mcr_Device_set_bits(struct mcr_Device *devPt, int bitType, int *bits,
						size_t bitLen)
{
	struct mcr_Array *element =
		mcr_Map_element_ensured(&devPt->type_value_map,
								&bitType);
	if (element)
		element = mcr_Map_valueof(&devPt->type_value_map, element);
	dassert(devPt);
	dassert(bits);
	dassert(bitLen != (size_t) ~ 0);
	return element ? mcr_Array_replace(element, bits, bitLen) : mcr_err;
}

struct mcr_Array *mcr_Device_bits(struct mcr_Device *devPt, int bitType)
{
	void *ret = MCR_MAP_ELEMENT(devPt->type_value_map, &bitType);
	dassert(devPt);
	return MCR_MAP_VALUEOF(devPt->type_value_map, ret);
}

bool mcr_Device_has_evbit(struct mcr_Device * devPt)
{
	int evbit = UI_SET_EVBIT;
	struct mcr_Array *evbit_arr = MCR_MAP_ELEMENT(devPt->type_value_map,
								  &evbit);
	dassert(devPt);
	evbit_arr = MCR_MAP_VALUEOF(devPt->type_value_map, evbit_arr);
	return evbit_arr && evbit_arr->used;
}

static int device_open(struct mcr_Device *devPt)
{
	dassert(devPt);
	/* Close in case previously opened. */
	if (device_close(devPt))
		return mcr_err;
	devPt->fd = open(_uinputPath.array, O_WRONLY | O_SYNC);
	if (devPt->fd == -1) {
		mcr_errno(EINTR);
		return mcr_err;
	}
	if (write(devPt->fd, &devPt->device, sizeof(devPt->device)) !=
		sizeof(devPt->device)) {
		mcr_errno(EINTR);
		device_close(devPt);
		return mcr_err;
	}
	return 0;
}

static int device_open_event(struct mcr_Device *devPt)
{
	long int size;
	char *wd;
	char *ptr;
	dassert(devPt);
	if (device_close_event(devPt))
		return mcr_err;
	size = pathconf(".", _PC_PATH_MAX);
	if (size == -1) {
		mcr_errno(EINTR);
		return mcr_err;
	}
	wd = malloc(size);
	if (!wd) {
		mset_error_return(ENOMEM);
	}
	mcr_err = 0;
	/* Get the current working directory to return to later. */
	ptr = getcwd(wd, size);
	UNUSED(ptr);
	if (chdir(_eventPath.array) == -1) {
		mcr_errno(EINTR);
		free(wd);
		return mcr_err;
	}

	if (device_open_event_wd(devPt) || chdir(wd) == -1) {
		mcr_errno(EINTR);
		free(wd);
		return mcr_err;
	}
	free(wd);
	return mcr_err;
}

static int device_close(struct mcr_Device *devPt)
{
	dassert(devPt);
	if (devPt->fd != -1) {
		if (ioctl(devPt->fd, UI_DEV_DESTROY) < 0
			|| close(devPt->fd) < 0) {
			mcr_errno(EINTR);
			return mcr_err;
		} else {
			devPt->fd = -1;
		}
	}
	return 0;
}

static int device_close_event(struct mcr_Device *devPt)
{
	dassert(devPt);
	if (devPt->event_fd != -1) {
		if (close(devPt->event_fd) < 0) {
			mcr_errno(EINTR);
			return mcr_err;
		} else {
			devPt->event_fd = -1;
		}
	}
	return 0;
}

static int device_open_event_wd(struct mcr_Device *devPt)
{
	DIR *dirp = opendir(".");
	struct dirent *entry;
	bool isdev;
	char dev_name[UINPUT_MAX_NAME_SIZE];
	struct stat s;
	dassert(devPt);
	if (dirp == NULL) {
		mcr_errno(EINTR);
		return mcr_err;
	}
	mcr_err = 0;

	memset(dev_name, 0, sizeof(dev_name));
	/* if !entry then end of directory */
	while ((entry = readdir(dirp))) {
		/* Ignore files without access, and do not fail with lack
		 * of permissions */
		if (!access(entry->d_name, R_OK)) {
			if (stat(entry->d_name, &s) < 0) {
				mset_error(errno);
				continue;
			}
			/* Our uinput devices are always char devices. */
			if (S_ISCHR(s.st_mode)) {
				devPt->event_fd = open(entry->d_name, O_RDONLY);
				if (devPt->event_fd == -1) {
					mset_error(errno);
					continue;
				}

				isdev = !strncasecmp(entry->d_name, "js", 2) ||
						!strncasecmp(entry->d_name, "joystick", 8);
				/* Event fd is set.  If it is correct, finish out. */
				if ((isdev = device_is_event_me(devPt, dev_name,
												isdev))) {
					mcr_err = 0;
					break;
				}
				/* Incorrect dev, continue */
				if (device_close_event(devPt))
					break;
			}
		}
	}
	closedir(dirp);
	return mcr_err;
}

static bool device_is_event_me(struct mcr_Device *devPt,
							   char *nameBuffer, bool isJoy)
{
	int comparison = 0, err = 0;
	dassert(devPt);
	/* joysticks have different ioctl for names. */
	if (isJoy) {
		comparison =
			ioctl(devPt->event_fd, JSIOCGNAME(UINPUT_MAX_NAME_SIZE),
				  nameBuffer);
	} else {
		comparison =
			ioctl(devPt->event_fd, EVIOCGNAME(UINPUT_MAX_NAME_SIZE),
				  nameBuffer);
	}
	/* Could not retrieve name, unknown error. */
	if (comparison < 0) {
		if ((err = device_close_event(devPt)))
			return err;
		return false;
	}
	return !strncmp(nameBuffer, devPt->device.name, UINPUT_MAX_NAME_SIZE);
}

static int device_write_bits(struct mcr_Device *devPt, int setBit,
							 struct mcr_Array *bits)
{
	int fd = devPt->fd;
	char *itPt, *end;
	size_t bytes;
	dassert(devPt);
	dassert(bits);
	if (!bits->used)
		return 0;
	mcr_Array_iter(bits, &itPt, &end, &bytes);
	while (itPt < end) {
		if (ioctl(fd, setBit, *(int *)itPt) < 0) {
			mcr_errno(EINTR);
			return mcr_err;
		}
		itPt += bytes;
	}
	return 0;
}

static int device_write_evbit(struct mcr_Device *devPt)
{
	int evbit = UI_SET_EVBIT;
	struct mcr_Array *evbit_arr = MCR_MAP_ELEMENT(devPt->type_value_map,
								  &evbit);
	dassert(devPt);
	evbit_arr = MCR_MAP_VALUEOF(devPt->type_value_map, evbit_arr);
	return evbit_arr
		   && evbit_arr->used ? device_write_bits(devPt, evbit,
				   evbit_arr) : EINVAL;
}

static int device_write_non_evbit(struct mcr_Device *devPt)
{
	dassert(devPt);
	int evBit;
	struct mcr_Array *evbitArr;
	char *itPt, *end;
	size_t bytes;
	/* All non-evbits. */
	if (!devPt->type_value_map.set.used)
		mset_error_return(EINVAL);
	mcr_Map_iter(&devPt->type_value_map, &itPt, &end, &bytes);
	while (itPt < end) {
		evBit = *(int *)itPt;
		if (evBit != UI_SET_EVBIT) {
			evbitArr = MCR_MAP_VALUEOF(devPt->type_value_map, itPt);
			if (device_write_bits(devPt, evBit, evbitArr))
				return mcr_err;
		}
		itPt += bytes;
	}
	return 0;
}

static int device_create(struct mcr_Device *devPt)
{
	dassert(devPt);
	if (ioctl(devPt->fd, UI_DEV_CREATE) < 0) {
		mcr_errno(EINTR);
		return mcr_err;
	}
	return 0;
}

static inline int gen_dev_set(int arr[], size_t count, int uibit)
{
	return mcr_Device_set_bits(&mcr_genDev, uibit, arr, count);
}

static int genDevice_init()
{
	int i, err = 0;
	int evbits[KEY_CNT];
	int uibits[] = {
		EV_SYN, EV_KEY, EV_REL, EV_MSC, EV_SND, EV_REP
	};
	/* Does not handle abs, sw, led, ff, ff_status */
	for (i = 0; i < KEY_CNT; i++) {
		evbits[i] = i;
	}

	mcr_Device_init(&mcr_genDev);

	snprintf(mcr_genDev.device.name, UINPUT_MAX_NAME_SIZE, "libmacro-gen");

	mcr_genDev.device.id.bustype = BUS_VIRTUAL;
	mcr_genDev.device.id.vendor = 1;
	mcr_genDev.device.id.product = 1;
	mcr_genDev.device.id.version = 1;

	if ((err = gen_dev_set(uibits, arrlen(uibits), UI_SET_EVBIT)))
		return err;
	/* Do not write 0 value */
	if ((err = gen_dev_set(evbits + 1, KEY_CNT - 1, UI_SET_KEYBIT)))
		return err;
	if ((err = gen_dev_set(evbits, REL_CNT, UI_SET_RELBIT)))
		return err;
	if ((err = gen_dev_set(evbits, MSC_CNT, UI_SET_MSCBIT)))
		return err;
	if ((err = gen_dev_set(evbits, SND_CNT, UI_SET_SNDBIT)))
		return err;
	/* On-screen pointer and mapped directly to screen coordinates */
	evbits[0] = INPUT_PROP_POINTER;
	evbits[1] = INPUT_PROP_DIRECT;
	return gen_dev_set(evbits, 2, UI_SET_PROPBIT);
}

static int absDevice_init()
{
	int i, err = 0;
	int evbits[] = { EV_SYN, EV_KEY, EV_ABS };
	int absbits[ABS_MISC + 1];
	mcr_Device_init(&mcr_absDev);

	snprintf(mcr_absDev.device.name, UINPUT_MAX_NAME_SIZE, "libmacro-abs");

	mcr_absDev.device.id.bustype = BUS_VIRTUAL;
	mcr_absDev.device.id.vendor = 1;
	mcr_absDev.device.id.product = 1;
	mcr_absDev.device.id.version = 1;

	for (i = 0; i <= ABS_MISC; i++) {
		mcr_absDev.device.absmax[i] = mcr_abs_resolution;
	}

	if ((err = mcr_Device_set_bits(&mcr_absDev, UI_SET_EVBIT, evbits, 3)))
		return err;
	/* Not used, but required to be set up correctly */
	evbits[0] = BTN_LEFT;
	if ((err = mcr_Device_set_bits(&mcr_absDev, UI_SET_KEYBIT, evbits, 1)))
		return err;
	/* On-screen pointer and mapped directly to screen coordinates */
	evbits[0] = INPUT_PROP_POINTER;
	evbits[1] = INPUT_PROP_DIRECT;
	if ((err = mcr_Device_set_bits(&mcr_absDev, UI_SET_PROPBIT, evbits, 2)))
		return err;
	for (i = 0; i <= ABS_MISC; i++) {
		absbits[i] = i;
	}

	if ((err = mcr_Device_set_bits(&mcr_absDev, UI_SET_ABSBIT, absbits,
								   ABS_MISC + 1))) {
		return err;
	}
	return err;
}

int mcr_Device_initialize(struct mcr_context *context)
{
	int mtxErr = mtx_init(&_deviceLock, mtx_plain);
	UNUSED(context);
	if (mtxErr != thrd_success) {
		mtxErr = mcr_thrd_errno(mtxErr);
		mset_error_return(mtxErr);
	}
	mcr_String_init(&_uinputPath);
	mcr_String_init(&_eventPath);
	if (mcr_String_replace(&_uinputPath, MCR_STR(MCR_UINPUT_PATH)))
		return mcr_err;
	if (mcr_String_replace(&_eventPath, MCR_STR(MCR_EVENT_PATH)))
		return mcr_err;
	/* ioctl is unpredictable for valgrind. The first ioctl will
	 * be uninitialized, and others should not cause errors. */
	if (genDevice_init())
		return mcr_err;
	if (absDevice_init())
		return mcr_err;
	if (!access(MCR_STR(MCR_UINPUT_PATH), W_OK | R_OK)) {
		if (mcr_Device_enable_all(true))
			return mcr_err;
	}
	return 0;
}

int mcr_Device_deinitialize(struct mcr_context *context)
{
	UNUSED(context);
	mcr_Device_deinit(&mcr_genDev);
	mcr_Device_deinit(&mcr_absDev);
	mcr_Array_deinit(&_uinputPath);
	mcr_Array_deinit(&_eventPath);
	mtx_destroy(&_deviceLock);
	return 0;
}
