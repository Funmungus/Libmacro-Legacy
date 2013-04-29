/*! \file signal/standard.h
 * \brief mcr_Signal data types : mcr_Alarm, mcr_HIDEcho, mcr_Key,
 * mcr_MoveCursor, mcr_NoOp, and mcr_Scroll. <br>
 * mcr_ISignals : mcr_IAlarm, mcr_IHIDEcho, mcr_IKey,
 * mcr_IMoveCursor, mcr_INoOp, and mcr_IScroll.
 *
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
 * time_point - tm. <br>
 * tm int members : tm_sec 0-60, tm_min 0-59,
 * tm_hour 0-23, tm_mday 1-31, tm_mon 0-11, tm_year, tm_wday 0-6,
 * tm_yday 0-365, tm_isdst ( daylight savings ).
 * */
typedef struct mcr_Alarm
{
	struct tm time_point ;
} mcr_Alarm ;
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
 * tv - timespec. <br>
 * timespec members : tv_sec, tv_nsec.
 * */
typedef struct mcr_NoOp
{
	struct timespec tv ;
} mcr_NoOp ;
// Scroll
//! \brief Scroll through pages/visible area.
typedef struct mcr_Scroll mcr_Scroll ;


//
// Standard signal ISignals, i.e. type definition, or vtable.
//
/*! \brief ISignal for Alarm. */
extern mcr_ISignal mcr_IAlarm ;
/*! \brief ISignal for HIDEcho keys. */
extern mcr_ISignal mcr_IHIDEcho ;
/*! \brief ISignal for keyboard keys. */
extern mcr_ISignal mcr_IKey ;
/*! \brief ISignal for MoveCursor. */
extern mcr_ISignal mcr_IMoveCursor ;
/*! \brief ISignal for NoOp keys. */
extern mcr_ISignal mcr_INoOp ;
/*! \brief ISignal for Scroll. */
extern mcr_ISignal mcr_IScroll ;


//
// mcr_HIDEcho echo code memory.
//
/*!
 * \brief mcr_Echo_code Get event code from its name.
 *
 * \param eventName const char * Name of event code.
 * \return int event code. -1 if it is not registered.
 * */
MCR_API int mcr_Echo_code ( const char * eventName ) ;
/*!
 * \brief mcr_Echo_name Get name of event code.
 *
 * \param eventCode int
 * \return const char * name of event code.
 * */
MCR_API const char * mcr_Echo_name ( int eventCode ) ;
/*!
 * \brief \return Count of registered echo codes.
 * */
MCR_API size_t mcr_Echo_count ( ) ;
/*!
 * \brief mcr_Echo_register
 *
 * \param eventName const char * Name of event code.
 * \return int Value of event code, or -1 if not successful.
 * */
MCR_API int mcr_Echo_register ( const char * eventName ) ;


//
// ctors for native types
//
/*! \brief Echo ctor.
 *
 * \param echoPt mcr_HIDEcho *
 * */
MCR_API void mcr_Echo_init ( mcr_HIDEcho * echoPt ) ;
/*! \brief Echo ctor with initial data.
 *
 * \param echoPt mcr_HIDEcho *
 * \param event const int Echo event code.
 * */
MCR_API void mcr_Echo_init_with ( mcr_HIDEcho * echoPt, const int event ) ;
/*! \brief Key ctor.
 *
 * \param keyPt mcr_Key *
 * */
MCR_API void mcr_Key_init ( mcr_Key * keyPt ) ;
/*! \brief Key ctor with initial data
 *
 * \param keyPt mcr_Key *
 * \param key const int Key code.
 * \param scan const int Scan code.
 * \param keyUp mcr_KeyUpType
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
 * \param mcPt mcr_MoveCursor *
 * \param blip const mcr_SpacePosition Spatial coordinates of movement.
 * \param cursorJustify const int 0 absolute, else relative.
 * */
MCR_API void mcr_MoveCursor_init_with ( mcr_MoveCursor * mcPt,
		const mcr_SpacePosition blip, const int cursorJustify ) ;
/*! \brief Scroll ctor
 *
 * \param scrollPt mcr_Scroll *
 * */
MCR_API void mcr_Scroll_init ( mcr_Scroll * scrollPt ) ;
/*! \brief Scroll ctor with initial data.
 *
 * \param scrollPt mcr_Scroll *
 * \param dimVals const mcr_Dimensions Spatial coordinates to
 * scroll through.
 * */
MCR_API void mcr_Scroll_init_with ( mcr_Scroll * scrollPt,
		const mcr_Dimensions dimVals ) ;


