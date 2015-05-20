/* include/signal/isignal.h - Interface type for signals.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file signal/isignal.h
 * \brief Interface for signals, and signal structure of
 * interface and data. Use signal with \ref mcr_send.
 * */

# ifndef MCR_ISIGNAL_H
# define MCR_ISIGNAL_H

# include "signal/def.h"

//
// Types
//
//! \brief Pair of signal interface and data to send.
typedef struct mcr_Signal mcr_Signal ;
/*! \brief Function to send or dispatch signal.
 *
 * \param signalData Signal to send or dispatch.
 * \return Value is interpreted as success or to block intercept.
 * */
typedef int ( * mcr_signal_fnc ) ( mcr_Signal * signalData ) ;

/*! \brief Common members for unique signal type. Signal interface,
 * or vtable.
 *
 * Logically a signal should be anything that can be "dispatched"
 * to another entity, or "sent" to cause an action.
 * */
typedef struct mcr_ISignal
{
	//! \brief Unique identifier for this signal type.
	size_t id ;
	//
	// Internal
	//
	//! \brief Byte size of a data element.
	size_t data_size ;
	/*! \brief Function to intercept before sending. Returns 0
	 * it not block signals from sending, otherwise do block the signal.
	 **/
	mcr_signal_fnc dispatch ;
	//! \brief Case insensitive signal name.
	const char * name ;
	//! \brief Function to send, when not interrupted.
	mcr_signal_fnc send ;
} mcr_ISignal ;

/*!
 * \brief Use functions in \ref mcr_ISignal to dispatch and
 * send the whole object, ISignal + data members.
 * */
typedef struct mcr_Signal
{
	//! \brief Signal interface, or vtable.
	mcr_ISignal * type ;
	//! \brief Extra information to send or dispatch.
	void * data ;
} mcr_Signal ;

/*!
 * \brief Dispatcher used for all signals, after specific is used.
 *
 * In order to inline, we cannot privatize generic dispatcher.
 * */
extern mcr_signal_fnc mcr_allDispatch ;

//
// General use: initializer and send function.
//
/*!
 * \brief mcr_Signal_init Signal ctor
 *
 * \param type Signal interface to set.
 * */
MCR_API void mcr_Signal_init ( mcr_Signal * sigPt, mcr_ISignal * type ) ;
/*! \brief \ref mcr_Signal_init with data member included.
 *
 * \param type Signal interface.
 * \param data Extra information to set.
 * */
MCR_API void mcr_Signal_init_with ( mcr_Signal * sigPt, mcr_ISignal * type,
		void * data ) ;
/*!
 * \brief mcr_send
 * Functions are taken from object's type, and signalData
 * is taken from signal's data.
 *
 * \return 0 on failure, otherwise successful.
 * */
MCR_API int mcr_send ( mcr_Signal * sigPt ) ;

//
// Signal development: Add signal type to end, get signal type,
//
/*!
 * \brief mcr_ISignal ctor, Set all members of new type,
 * except the dispatcher, which is not modified.
 *
 * \param newType New signal interface to initialize.
 * \param name Name of type.
 * \param sender Send function.
 * \param dataSize Byte size of data members.
 * */
MCR_API void mcr_ISignal_init ( mcr_ISignal * newType, const char * name,
		mcr_signal_fnc sender, size_t dataSize ) ;
/*!
 * \brief Id is set as a unique value.
 *
 * \param newType The signal interface to register as new ISignal type.
 * \return Value of id for new type. -1 if not successful.
 * */
MCR_API size_t mcr_ISignal_register ( mcr_ISignal * newType ) ;
/*!
 * \brief Map an additional name to this signal.
 *
 * \param sigPt Pointer to signal to add name for.
 * \param addName This name will be mapped to the signal.
 * \return 0 on failure, otherwise success.
 * */
MCR_API int mcr_ISignal_add_name ( mcr_ISignal * sigPt,
		const char * addName ) ;
/*!
 * \brief Reset the mapped name for a signal.
 *
 * This will also set the name member, \ref mcr_ISignal#name.
 * \param isigPt Pointer to signal to replace name for.
 * \param newName The value to put in \ref mcr_ISignal#name.
 * \return 0 on failure, otherwise success.
 * */
