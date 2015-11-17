/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
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

/*! \file signal/standard.h
 * \brief \ref mcr_Signal data types : \ref mcr_Alarm, \ref mcr_HIDEcho,
 * \ref mcr_Key, \ref mcr_MoveCursor, \ref mcr_NoOp,
 * and \ref mcr_Scroll <br>
 * \ref mcr_ISignal : \ref mcr_IAlarm, \ref mcr_IHIDEcho, \ref mcr_IKey,
 * \ref mcr_IMoveCursor, \ref mcr_INoOp, and \ref mcr_IScroll
 * */

# ifndef MCR_STANDARD_H
# define MCR_STANDARD_H

# include "signal/isignal.h"

//
// Standard signal data.
//
// Alarm
/*! \brief \ref mcr_iAlarm */
typedef struct tm mcr_Alarm ;
// HIDEcho
/*! \brief \ref mcr_iHIDEcho */
typedef struct mcr_HIDEcho mcr_HIDEcho ;
/*! \brief \ref mcr_HIDEcho */
typedef mcr_HIDEcho mcr_Echo ;
// Key
/*! \brief \ref mcr_iKey */
typedef struct mcr_Key mcr_Key ;
// MoveCursor
/*! \brief \ref mcr_iMoveCursor */
typedef struct mcr_MoveCursor mcr_MoveCursor ;
// NoOp
/*! \brief \ref mcr_iNoOp */
typedef struct timespec mcr_NoOp ;
// Scroll
/*! \brief \ref mcr_iScroll */
typedef struct mcr_Scroll mcr_Scroll ;

//
// Standard signal ISignals, i.e. type definition
//
/*! \brief Pause execution until a day, hour and minute mark.
 *
 * tm members are tm_sec 0-60, tm_min 0-59,
 * tm_hour 0-23, tm_mday 1-31, tm_mon 0-11, tm_year, tm_wday 0-6,
 * tm_yday 0-365, tm_isdst ( daylight savings )
 * */
MCR_API extern mcr_ISignal mcr_iAlarm ;
/*! \brief Simulate mouse click, screen tap, etc.
 * spatial position activation
 * */
MCR_API extern mcr_ISignal mcr_iHIDEcho ;
# define mcr_iEcho mcr_iHIDEcho
/*! \brief Simulate keyboard keys
 *
 * Keyboard signals have a scan code, specified by the hardware,
 * or a key code, specified by the operating system. One or both may
 * be used.
 * <br>
 * A 0 value of both key code and scan code should be logically
 * interpreted as either, "no key available, do not send,"
 * or "either a key code or scan code of any value."
 * */
MCR_API extern mcr_ISignal mcr_iKey ;
//! \brief Change spatial coordinates of user's cursor.
MCR_API extern mcr_ISignal mcr_iMoveCursor ;
/*! \brief Pause execution in seconds and nanoseconds.
 *
 * timespec members are : tv_sec, and tv_nsec
 * */
MCR_API extern mcr_ISignal mcr_iNoOp ;
//! \brief Scroll visible area.
MCR_API extern mcr_ISignal mcr_iScroll ;

//
// Implement in native directory
//
/*! \brief Enable or disable instance of standard signals. */
MCR_API void mcr_standard_enable ( int enable ) ;
/*! \brief Load native key definitions */
MCR_API void mcr_Key_load_contract ( ) ;
/*! \brief Load native echo definitions. */
MCR_API void mcr_Echo_load_contract ( ) ;

//
// Codes by name
//
// Echo
/*! \brief Get event code from its name.
 *
 * \param eventName Name of echo code
 * \return Echo code, or -1 if it is not registered
 * */
MCR_API int mcr_Echo_code ( const char * eventName ) ;
/*! \brief Get the name of an echo code.
 * */
MCR_API const char * mcr_Echo_name ( int eventCode ) ;
/*! \brief Get the number of registered echo codes.
 * */
MCR_API size_t mcr_Echo_count ( ) ;
// Key
/*! \brief Get a key code from its name.
 *
 * \param keyName Name of key code
 * \return Key code, or 0 if it is not registered
 * */
MCR_API int mcr_Key_code ( const char * keyName ) ;
/*! \brief Get the name of a key code.
 * */