//
// Native data manipulation.
//
// HIDEcho
/*! \brief \return int Echo event code.
 *
 * \param echoPt const mcr_HIDEcho *
 * */
MCR_API int mcr_Echo_get ( const mcr_HIDEcho * echoPt ) ;
/*! \brief Set echo code.
 *
 * \param echoPt mcr_HIDEcho *
 * \param echo int Echo event code.
 * */
MCR_API void mcr_Echo_set ( mcr_HIDEcho * echoPt, int echo ) ;
// Key
/*! \brief \return int Key code.
 *
 * \param keyPt const mcr_Key *
 * */
MCR_API int mcr_Key_get ( const mcr_Key * keyPt ) ;
/*! \brief Set key code.
 *
 * \param keyPt mcr_Key *
 * \param key int Key code to set.
 * */
MCR_API void mcr_Key_set ( mcr_Key * keyPt, int key ) ;
/*! \brief \return int Scan code.
 *
 * \param keyPt const mcr_Key *
 * */
MCR_API int mcr_Key_get_scan ( const mcr_Key * keyPt ) ;
/*! \brief Set scan code.
 *
 * \param keyPt mcr_Key *
 * \param scan int Scan code.
 * */
MCR_API void mcr_Key_set_scan ( mcr_Key * keyPt, int scan ) ;
/*! \brief \return mcr_KeyUpType Key up type.
 *
 * \param keyPt const mcr_Key *
 * */
MCR_API mcr_KeyUpType mcr_Key_get_up_type ( const mcr_Key * keyPt ) ;
/*! \brief Set key up type.
 *
 * \param keyPt mcr_Key *
 * \param keyUp const mcr_KeyUpType
 * */
MCR_API void mcr_Key_set_up_type ( mcr_Key * keyPt,
		const mcr_KeyUpType keyUp ) ;
// MoveCursor
/*! \brief Get the cursor spatial positions, copied into the buffer.
 *
 * \param mcPt const mcr_MoveCursor *
 * \param buffer mcr_SpacePosition Buffer to copy into.
 * */
MCR_API void mcr_MoveCursor_get ( const mcr_MoveCursor * mcPt,
		mcr_SpacePosition buffer ) ;
/*! \brief Set cursor spatial positions.
 *
 * \param mcPt mcr_MoveCursor *
 * \param pos const mcr_SpacePosition
 * */
MCR_API void mcr_MoveCursor_set ( mcr_MoveCursor * mcPt,
		const mcr_SpacePosition pos ) ;
/*! \brief \return Value of a single coordinate position.
 *
 * \param mcPt const mcr_MoveCursor *
 * \param coordinate int x, y, z coordinate type.
 * */
MCR_API long long mcr_MoveCursor_get_position (
		const mcr_MoveCursor * mcPt, int coordinate ) ;
/*! \brief Set the position of one coordinate.
 *
 * \param mcPt mcr_MoveCursor *
 * \param coordinate int Coordinate type.
 * \param value long long Coordinate value to set.
 * */
MCR_API void mcr_MoveCursor_set_position ( mcr_MoveCursor * mcPt,
		int coordinate, long long value ) ;
/*! \brief \return int Cursor justification. 0 is absolute, otherwise
 * cursor is justified.
 *
 * \param mcPt const mcr_MoveCursor *
 * */
MCR_API int mcr_MoveCursor_is_justify ( const mcr_MoveCursor * mcPt ) ;
/*! \brief Set cursor justification.
 *
 * \param mcPt mcr_MoveCursor *
 * \param cursorJustify int 0 absolute, otherwise cursor is justified.
 * */
MCR_API void mcr_MoveCursor_enable_justify ( mcr_MoveCursor * mcPt,
		int cursorJustify ) ;
// Scroll
/*! \brief Set all coordinate dimensions.
 *
 * \param scrollPt mcr_Scroll *
 * \param pos const mcr_Dimensions Values to set from.
 * */
MCR_API void mcr_Scroll_set ( mcr_Scroll * scrollPt,
		const mcr_Dimensions pos ) ;
/*! \brief All coordinate dimensions are copied into the buffer.
 *
 * \param scrollPt const mcr_Scroll *
 * \param buffer mcr_Dimensions Values are copied here.
 * */
MCR_API void mcr_Scroll_get ( const mcr_Scroll * scrollPt,
		mcr_Dimensions buffer ) ;
/*! \brief Set a single coordinate dimension.
 *
 * \param scrollPt mcr_Scroll *
 * \param coordinate int Coordinate type.
 * \param value long long Coordinate value to set.
 * */