MCR_API int mcr_ISignal_rename ( mcr_ISignal * isigPt,
		const char * newName ) ;
/*!
 * \brief Reset the mapped name for a signal. First that signal
 * will be found by the old name.
 *
 * This will also set the name member, \ref mcr_ISignal#name.
 * \param oldName Name to search for signal to replace name for.
 * \return 0 on failure, otherwise success.
 * */
MCR_API int mcr_ISignal_rename_by_name ( const char * oldName,
		const char * newName ) ;
/*! \brief Get the \ref mcr_ISignal for given id.
 *
 * \param typeId Id of given \ref mcr_ISignal
 * \return Pointer to Signal interface. NULL of id is not registered.
 * */
MCR_API mcr_ISignal * mcr_ISignal_get ( size_t typeId ) ;
/*! \brief Get the id of signal interface from a signal name.
 *
 * \param typeName Name of given \ref mcr_ISignal.
 * \return Id of named signal type, or -1 if not found.
 * */
MCR_API size_t mcr_ISignal_get_id ( const char * typeName ) ;
/*! \brief Get the ISignal from the name of signal type.
 *
 * \param typeName Name of given \ref mcr_ISignal.
 * \return Pointer to signal interface. NULL if not found.
 * */
MCR_API mcr_ISignal * mcr_ISignal_from_name ( const char * typeName ) ;
//! \brief Get the number of all \ref mcr_ISignal references registered.
MCR_API size_t mcr_ISignal_count ( ) ;
/*! \brief Copy into buffer all registered mcr_ISignals starting
 * with id 0.
 *
 * \param buffer Container to copy all pointers to
 * \ref mcr_ISignals.
 * \param bufferLength Size/length of buffer that we may write to.
 * */
MCR_API void mcr_ISignal_get_all ( mcr_ISignal ** buffer,
		size_t bufferLength ) ;
/*! \brief Remove all registered \ref mcr_ISignal. */
MCR_API void mcr_ISignal_clear_all ( ) ;

//
// Comparisons
//
/*! \brief Compare const c-strings referenced by each pointer,
 * case insensitive.
 *
 * \param lhs Pointer to const char *
 * \param rhs Pointer to const char *
 * \return < 0 if lhs < rhs, 0 if lhs == rhs,
 * > 0 if lhs > rhs.
 * */
MCR_API int mcr_name_compare ( const void * lhs, const void * rhs ) ;
MCR_API int mcr_str_compare ( const void * lhs, const void * rhs ) ;
/*! \brief Compare integers referenced by each pointer.
 *
 * \param lhs Pointer to int.
 * \param rhs Pointer to int.
 * \return < 0 if lhs < rhs, 0 if lhs == rhs,
 * > 0 if lhs > rhs.
 * */
MCR_API int mcr_int_compare ( const void * lhs, const void * rhs ) ;
/*! \brief Compare unsigned integers referenced by each pointer.
 *
 * \param lhs Pointer to unsigned int.
 * \param rhs Pointer to unsigned int.
 * \return < 0 if lhs < rhs, 0 if lhs == rhs,
 * > 0 if lhs > rhs.
 * */
MCR_API int mcr_unsigned_compare ( const void * lhs, const void * rhs ) ;
/*! \brief Compare size_t referenced by each pointer.
 *
 * \param lhs Pointer to size_t.
 * \param rhs Pointer to size_t.
 * \return < 0 if lhs < rhs, 0 if lhs == rhs,
 * > 0 if lhs > rhs.
 * */
MCR_API int mcr_size_t_compare ( const void * lhs, const void * rhs ) ;
/*! \brief Compare void * referenced by each pointer.
 *
 * \param lhs Pointer to void *.
 * \param rhs Pointer to void *.
 * \return < 0 if lhs < rhs, 0 if lhs == rhs,
 * > 0 if lhs > rhs.
 * */
MCR_API int mcr_ref_compare ( const void * lhs, const void * rhs ) ;

/*!
 * \brief Free used memory of registered \ref mcr_ISignal set.
 * */
MCR_API void mcr_signal_cleanup ( ) ;
/*!
 * \brief Initialize containers for \ref mcr_ISignal registration.
 * */
MCR_API void mcr_signal_initialize ( ) ;

# endif