MCR_API const char * mcr_Key_name ( int keyCode ) ;
/*! \brief Get the number of registered key codes.
 * */
MCR_API size_t mcr_Key_count ( ) ;

//
// Native data manipulation
//
// HIDEcho
/*! \brief \return int Echo event code
 * */
MCR_API int mcr_Echo_echo ( const mcr_HIDEcho * echoPt ) ;
/*! \brief Set echo code
 * \param echo Echo event code
 * */
MCR_API void mcr_Echo_set_echo ( mcr_HIDEcho * echoPt, int echo ) ;
// Key
/*! \brief Get key code
 * */
MCR_API int mcr_Key_key ( const mcr_Key * keyPt ) ;
/*! \brief Set key code
 * */
MCR_API void mcr_Key_set_key ( mcr_Key * keyPt, int key ) ;
/*! \brief Get Scan code
 * */
MCR_API int mcr_Key_scan ( const mcr_Key * keyPt ) ;
/*! \brief Set scan code
 * */
MCR_API void mcr_Key_set_scan ( mcr_Key * keyPt, int scan ) ;
/*! \brief Get key up type
 * */
MCR_API mcr_KeyUpType mcr_Key_up_type ( const mcr_Key * keyPt ) ;
/*! \brief Set key up type
 * */
MCR_API void mcr_Key_set_up_type ( mcr_Key * keyPt,
		const mcr_KeyUpType keyUp ) ;
// MoveCursor
/*! \brief Get cursor spatial positions, copied into the buffer
 *
 * \param buffer Buffer to copy into
 * */
MCR_API void mcr_MoveCursor_position ( const mcr_MoveCursor * mcPt,
		mcr_SpacePosition buffer ) ;
/*! \brief Set cursor spatial positions
 *
 * \param pos Buffer to set positions from
 * */
MCR_API void mcr_MoveCursor_set_position ( mcr_MoveCursor * mcPt,
		const mcr_SpacePosition pos ) ;
/*! \brief Get the value of a single coordinate position.
 *
 * \param coordinate x, y, z coordinate type, \ref MCR_DIMENSION_MAX
 * */
MCR_API long long mcr_MoveCursor_coordinate (
		const mcr_MoveCursor * mcPt, int coordinate ) ;
/*! \brief Set the position of one coordinate.
 *
 * \param coordinate x, y, z, coordinate type, \ref MCR_DIMENSION_MAX
 * \param value Coordinate value to set
 * */
MCR_API void mcr_MoveCursor_set_coordinate ( mcr_MoveCursor * mcPt,
		int coordinate, long long value ) ;
/*! \brief Get cursor justification
 *
 * \return 0 is absolute, otherwise cursor is justified
 * */
MCR_API int mcr_MoveCursor_justify ( const mcr_MoveCursor * mcPt ) ;
/*! \brief Set cursor justification
 *
 * \param cursorJustify 0 absolute, otherwise cursor is justified
 * */
MCR_API void mcr_MoveCursor_set_justify ( mcr_MoveCursor * mcPt,
		int cursorJustify ) ;
// Scroll
/*! \brief All coordinate dimensions are copied into the buffer.
 *
 * \param buffer Values are copied here
 * */
MCR_API void mcr_Scroll_dimensions ( const mcr_Scroll * scrollPt,
		mcr_Dimensions buffer ) ;
/*! \brief Set all coordinate dimensions.
 *
 * \param pos Values to set from
 * */
MCR_API void mcr_Scroll_set_dimensions ( mcr_Scroll * scrollPt,
		const mcr_Dimensions pos ) ;
/*! \brief Get the value of a single coordinate dimension.
 *
 * \param coordinate x, y, z coordinate type, \ref MCR_DIMENSION_MAX
 * */
MCR_API long long mcr_Scroll_coordinate ( mcr_Scroll * scrollPt,
		int coordinate ) ;
/*! \brief Set a single coordinate dimension.
 *
 * \param coordinate x, y, z coordinate type, \ref MCR_DIMENSION_MAX
 * \param value Coordinate value to set
 * */
MCR_API void mcr_Scroll_set_coordinate ( mcr_Scroll * scrollPt,
		int coordinate, long long value ) ;

