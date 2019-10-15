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

/*! \file
 *  \brief Currently Linux-specific. mcr_Device is a wrapper for
 *  creating uinput devices.
 *
 *  The source file contains predefined devices for keyboard,
 *  absolute, and relative.
 */

#ifndef MCR_STANDARD_LNX_NDEVICE_H_
#define MCR_STANDARD_LNX_NDEVICE_H_

#include "mcr/standard/linux/p_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Linux - Wrapper for uinput devices. */
struct mcr_Device {
	/*! Create a device and write to it */
	struct uinput_user_dev device;
	/*! int => mcr_Array, input type to input bit array */
	struct mcr_Map type_value_map;
	/*! File descriptor to write input_events */
	int fd;
	/*! File descriptor for event node, to ioctl and read from */
	int event_fd;
	/*! false disabled, otherwise can write events to fd */
	bool enabled;
};

/* Non-static to inline write function. */
/*! Linux - To send non-absolute events. */
extern MCR_API struct mcr_Device mcr_genDev;
/*! Linux - To send MoveCursor unjustified */
extern MCR_API struct mcr_Device mcr_absDev;
/*! Linux - Append similar input_event to the end of all sending of events. */
extern MCR_API const struct input_event mcr_syncer;
/*! Linux - Resolution of absolute devices */
extern MCR_API __s32 mcr_abs_resolution;

/*! Linux - Set the uinput file to read and write uinput devices
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Device_set_uinput_path(const char *path);
/*! Linux - Set the directory event files are located in
 *
 *  Event files are used to read device state
 *  \return \ref reterr
 */
MCR_API int mcr_Device_set_event_path(const char *directoryPath);
/*! Linux - Set absolute device resolution
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Device_set_absolute_resolution(__s32 resolution);
/*! \ref mcr_Device ctor
 *
 *  \param devPt \ref opt \ref mcr_Device *
 *  \return \ref reterr
 */
MCR_API int mcr_Device_init(void *devPt);
/*! \ref mcr_Device dtor
 *
 *  \param devPt \ref opt \ref mcr_Device *
 *  \return \ref reterr
 */
MCR_API int mcr_Device_deinit(void *devPt);
/*! Start or end user device. This will modify fd, event_fd,
 *  and enabled state.
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Device_enable(struct mcr_Device *devPt, bool enable);
/*! \ref mcr_Device_enable for all devices
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Device_enable_all(bool enable);

/*! Set input bit values for a single bit type.
 *
 *  \param bitType Type of event to set values for
 *  \param bits Set of all values to set for bitType
 *  \param bitLen Length of bits
 *  \return \ref reterr
 */
MCR_API int mcr_Device_set_bits(struct mcr_Device *devPt, int bitType,
								int *bits, size_t bitLen);
/*! Get all input bits to be set.
 *
 *  \param bitType Type of event to find values for
 *  \return The set of all input bits to be set, or
 *  null if not found.
 */
MCR_API struct mcr_Array *mcr_Device_bits(struct mcr_Device *devPt,
		int bitType);
/*! Does the device have UI_SET_EVBIT? */
MCR_API bool mcr_Device_has_evbit(struct mcr_Device *devPt);

/* Sending, macro inlined for efficiency. */
/*! Send input_events to given device.
 *
 *  \param dev \ref mcr_Device
 *  \param eventObjects input_event * Pointer to or array of input_events
 *  \param size size_t Byte size of all input_events in eventObjects
 *  \return \ref reterr
 */
#define MCR_DEV_SEND(dev, eventObjects, size) \
((dev).fd == -1 ? \
	ENXIO : \
	write((dev).fd, eventObjects, size))

/*! \ref MCR_DEV_SEND for single input_event.
 *
 *  \param dev \ref mcr_Device
 *  \param eventObject input_event * Pointer to single event object
 *  \return \ref reterr
 */
#define MCR_DEV_SEND_ONE(dev, eventObject, success) \
MCR_DEV_SEND(dev, eventObject, sizeof (struct input_event))

/*! Send a syncing event to this device. */
#define MCR_DEV_SYNC(dev, success) \
MCR_DEV_SEND_ONE(dev, &mcr_syncer)

/*! Initialize values for known mcr_Device objects,
 *  and allocate resources.
 */
MCR_API int mcr_Device_initialize(struct mcr_context *context);
/*! Dealocate resources for linux devices. */
MCR_API int mcr_Device_deinitialize(struct mcr_context *context);

/// \todo dynamic path.

#ifdef __cplusplus
}
#endif

#endif
