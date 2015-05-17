/* include/signal/standard.h - Standard, predefined signal types.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file signal/standard.h
 * \brief mcr_Signal data types : mcr_Alarm, mcr_HIDEcho, mcr_Key,
 * mcr_MoveCursor, mcr_NoOp, and mcr_Scroll. <br>
 * mcr_ISignals : mcr_IAlarm, mcr_IHIDEcho, mcr_IKey,
 * mcr_IMoveCursor, mcr_INoOp, and mcr_IScroll.
 * */

# ifndef MCR_STANDARD_H
# define MCR_STANDARD_H

# include "signal/isignal.h"

//
// Standard signal data.
//
// Alarm
/*! \brief A signal to pause execution until a day, hour and minute mark.
 *
 * tm members are tm_sec 0-60, tm_min 0-59,
 * tm_hour 0-23, tm_mday 1-31, tm_mon 0-11, tm_year, tm_wday 0-6,
 * tm_yday 0-365, tm_isdst ( daylight savings ).
 * */
typedef struct tm mcr_Alarm ;
// HIDEcho
/*!
 * \brief Simulate mouse click, screen tap, etc.
 * spatial position activation.
 * */
typedef struct mcr_HIDEcho mcr_HIDEcho ;
// Key
/*!
 * \brief Send or receive keyboard keys.
 *
 * Keyboard signals have both a scan code, specified by the hardware,
 * or a key code, specified by the operating system. One or both may
 * be used.
 * <br>
 * A 0 value of both key code and scan code should be logically
 * interpreted as either, "no key available, do not send, "
 * or "either a key code or scan code of any value."
 * */
typedef struct mcr_Key mcr_Key ;
// MoveCursor
//! \brief Change spatial coordinates of user's cursor.
typedef struct mcr_MoveCursor mcr_MoveCursor ;
// NoOp
/*! \brief A signal to pause execution in seconds and nanoseconds.
 *
 * timespec members are : tv_sec, and tv_nsec.
 * */
typedef struct timespec mcr_NoOp ;
// Scroll
//! \brief Scroll through pages/visible area.
typedef struct mcr_Scroll mcr_Scroll ;


//
// Standard signal ISignals, i.e. type definition, or vtable.
//
/*! \brief Signal interface for \ref mcr_Alarm. */
extern mcr_ISignal mcr_iAlarm ;
/*! \brief Signal interface for \ref mcr_HIDEcho. */
extern mcr_ISignal mcr_iHIDEcho ;
/*! \brief Signal interface for \ref mcr_Key. */
extern mcr_ISignal mcr_iKey ;
/*! \brief Signal interface for \ref mcr_MoveCursor. */
extern mcr_ISignal mcr_iMoveCursor ;
/*! \brief Signal interface for \ref mcr_NoOp. */
extern mcr_ISignal mcr_iNoOp ;
/*! \brief Signal interface for \ref mcr_Scroll. */
extern mcr_ISignal mcr_iScroll ;


//
// Codes by name.
//
// Echo
/*!
 * \brief mcr_Echo_code Get event code from its name.
 *
 * \param eventName Name of event code.
 * \return Event code. -1 if it is not registered.
 * */
MCR_API int mcr_Echo_code ( const char * eventName ) ;
/*!
 * \brief mcr_Echo_name Get name of event code.
 * */
MCR_API const char * mcr_Echo_name ( int eventCode ) ;
/*!
 * \brief Get the count of registered echo codes.
 * */
MCR_API size_t mcr_Echo_count ( ) ;
// Key
/*!
 * \brief Get key code from its name.
 *
 * \param keyName Name of key code.
 * \return Key code. 0 if it is not registered.
 * */
MCR_API int mcr_Key_code ( const char * keyName ) ;
/*!
 * \brief Get name of key code.
 * */
MCR_API const char * mcr_Key_name ( int keyCode ) ;
/*!
 * \brief Get the count of registered key codes.
 * */
MCR_API size_t mcr_Key_count ( ) ;

//
// ctors for native types
//
/*! \brief Echo ctor.
 * */
MCR_API void mcr_Echo_init ( mcr_HIDEcho * echoPt ) ;
/*! \brief Echo ctor with initial data.
 * \param event Echo event code.
 * */
MCR_API void mcr_Echo_init_with ( mcr_HIDEcho * echoPt, const int event ) ;
/*! \brief Key ctor.
 * */
MCR_API void mcr_Key_init ( mcr_Key * keyPt ) ;
/*! \brief Key ctor with initial data
 *
 * \param key Key code.
 * \param scan Scan code.
 * */
MCR_API void mcr_Key_init_with ( mcr_Key * keyPt, const int key,
		const int scan, const mcr_KeyUpType keyUp ) ;