//
// Interface functions, internal use
//
/*! \brief Pause execution until a time point.
 *
 * \param signalData Has data member \ref mcr_Alarm or struct tm
 * \return 0 on failure, otherwise successful
 * */
MCR_API int mcr_Alarm_send ( mcr_Signal * signalData ) ;
//! \brief \ref mcr_compare_fnc to compare \ref mcr_Alarm or struct tm
MCR_API int mcr_tm_compare ( const void * lhs, const void * rhs ) ;
/* Default copy, init, and free */
/*! \brief Send human interface impersonation.
 *
 * \param signalData Has data member \ref mcr_HIDEcho
 * \return 0 on failure, otherwise successful
 * */
MCR_API int mcr_Echo_send ( mcr_Signal * signalData ) ;
/*! \brief Compare \ref mcr_HIDEcho */
MCR_API int mcr_Echo_compare ( const void * lhs, const void * rhs ) ;
/*! \brief Copy \ref mcr_HIDEcho */
MCR_API void mcr_Echo_copy ( void * dstPt, void * srcPt ) ;
/*! \brief Initialize \ref mcr_HIDEcho */
MCR_API void mcr_Echo_init ( void * echoPt ) ;
/*! \brief Initialize \ref mcr_HIDEcho, and give an echo code value */
MCR_API void mcr_Echo_init_with ( mcr_HIDEcho * echoPt, const int event ) ;
/* No free */
/*! \brief Simulate keyboard keys.
 *
 * \param signalData Has data member \ref mcr_Key
 * \return 0 on failure, otherwise successful
 * */
MCR_API int mcr_Key_send ( mcr_Signal * signalData ) ;
/*! \brief Compare \ref mcr_Key */
MCR_API int mcr_Key_compare ( const void * lhs, const void * rhs ) ;
/*! \brief Copy \ref mcr_Key */
MCR_API void mcr_Key_copy ( void * dstPt, void * srcPt ) ;
/*! \brief Initialize \ref mcr_Key */
MCR_API void mcr_Key_init ( void * keyPt ) ;
/*! \brief Initialize \ref mcr_Key and set key, scan, and up type */
MCR_API void mcr_Key_init_with ( mcr_Key * keyPt, const int key,
		const int scan, const mcr_KeyUpType keyUp ) ;
/* No free */
/*! \brief Move HID cursor position.
 *
 * \param signalData Has data member \ref mcr_MoveCursor
 * \return 0 on failure, otherwise successful
 * */
MCR_API int mcr_MoveCursor_send ( mcr_Signal * signalData ) ;
/*! \brief Compare \ref mcr_MoveCursor */
MCR_API int mcr_MoveCursor_compare ( const void * lhs,
		const void * rhs ) ;
/*! \brief Copy \ref mcr_MoveCursor */
MCR_API void mcr_MoveCursor_copy ( void * dstPt, void * srcPt ) ;
/*! \brief Initialize \ref mcr_MoveCursor */
MCR_API void mcr_MoveCursor_init ( void * mcPt ) ;
/*! \brief Initialize \ref mcr_MoveCursor and set position and cursor
 * justification */
MCR_API void mcr_MoveCursor_init_with ( mcr_MoveCursor * mcPt,
		const mcr_SpacePosition blip, const int cursorJustify ) ;
/* No free */
/*! \brief Pause execution in seconds and nanoseconds.
 *
 * \param signalData Has data member \ref mcr_NoOp
 * or struct timespec
 * \return 0 on failure, otherwise successful
 * */
MCR_API int mcr_NoOp_send ( mcr_Signal * signalData ) ;
/* Default everything */
/*! \brief Scroll through visible area.
 *
 * \param signalData Has data member \ref mcr_Scroll
 * \return 0 on failure, otherwise successful
 * */
MCR_API int mcr_Scroll_send ( mcr_Signal * signalData ) ;
/*! \brief Compare \ref mcr_Scroll */
MCR_API int mcr_Scroll_compare ( const void * lhs,
		const void * rhs ) ;
/*! \brief Copy \ref mcr_Scroll */
MCR_API void mcr_Scroll_copy ( void * dstPt, void * srcPt ) ;
/*! \brief Initialize \ref mcr_Scroll */
MCR_API void mcr_Scroll_init ( void * scrollPt ) ;
/*! \brief Initialize \ref mcr_Scroll and set dimensions */
MCR_API void mcr_Scroll_init_with ( mcr_Scroll * scrollPt,
		const mcr_Dimensions dimVals ) ;
