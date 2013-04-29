/*! \file signal/lnx/device.h
 * \brief Currently Linux-specific. mcr_Device is a wrapper for
 * creating uinput devices.
 *
 * The source file contains predefined devices for keyboard,
 * absolute, and relative.
 * */

# ifndef MCR_LNX_DEVICE_H
# define MCR_LNX_DEVICE_H

# include "signal/def.h"
# include "signal/lnx/def.h"

/*! \brief Wrapper for uinput devices.
 *
 * device - uinput_user_dev Used to create device and write to it. <br>
 * type_value_map - Map int->mcr_Array, input type to input bit. <br>
 * fd - File descriptor to write input_events. <br>
 * event_fd - File descriptor for event node, to ioctl and read from. <br>
 * enabled - 0 disabled, else can write events to fd. <br>
 *
 * */
typedef struct mcr_Device
{
	struct uinput_user_dev device ;
	mcr_Map type_value_map ;
	int fd ;
	int event_fd ;
	int enabled ;
} mcr_Device ;

// Non-static to inline write function.
//! \brief To send Key and HIDEcho signals.
extern mcr_Device mcr_KeyDev ;
//! \brief To send MoveCursor unjustified.
extern mcr_Device mcr_AbsDev ;
//! \brief To send MoveCursor justified.
extern mcr_Device mcr_RelDev ;

//! \brief Append similar input_event to the end of all sending of events.
extern const struct input_event mcr_Syncer ;

/*! \brief ctor. Initialize empty device, map, and files.
 *
 * \param devPt mcr_Device *
 * */
MCR_API void mcr_Device_init ( mcr_Device * devPt ) ;
/*! \brief Free all mapped arrays, and free the map itself.
 *
 * \param devPt mcr_Device *
 * */
MCR_API void mcr_Device_free ( mcr_Device * devPt ) ;
/*! \brief Start or end user device. This will modify fd, event_fd,
 * and enabled state.
 *
 * \param devPt mcr_Device *
 * \param enable int 0 disable, any other value to enable.
 * \return int 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Device_enable ( mcr_Device * devPt, int enable ) ;

//! \brief Call this if you do not know what's going on.
MCR_API void mcr_Device_usage ( ) ;

/*! \brief Set input bit values for a single bit type.
 *
 * \param devPt mcr_Device *
 * \param bitType int Type of event to set values for.
 * \param bits int * Set of all values to set for bitType.
 * \param bitLen size_t Length of bits.
 * \return 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Device_set_bits ( mcr_Device * devPt, int bitType,
		int * bits, size_t bitLen ) ;
/*! \brief \return Get the set of all input bits to be set.
 * NULL if not found.
 *
 * \param devPt mcr_Device *
 * \param bitType int Type of event to find values for.
 * */
MCR_API mcr_Array * mcr_Device_get_bits ( mcr_Device * devPt,
		int bitType ) ;
/*! \brief \return Non-zero if device has evbits to write,
 *
 * \param devPt mcr_Device *
 * */
MCR_API int mcr_Device_has_evbit ( mcr_Device * devPt ) ;

//
// Sending, macro inlined for efficiency.
//
/*!
 * \brief Send input_events to given device.
 *
 * \param dev mcr_Device
 * \param eventObjects input_event * Pointer to or array of input_events.
 * \param size size_t Byte size of all input_events in eventObjects.
 * \param success int Will be 0 on failure.
 * */
# define MCR_DEV_SEND( dev, eventObjects, size, success ) \
	if ( ( dev ).fd == -1 ) \
		success = 0 ; \
	else \
		success = write ( ( dev ).fd, eventObjects, size ) == -1 ? \
				0 : success ;

/*!
 * \brief \ref MCR_DEV_SEND for single input_event.
 *
 * \param dev mcr_Device
 * \param eventObject input_event * Pointer to single event object.
 * \param success int Will be 0 on failure.
 * */
# define MCR_DEV_SEND_ONE( dev, eventObject, success) \
	MCR_DEV_SEND ( dev, eventObject, sizeof ( struct input_event ), \
			success )

//! \brief Send a syncing event to this device.
# define MCR_DEV_SYNC( dev, success ) \
	MCR_DEV_SEND ( dev, & mcr_Syncer, sizeof ( struct input_event ), \
			success )

/*! \brief Initialize values for known mcr_Device objects,
 * and allocate resources.
 * */
MCR_API void mcr_Device_initialize ( ) ;
/*! \brief Dealocate resources for linux devices. */
MCR_API void mcr_Device_cleanup ( ) ;

// TODO: dynamic path.

# endif