/*! \brief MoveCursor ctor.
 *
 * \param mcPt mcr_MoveCursor *
 * */
MCR_API void mcr_MoveCursor_init ( mcr_MoveCursor * mcPt ) ;
/*! \brief MoveCursor ctor with initial data.
 *
 * \param blip Spatial coordinates of movement.
 * \param cursorJustify 0 absolute, else relative.
 * */
MCR_API void mcr_MoveCursor_init_with ( mcr_MoveCursor * mcPt,
		const mcr_SpacePosition blip, const int cursorJustify ) ;
/*! \brief Scroll ctor
 * */
MCR_API void mcr_Scroll_init ( mcr_Scroll * scrollPt ) ;
/*! \brief Scroll ctor with initial data.
 *
 * \param dimVals Spatial coordinates to scroll through.
 * */
MCR_API void mcr_Scroll_init_with ( mcr_Scroll * scrollPt,
		const mcr_Dimensions dimVals ) ;


//
// Native data manipulation.
//
// HIDEcho
/*! \brief \return int Echo event code.
 * */
MCR_API int mcr_Echo_get ( const mcr_HIDEcho * echoPt ) ;
/*! \brief Set echo code.
 * \param echo Echo event code.
 * */
MCR_API void mcr_Echo_set ( mcr_HIDEcho * echoPt, int echo ) ;
// Key
/*! \brief Get the key code.
 * */
MCR_API int mcr_Key_get ( const mcr_Key * keyPt ) ;
/*! \brief Set key code.
 * */
MCR_API void mcr_Key_set ( mcr_Key * keyPt, int key ) ;
/*! \brief Get the Scan code.
 * */
MCR_API int mcr_Key_get_scan ( const mcr_Key * keyPt ) ;
/*! \brief Set scan code.
 * */
MCR_API void mcr_Key_set_scan ( mcr_Key * keyPt, int scan ) ;
/*! \brief Get the \ref mcr_KeyUpType Key up type.
 * */
MCR_API mcr_KeyUpType mcr_Key_get_up_type ( const mcr_Key * keyPt ) ;
/*! \brief Set key up type.
 * */
MCR_API void mcr_Key_set_up_type ( mcr_Key * keyPt,
		const mcr_KeyUpType keyUp ) ;
// MoveCursor
/*! \brief Get the cursor spatial positions, copied into the buffer.
 * \param buffer Buffer to copy into.
 * */
MCR_API void mcr_MoveCursor_get ( const mcr_MoveCursor * mcPt,
		mcr_SpacePosition buffer ) ;
/*! \brief Set cursor spatial positions.
 *
 * \param pos Buffer to set positions from.
 * */
MCR_API void mcr_MoveCursor_set ( mcr_MoveCursor * mcPt,
		const mcr_SpacePosition pos ) ;
/*! \brief Get the value of a single coordinate position.
 *
 * \param coordinate x, y, z coordinate type. \ref MCR_DIMENSION_MAX
 * */
MCR_API long long mcr_MoveCursor_get_position (
		const mcr_MoveCursor * mcPt, int coordinate ) ;
/*! \brief Set the position of one coordinate.
 *
 * \param coordinate x, y, z, coordinate type. \ref MCR_DIMENSION_MAX
 * \param value Coordinate value to set.
 * */
MCR_API void mcr_MoveCursor_set_position ( mcr_MoveCursor * mcPt,
		int coordinate, long long value ) ;
/*! \brief Get Cursor justification.
 *
 * \return 0 is absolute, otherwise cursor is justified.
 * */
MCR_API int mcr_MoveCursor_is_justify ( const mcr_MoveCursor * mcPt ) ;
/*! \brief Set cursor justification.
 *
 * \param cursorJustify 0 absolute, otherwise cursor is justified.
 * */
MCR_API void mcr_MoveCursor_enable_justify ( mcr_MoveCursor * mcPt,
		int cursorJustify ) ;
// Scroll
/*! \brief Set all coordinate dimensions.
 *
 * \param pos Values to set from.
 * */
MCR_API void mcr_Scroll_set ( mcr_Scroll * scrollPt,
		const mcr_Dimensions pos ) ;
/*! \brief All coordinate dimensions are copied into the buffer.
 *
 * \param buffer Values are copied here.
 * */
MCR_API void mcr_Scroll_get ( const mcr_Scroll * scrollPt,
		mcr_Dimensions buffer ) ;
/*! \brief Get the value of a single coordinate dimension.
 *
 * \param coordinate int x, y, z coordinate type. \ref MCR_DIMENSION_MAX
 * */