/* No free */

//
// Some extra, possibly useful, utilities
//
MCR_API void mcr_cursor_position ( mcr_SpacePosition buffer ) ;
/*! \brief For both positions, either may be 0, or the same coordinate
 * array member must either be both negative, or both positive.
 *
 * \return 0 false, else true
 * */
MCR_API int mcr_resembles_justified ( const mcr_Dimensions first,
		const mcr_Dimensions second ) ;
/*! \brief For both positions, the same coordinate array member must
 * be valued within the measurementError.
 *
 * \return 0 false, else true
 * */
MCR_API int mcr_resembles_absolute ( const mcr_Dimensions first,
		const mcr_Dimensions second,
		const unsigned int measurementError ) ;
/*! \brief If justified then \ref mcr_resembles_justified,
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
// Code development
//
// Echo
/*! \brief Set the name mapped by an echo code.
 *
 * The name will also map to this echo code.
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_Echo_set_name ( int eventCode,
		const char * eventName ) ;
/*! \brief Map an additional name to this echo code.
 *
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_Echo_add_name ( int eventCode,
		const char * addName ) ;
/*! \brief Add a set of names to map to an echo code.
 *
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_Echo_add_names ( int eventCode,
		const char ** addNames, size_t bufferLen ) ;
/*! \brief \ref mcr_Echo_set_name and \ref mcr_Echo_add_names
 *
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_Echo_set_names ( int eventCode,
		const char * eventName,
		const char ** addNames, size_t bufferLen ) ;
/*! \brief Rename an echo code
 *
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_Echo_rename ( int eventCode,
		const char * newName ) ;
/*! \brief Rename an echo code. The code is first found from its
 * old name.
 *
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_Echo_rename_from_name ( const char * oldName,
		const char * newName ) ;
/*! \brief Remove all mapped echo codes. */
MCR_API void mcr_Echo_clear ( ) ;
// Key
/*! \brief Set the name mapped by a key code.
 *
 * The name will also map to this key code.
 * \return 0 if unsuccessful, othewise successful
 * */
MCR_API int mcr_Key_set_name ( int keyCode,
		const char * newName ) ;
/*! \brief Map an additional name to this key code.
 *
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_Key_add_name ( int keyCode,
		const char * addName ) ;
/*! \brief Add a set of names to map to a key code.
 *
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_Key_add_names ( int keyCode,
		const char ** addNames, size_t bufferLen ) ;
/*! \brief \ref mcr_Key_set_name and \ref mcr_Key_add_names
 *
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_Key_set_names ( int keyCode,
		const char * newName,
		const char ** addNames, size_t bufferLen ) ;
/*! \brief Rename a key code.
 *
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_Key_rename ( int keyCode,
		const char * newName ) ;
/*! \brief Rename a key code. The code is first found from its
 * old name.
 *
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_Key_rename_from_name ( const char * oldName,
		const char * newName ) ;
/*! \brief Remove all mapped key codes. */
MCR_API void mcr_Key_clear ( ) ;

/*! \brief Initialize standard signals.
 *
 * Called by \ref mcr_signal_initialize
 * Calls \ref mcr_standard_native_initialize
 * */
MCR_API void mcr_standard_initialize ( ) ;
/*! \brief Initialize names for standard signals and codes.
 *
 * Called by \ref mcr_standard_initialize
 * */
MCR_API void mcr_standardnames_initialize ( ) ;
/*!
 * \brief Clean up standard signals.
 *
 * Called by \ref mcr_signal_cleanup
 * Calls \ref mcr_standard_native_cleanup
 * */
MCR_API void mcr_standard_cleanup ( void ) ;
/*! \brief Clean up names for standard signals and codes.
 *
 * Called by \ref mcr_standard_cleanup
 * */
MCR_API void mcr_standardnames_cleanup ( void ) ;

//
// Must be implemented in native directory.
//
/*! \brief Initialize native requirements for standard signals.
 *
 * Called by \ref mcr_standard_initialize
 * */