MCR_API void mcr_Scroll_set_dimension ( mcr_Scroll * scrollPt,
		int coordinate, long long value ) ;
/*! \brief \return long long Value of a single coordinate dimension.
 *
 * \param scrollPt mcr_Scroll *
 * \param coordinate int x, y, z coordinate type
 * */
MCR_API long long mcr_Scroll_get_dimension ( mcr_Scroll * scrollPt,
		int coordinate ) ;

//
// Send functions that are placed into ISignals.
//
/*!
 * \brief Pause execution until time point.
 *
 * \param signalData mcr_Signal * has data member mcr_Alaram or struct tm.
 * \return int 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Alarm_send ( mcr_Signal * signalData ) ;
/*!
 * \brief Send human interface impersonation.
 *
 * \param signalData mcr_Signal * has data member mcr_HIDEcho.
 * \return int 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Echo_send ( mcr_Signal * signalData ) ;
/*!
 * \brief Simulate keyboard keys.
 *
 * \param signalData mcr_Signal * has data member mcr_Key.
 * \return int 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Key_send ( mcr_Signal * signalData ) ;
/*!
 * \brief Move HID cursor position.
 *
 * \param signalData mcr_Signal * has data member mcr_MoveCursor.
 * \return int 0 on failure, otherwise successful.
 * */
MCR_API int mcr_MoveCursor_send ( mcr_Signal * signalData ) ;
/*!
 * \brief Pause execution in seconds and nanoseconds.
 *
 * \param signalData mcr_Signal * has data member mcr_NoOp
 * or struct timespec.
 * \return int 0 on failure, otherwise successful.
 * */
MCR_API int mcr_NoOp_send ( mcr_Signal * signalData ) ;
/*!
 * \brief Scroll through pages/visible area.
 *
 * \param signalData mcr_Signal * has data member mcr_Scroll.
 * \return int 0 on failure, otherwise successful.
 * */
MCR_API int mcr_Scroll_send ( mcr_Signal * signalData ) ;


//
// Some extra, possibly useful, utilities.
//
/*! mcr_resembles_justified
 * \brief For both positions, either may be 0, or the same array member
 * must either be both negative, or both positive.
 *
 * \param first const \ref mcr_Dimensions.
 * \param second const \ref mcr_Dimensions.
 * \return int 0 false, else true.
 * */
MCR_API int mcr_resembles_justified ( const mcr_Dimensions first,
		const mcr_Dimensions second ) ;
/*! mcr_resembles_absolute
 * \brief For both positions, the same array member must be valued within
 * the measurementError.
 *
 * \param first const \ref mcr_Dimensions.
 * \param second const \ref mcr_Dimensions.
 * \param measurementError int.
 * \return 0 false, else true.
 * */
MCR_API int mcr_resembles_absolute ( const mcr_Dimensions first,
		const mcr_Dimensions second,
		const unsigned int measurementError ) ;
/*!
 * \brief If justified then \ref mcr_resembles_justified,
 * else \ref mcr_resembles_absolute
 *
 * \param lhs const mcr_MoveCursor *
 * \param rhs const mcr_MoveCursor *
 * \param measurementError const unsigned int only used in
 * \ref mcr_resembles_absolute.
 * \return int 0 false, else true.
 * */
MCR_API int mcr_resembles ( const mcr_MoveCursor * lhs,
		const mcr_MoveCursor * rhs,
		const unsigned int measurementError ) ;

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
/*! \brief \return int Echo event code.
 *
 * \param echoPt mcr_HIDEcho *
 * */
# define MCR_ECHO_GET( echoPt )
/*! \brief Set echo code.
 *
 * \param echoPt mcr_HIDEcho *
 * \param echo int Echo event code.
 * */
# define MCR_ECHO_SET( echoPt, echo )
// mcr_Key
/*! \brief \return int Key code.
 *
 * \param keyPt mcr_Key *
 * */
# define MCR_KEY_GET( keyPt )
/*! \brief Set key code.
 *
 * \param keyPt mcr_Key *
 * \param key int Key code to set.
 * */
# define MCR_KEY_SET( keyPt, key )
/*! \brief \return int Scan code.
 *
 * \param keyPt mcr_Key *
 * */
# define MCR_KEY_GET_SCAN( keyPt )
/*! \brief Set scan code.
 *
 * \param keyPt mcr_Key *
 * \param scan int Scan code.
 * */
# define MCR_KEY_SET_SCAN( keyPt, scan )
/*! \brief \return mcr_KeyUpType Key up type.
 *
 * \param keyPt mcr_Key *
 * */