MCR_API long long mcr_Scroll_get_dimension ( mcr_Scroll * scrollPt,
		int coordinate ) ;
/*! \brief Set a single coordinate dimension.
 *
 * \param coordinate x, y, z coordinate type. \ref MCR_DIMENSION_MAX
 * \param value Coordinate value to set.
 * */
MCR_API void mcr_Scroll_set_dimension ( mcr_Scroll * scrollPt,
		int coordinate, long long value ) ;

//
// Send functions that are placed into ISignals.
//
/*!
 * \brief Pause execution until time point.
 *
 * \param signalData Has data member \ref mcr_Alaram or struct tm.
 * \return 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Alarm_send ( mcr_Signal * signalData ) ;
/*!
 * \brief Send human interface impersonation.
 *
 * \param signalData Has data member \ref mcr_HIDEcho.
 * \return 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Echo_send ( mcr_Signal * signalData ) ;
/*!
 * \brief Simulate keyboard keys.
 *
 * \param signalData Has data member \ref mcr_Key.
 * \return 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Key_send ( mcr_Signal * signalData ) ;
/*!
 * \brief Move HID cursor position.
 *
 * \param signalData Has data member \ref mcr_MoveCursor.
 * \return 0 on failure, otherwise successful.
 * */
MCR_API int mcr_MoveCursor_send ( mcr_Signal * signalData ) ;
/*!
 * \brief Pause execution in seconds and nanoseconds.
 *
 * \param signalData Has data member \ref mcr_NoOp
 * or struct timespec.
 * \return 0 on failure, otherwise successful.
 * */
MCR_API int mcr_NoOp_send ( mcr_Signal * signalData ) ;
/*!
 * \brief Scroll through pages/visible area.
 *
 * \param signalData Has data member \ref mcr_Scroll.
 * \return 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Scroll_send ( mcr_Signal * signalData ) ;


//
// Some extra, possibly useful, utilities.
//
MCR_API void mcr_cursor_position ( mcr_SpacePosition buffer ) ;
/*! mcr_resembles_justified
 * \brief For both positions, either may be 0, or the same coordinate
 * array member must either be both negative, or both positive.
 *
 * \return 0 false, else true.
 * */
MCR_API int mcr_resembles_justified ( const mcr_Dimensions first,
		const mcr_Dimensions second ) ;
/*! mcr_resembles_absolute
 * \brief For both positions, the same coordinate array member must
 * be valued within the measurementError.
 *
 * \return 0 false, else true.
 * */
MCR_API int mcr_resembles_absolute ( const mcr_Dimensions first,
		const mcr_Dimensions second,
		const unsigned int measurementError ) ;
/*!
 * \brief If justified then \ref mcr_resembles_justified,
 * else \ref mcr_resembles_absolute
 *
 * \param measurementError Only used in
 * \ref mcr_resembles_absolute.
 * \return 0 false, else true.
 * */
MCR_API int mcr_resembles ( const mcr_MoveCursor * lhs,
		const mcr_MoveCursor * rhs,
		const unsigned int measurementError ) ;

//
// Code development.
//
// Echo
/*!
 * \brief Set the name mapped by a echo code.
 *
 * The name will also map to this echo code.
 * \return 0 on failure, otherwise success.
 * */
MCR_API int mcr_Echo_set_name ( int eventCode, const char * eventName ) ;
/*!
 * \brief Map an additional name to this echo code.
 *
 * \return 0 on failure, otherwise success.
 * */
MCR_API int mcr_Echo_add_name ( int eventCode, const char * addName ) ;
/*!
 * \brief Rename an echo code.
 *
 * \return 0 on failure, otherwise success.
 * */
MCR_API int mcr_Echo_rename ( int eventCode,
		const char * newName ) ;
/*!
 * \brief Rename an echo code. The code is first found from its
 * old name.
 *
 * \return 0 on failure, otherwise success.
 * */
MCR_API int mcr_Echo_rename_from_name ( const char * oldName,
		const char * newName ) ;
/*! \brief Remove all mapped echo codes. */
MCR_API void mcr_Echo_clear_all ( ) ;
// Key
/*!
 * \brief Set the name mapped by a key code.
 *
 * The name will also map to this key code.
 * \return 0 if unsuccessful, othewise successful.
 * */
MCR_API int mcr_Key_set_name ( int keyCode, const char * newName ) ;
/*!
 * \brief Map an additional name to this key code.
 *
 * \return 0 on failure, otherwise success.
 * */
MCR_API int mcr_Key_add_name ( int keyCode, const char * addName ) ;
/*!
 * \brief Rename a key code.
 *
 * \return 0 on failure, otherwise success.
 * */
