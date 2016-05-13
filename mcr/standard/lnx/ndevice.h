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

/*! \file
 * \brief Currently Linux-specific. mcr_Device is a wrapper for
 * creating uinput devices.
 *
 * The source file contains predefined devices for keyboard,
 * absolute, and relative.
 */

#ifndef MCR_LNX_DEVICE_H
#define MCR_LNX_DEVICE_H

#include "mcr/standard/lnx/ndef.h"

/*! \brief Wrapper for uinput devices. */
struct mcr_Device {
	/*! \brief Create a device and write to it */
	struct uinput_user_dev device;
	/*! \brief int->mcr_Array, input type to input bit array */
	struct mcr_Map type_value_map;
	/*! \brief File descriptor to write input_events */
	int fd;
	/*! \brief File descriptor for event node, to ioctl and read from */
	int event_fd;
	/*! \brief 0 disabled, otherwise can write events to fd */
	bool enabled;
};

/* Non-static to inline write function. */
/*! \brief To send non-absolute events. */
extern struct mcr_Device mcr_genDev;
/*! \brief To send MoveCursor unjustified */
extern struct mcr_Device mcr_absDev;
/*! \brief Append similar input_event to the end of all sending of events. */
extern const struct input_event mcr_syncer;

extern __s32 mcr_abs_resolution;

MCR_API int mcr_Device_set_uinput_path(const char *path);
MCR_API int mcr_Device_set_event_path(const char *directoryPath);
MCR_API int mcr_Device_set_absolute_resolution(__s32 resolution);
/*! ctor, Initialize empty device, map, and files.
 */
MCR_API void mcr_Device_init(void *dataPt);
/*! Free all mapped arrays, and free the map itself.
 */
MCR_API void mcr_Device_free(void *dataPt);
/*!
 * \brief Start or end user device. This will modify fd, event_fd,
 * and enabled state.
 *
 * \param enable int 0 disable, any other value to enable
 * \return int 0 on failure, otherwise successful
 */
MCR_API int mcr_Device_enable(struct mcr_Device *devPt, bool enable);
MCR_API int mcr_Device_enable_all(bool enable);

/*!
 * \brief Set input bit values for a single bit type.
 *
 * \param bitType int Type of event to set values for
 * \param bits int * Set of all values to set for bitType
 * \param bitLen size_t Length of bits
 * \return 0 on failure, otherwise successful
 */
MCR_API int mcr_Device_set_bits(struct mcr_Device *devPt, int bitType,
	int *bits, size_t bitLen);
/*!
 * \brief Get all input bits to be set.
 *
 * \param bitType int Type of event to find values for
 * \return The set of all input bits to be set, or
 * NULL if not found.
 */
MCR_API struct mcr_Array *mcr_Device_bits(struct mcr_Device *devPt,
	int bitType);
/*! \brief \return Non-zero if device has evbits to write
 */
MCR_API int mcr_Device_has_evbit(struct mcr_Device *devPt);

/* Sending, macro inlined for efficiency. */
/*!
 * \brief Send input_events to given device.
 *
 * \param dev \ref mcr_Device
 * \param eventObjects input_event * Pointer to or array of input_events
 * \param size size_t Byte size of all input_events in eventObjects
 * \return \ref reterr
 */
#define MCR_DEV_SEND(dev, eventObjects, size) \
((dev).fd == -1 ? \
	write((dev).fd, eventObjects, size) : \
-1)

/*!
 * \brief \ref MCR_DEV_SEND for single input_event.
 *
 * \param dev mcr_Device
 * \param eventObject input_event * Pointer to single event object
 * \return int
 */
#define MCR_DEV_SEND_ONE(dev, eventObject, success) \
MCR_DEV_SEND(dev, eventObject, sizeof (struct input_event))

/*! \brief Send a syncing event to this device. */
#define MCR_DEV_SYNC(dev, success) \
MCR_DEV_SEND_ONE(dev, &mcr_syncer)

/*!
 * \brief Initialize values for known mcr_Device objects,
 * and allocate resources.
 */
MCR_API int mcr_Device_initialize(struct mcr_context *context);
/*! Dealocate resources for linux devices. */
MCR_API void mcr_Device_cleanup(struct mcr_context *context);

/* TODO: dynamic path. */

#endif