# define MCR_KEY_GET_UP_TYPE( keyPt )
/*! \brief Set key up type.
 *
 * \param keyPt mcr_Key *
 * \param keyUp mcr_KeyUpType
 * */
# define MCR_KEY_SET_UP_TYPE( keyPt, keyUp )
// mcr_MoveCursor
/*! \brief Set cursor spatial positions.
 *
 * \param mcPt mcr_MoveCursor *
 * \param buffer mcr_SpacePosition
 * */
# define MCR_MOVECURSOR_GET( mcPt, buffer )
/*! \brief Get the cursor spatial positions, copied into the buffer.
 *
 * \param mcPt mcr_MoveCursor *
 * \param buffer mcr_SpacePosition Buffer to copy into.
 * */
# define MCR_MOVECURSOR_SET( mcPt, buffer )
/*! \brief \return Value of a single coordinate position.
 *
 * \param mcPt mcr_MoveCursor *
 * \param posType int x, y, z coordinate type.
 * */
# define MCR_MOVECURSOR_GET_POSITION( mcPt, posType )
/*! \brief Set the position of one coordinate.
 *
 * \param mcPt mcr_MoveCursor *
 * \param posType int Coordinate type.
 * \param val long long Coordinate value to set.
 * */
# define MCR_MOVECURSOR_SET_POSITION( mcPt, posType, val )
/*! \brief \return int Cursor justification. 0 is absolute, otherwise
 * cursor is justified.
 *
 * \param mcPt mcr_MoveCursor *
 * */
# define MCR_MOVECURSOR_IS_JUSTIFY( mcPt )
/*! \brief Set cursor justification.
 *
 * \param mcPt mcr_MoveCursor *
 * \param enable int 0 absolute, otherwise cursor is justified.
 * */
# define MCR_MOVECURSOR_ENABLE_JUSTIFY( mcPt, enable )
// mcr_Scroll
/*! \brief All coordinate dimensions are copied into the buffer.
 *
 * \param scrollPt mcr_Scroll *
 * \param buffer mcr_Dimensions Values are copied here.
 * */
# define MCR_SCROLL_GET( scrollPt, buffer )
/*! \brief Set all coordinate dimensions.
 *
 * \param scrollPt mcr_Scroll *
 * \param buffer mcr_Dimensions Values to set from.
 * */
# define MCR_SCROLL_SET( scrollPt, buffer )
/*! \brief \return long long Value of a single coordinate dimension.
 *
 * \param scrollPt mcr_Scroll *
 * \param posType int x, y, z coordinate type
 * */
# define MCR_SCROLL_GET_DIMENSION( scrollPt, posType )
/*! \brief Set a single coordinate dimension.
 *
 * \param scrollPt mcr_Scroll *
 * \param posType int Coordinate type.
 * \param val long long Coordinate value to set.
 * */
# define MCR_SCROLL_SET_DIMENSION( scrollPt, posType, val )

//
// Inline send macros.
//
/*!
 * \brief Pause execution until time point.
 *
 * \param alarmPt mcr_Alarm *
 * \param success int Set to 0 on failure.
 * */
# define MCR_ALARM_SEND( alarmPt, success ) \
	if ( thrd_sleep_until ( ( struct tm * ) alarmPt ) != thrd_success ) \
		success = 0 ;
/*!
 * \brief Send human interface impersonation.
 *
 * \param echoPt mcr_HIDEcho *
 * \param success int Set to 0 on failure.
 * */
# define MCR_ECHO_SEND( echoPt, success )
/*!
 * \brief Simulate keyboard keys.
 *
 * \param keyPt mcr_Key *
 * \param success int Set to 0 on failure.
 * */
# define MCR_KEY_SEND( keyPt, success )
/*!
 * \brief Move HID cursor position.
 *
 * \param movePt mcr_MoveCursor *
 * \param success int Set to 0 on failure.
 * */
# define MCR_MOVECURSOR_SEND( movePt, success )
/*!
 * \brief Pause execution in seconds and nanoseconds.
 *
 * \param noopPt mcr_NoOp *
 * \param success int Set to 0 on failure.
 * */
# define MCR_NOOP_SEND( noopPt, success ) \
	if ( thrd_sleep ( ( struct timespec * ) noopPt, NULL ) != thrd_success ) \
		success = 0 ;
/*!
 * \brief Scroll through pages/visible area.
 *
 * \param scrollPt mcr_Scroll *
 * \param success int Set to 0 on failure.
 * */
# define MCR_SCROLL_SEND( scrollPt, success )


# include STRINGIFY(signal/MCR_NATIVE_DIR/standard.h)

# endif // MCR_STANDARD_H