MCR_API int mcr_Key_rename ( int keyCode,
		const char * newName ) ;
/*!
 * \brief Rename a key code. The code is first found from its
 * old name.
 *
 * \return 0 on failure, otherwise success.
 * */
MCR_API int mcr_Key_rename_from_name ( const char * oldName,
		const char * newName ) ;
/*! \brief Remove all mapped key codes. */
MCR_API void mcr_Key_clear_all ( ) ;

/*!
 * \brief Initialize standard signals.
 *
 * Called by \ref mcr_signal_initialize.
 * Calls \ref mcr_standard_native_initialize
 * */
MCR_API void mcr_standard_initialize ( ) ;
/*!
 * \brief Clean up standard signals.
 *
 * Called by \ref mcr_signal_cleanup.
 * Calls \ref mcr_standard_native_cleanup
 * */
MCR_API void mcr_standard_cleanup ( ) ;

//
// Must be implemented in native directory.
//
/*!
 * \brief Initialize native requirements for standard signals.
 *
 * Called by \ref mcr_standard_initialize
 * */
MCR_API void mcr_standard_native_initialize ( ) ;
/*!
 * \brief Clean up native requirements for standard signals.
 *
 * Called by \ref mcr_standard_cleanup
 * */
MCR_API void mcr_standard_native_cleanup ( ) ;


//
// Inline data manipulation macros.
//
// mcr_HIDEcho
//! \brief \ref mcr_Echo_get
# define MCR_ECHO_GET( echoPt )
//! \brief \ref mcr_Echo_set
# define MCR_ECHO_SET( echoPt, echo )
// mcr_Key
//! \brief \ref mcr_Key_get
# define MCR_KEY_GET( keyPt )
//! \brief \ref mcr_Key_set
# define MCR_KEY_SET( keyPt, key )
//! \brief \ref mcr_Key_get_scan
# define MCR_KEY_GET_SCAN( keyPt )
//! \brief \ref mcr_Key_set_scan
# define MCR_KEY_SET_SCAN( keyPt, scan )
//! \brief \ref mcr_Key_get_up_type
# define MCR_KEY_GET_UP_TYPE( keyPt )
//! \brief \ref mcr_Key_set_up_type
# define MCR_KEY_SET_UP_TYPE( keyPt, keyUp )
// mcr_MoveCursor
//! \brief \ref mcr_MoveCursor_get
# define MCR_MOVECURSOR_GET( mcPt, buffer )
//! \brief \ref mcr_MoveCursor_set
# define MCR_MOVECURSOR_SET( mcPt, buffer )
//! \brief \ref mcr_MoveCursor_get_position
# define MCR_MOVECURSOR_GET_POSITION( mcPt, posType )
//! \brief \ref mcr_MoveCursor_set_position
# define MCR_MOVECURSOR_SET_POSITION( mcPt, posType, val )
//! \brief \ref mcr_MoveCursor_is_justify
# define MCR_MOVECURSOR_IS_JUSTIFY( mcPt )
//! \brief \ref mcr_MoveCursor_enable_justify
# define MCR_MOVECURSOR_ENABLE_JUSTIFY( mcPt, enable )
// mcr_Scroll
//! \brief \ref mcr_Scroll_set
# define MCR_SCROLL_GET( scrollPt, buffer )
//! \brief \ref mcr_Scroll_get
# define MCR_SCROLL_SET( scrollPt, buffer )
//! \brief \ref mcr_Scroll_get_dimension
# define MCR_SCROLL_GET_DIMENSION( scrollPt, posType )
//! \brief \ref mcr_Scroll_set_dimension
# define MCR_SCROLL_SET_DIMENSION( scrollPt, posType, val )

//
// Inline send macros.
//
//! \brief \ref mcr_Alarm_send
# define MCR_ALARM_SEND( alarmPt, success ) \
	if ( thrd_sleep_until ( alarmPt ) != thrd_success ) \
		success = 0 ;
//! \brief \ref mcr_Echo_send
# define MCR_ECHO_SEND( echoPt, success )
//! \brief \ref mcr_Key_send
# define MCR_KEY_SEND( keyPt, success )
//! \brief \ref mcr_MoveCursor_send
# define MCR_MOVECURSOR_SEND( movePt, success )
//! \brief \ref mcr_NoOp_send
# define MCR_NOOP_SEND( noopPt, success ) \
	if ( thrd_sleep ( noopPt, NULL ) != thrd_success ) \
		success = 0 ;
//! \brief \ref mcr_Scroll_send
# define MCR_SCROLL_SEND( scrollPt, success )

# include STRINGIFY(signal/MCR_NATIVE_DIR/standard.h)

# endif // MCR_STANDARD_H