MCR_API void mcr_standard_native_initialize ( ) ;
/*! \brief Clean up native requirements for standard signals.
 *
 * Called by \ref mcr_standard_cleanup
 * */
MCR_API void mcr_standard_native_cleanup ( void ) ;

//
// Inline data manipulation macros
//
// mcr_HIDEcho
# ifndef MCR_ECHO_ECHO
//! \brief \ref mcr_Echo_echo
# define MCR_ECHO_ECHO( echo )
//! \brief \ref mcr_Echo_set_echo
# define MCR_ECHO_SET_ECHO( echo, echoCode )
// mcr_Key
//! \brief \ref mcr_Key_key
# define MCR_KEY_KEY( key )
//! \brief \ref mcr_Key_set_key
# define MCR_KEY_SET_KEY( key, keyCode )
//! \brief \ref mcr_Key_scan
# define MCR_KEY_SCAN( key )
//! \brief \ref mcr_Key_set_scan
# define MCR_KEY_SET_SCAN( key, scan )
//! \brief \ref mcr_Key_up_type
# define MCR_KEY_UP_TYPE( key )
//! \brief \ref mcr_Key_set_up_type
# define MCR_KEY_SET_UP_TYPE( key, keyUp )
// mcr_MoveCursor
//! \brief \ref mcr_MoveCursor_position
# define MCR_MOVECURSOR_POSITION( mc, buffer )
//! \brief \ref mcr_MoveCursor_set_position
# define MCR_MOVECURSOR_SET_POSITION( mc, buffer )
//! \brief \ref mcr_MoveCursor_coordinate
# define MCR_MOVECURSOR_COORDINATE( mc, posType )
//! \brief \ref mcr_MoveCursor_set_coordinate
# define MCR_MOVECURSOR_SET_COORDINATE( mc, posType, val )
//! \brief \ref mcr_MoveCursor_justify
# define MCR_MOVECURSOR_JUSTIFY( mc )
//! \brief \ref mcr_MoveCursor_set_justify
# define MCR_MOVECURSOR_SET_JUSTIFY( mc, enable )
// mcr_Scroll
//! \brief \ref mcr_Scroll_dimensions
# define MCR_SCROLL_DIMENSIONS( scroll, buffer )
//! \brief \ref mcr_Scroll_set_dimensions
# define MCR_SCROLL_SET_DIMENSIONS( scroll, buffer )
//! \brief \ref mcr_Scroll_coordinate
# define MCR_SCROLL_COORDINATE( scroll, posType )
//! \brief \ref mcr_Scroll_set_coordinate
# define MCR_SCROLL_SET_COORDINATE( scroll, posType, val )

//
// Inline send macros.
//
//! \brief \ref mcr_Alarm_send
# define MCR_ALARM_SEND( alarm ) \
( thrd_sleep_until ( & alarm ) == thrd_success )
//! \brief \ref mcr_Alarm_send
# define MCR_ALARM_QUICKSEND( alarm ) \
( thrd_sleep_until ( & alarm ) )
//! \brief \ref mcr_Echo_send
# define MCR_ECHO_SEND( echo, success )
//! \brief \ref mcr_Echo_send
# define MCR_ECHO_QUICKSEND( echo )
//! \brief \ref mcr_Key_send
# define MCR_KEY_SEND( key, success )
//! \brief \ref mcr_Key_send
# define MCR_KEY_QUICKSEND( key )
//! \brief \ref mcr_MoveCursor_send
# define MCR_MOVECURSOR_SEND( mover, success )
//! \brief \ref mcr_MoveCursor_send
# define MCR_MOVECURSOR_QUICKSEND( mover )
//! \brief \ref mcr_NoOp_send
# define MCR_NOOP_SEND( noop ) \
( thrd_sleep ( & noop, NULL ) == thrd_success )
//! \brief \ref mcr_NoOp_send
# define MCR_NOOP_QUICKSEND( noop ) \
( thrd_sleep ( & noop, NULL ) )
//! \brief \ref mcr_Scroll_send
# define MCR_SCROLL_SEND( scroll, success )
//! \brief \ref mcr_Scroll_send
# define MCR_SCROLL_QUICKSEND( scroll )
# endif

# include MCR_STR(signal/MCR_NATIVE_DIR/standard.h)

# endif // MCR_STANDARD_H
