/*! \file signal/isignal.h
 * \brief mcr_ISignal interface for signals.
 * mcr_Signal Pair of mcr_ISignal * and void *. Use with \ref mcr_send.
 * */

# ifndef MCR_ISIGNAL_H
# define MCR_ISIGNAL_H

# include "signal/def.h"

//
// Types
//
//! \brief mcr_ISignal * signal vtable + void * data
typedef struct mcr_Signal mcr_Signal ;
/*! \brief Function to send or dispatch signal.
 *
 * \param signalData mcr_Signal * Signal to send or dispatch.
 * \return Value is interpreted as success or blocked intercept.
 * */
typedef int ( * mcr_signal_fnc ) ( mcr_Signal * signalData ) ;

/*! \brief Common members for unique signal type. Signal vtable.
 *
 * Logically a signal should be anything that can be "dispatched"
 * to another entity, or "sent" to cause an action. <br>
 * id - size_t Unique identifier for this signal type. <br>
 * name - const char * Case insensitive signal name. <br>
 * send - mcr_signal_fnc Function to send, when not interrupted. <br>
 * dispatch - mcr_signal_fnc Function to intercept before sending.
 * Returns 0 to not block signals from sending, otherwise do block
 * the signal. <br>
 * data_size - size_t Byte size of a data element. <br>
 *
 * */
typedef struct mcr_ISignal
{
	size_t id ;
	const char * name ;
	mcr_signal_fnc send ;
	mcr_signal_fnc dispatch ;
	size_t data_size ;
} mcr_ISignal ;

/*!
 * \brief Use functions in \ref mcr_ISignal to dispatch and
 * send the whole object, ISignal + data members.
 *
 * type - mcr_ISignal * Signal vtable ;
 * data - void * Information to send.
 *
 * */
typedef struct mcr_Signal
{
	mcr_ISignal * type ;
	void * data ;
} mcr_Signal ;

/*!
 * \brief Dispatcher used for all signals, after specific is used.
 *
 * In order to inline, we cannot privatize generic dispatcher.
 * */
extern mcr_signal_fnc mcr_AllDispatch ;

//
// General use: initializer and send function.
//
/*!
 * \brief mcr_Signal_init Signal ctor
 *
 * \param sigPt mcr_Signal *
 * \param type mcr_ISignal * Signal vtable.
 * */
MCR_API void mcr_Signal_init ( mcr_Signal * sigPt, mcr_ISignal * type ) ;
/*! \brief \ref mcr_Signal_init with data member included.
 *
 * \param sigPt mcr_Signal *
 * \param type mcr_ISignal * Signal vtable.
 * \param data void * Information to send.
 * */
MCR_API void mcr_Signal_init_with ( mcr_Signal * sigPt, mcr_ISignal * type,
		void * data ) ;
/*!
 * \brief mcr_send
 * Functions are taken from object's type, and signalData
 * is taken from signal's data.
 *
 * \param sigPt mcr_Signal *
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
 * \param newType mcr_ISignal * to initialize.
 * \param name const char * Name of type.
 * \param sender mcr_signal_fnc Send function.
 * \param dataSize size_t Byte size of data members.
 * */
MCR_API void mcr_ISignal_init ( mcr_ISignal * newType, const char * name,
		mcr_signal_fnc sender, size_t dataSize ) ;
/*!
 * \brief mcr_ISignal_register Id is set as a unique value.
 *
 * \param newType mcr_ISignal * to register as new ISignal type.
 * \return size_t Value of id for new type. -1 if not successful.
 * */
MCR_API size_t mcr_ISignal_register ( mcr_ISignal * newType ) ;
/*! \brief \return Get the mcr_ISignal for given id.
 * NULL if it does not exist.
 *
 * \param typeId Id of given mcr_ISignal
 * */
MCR_API mcr_ISignal * mcr_ISignal_get ( size_t typeId ) ;
/*! \brief \return Get the id of signal type's ISignal from signal name.
 *
 * \param typeName const char * name of given mcr_ISignal.
 * */
MCR_API size_t mcr_ISignal_get_id ( const char * typeName ) ;
/*! \brief \return Get the ISignal from the name of signal type.
 *
 * \param typeName const char * name of given mcr_ISignal.
 * */
MCR_API mcr_ISignal * mcr_ISignal_from_name ( const char * typeName ) ;
//! \brief \return Count of all ISignal references registered.
MCR_API size_t mcr_ISignal_count ( ) ;
/*! \brief Copy into buffer pointers registered ISignals starting
 * with id 0.
 *
 * \param buffer mcr_ISignal ** Container to copy all pointers to
 * mcr_ISignals.
 * \param bufferSize size/length of registered mcr_ISignals.
 * */
MCR_API void mcr_ISignal_get_all ( mcr_ISignal ** buffer,
		size_t bufferLength ) ;
/*! \brief compare const c-strings referenced by each pointer,
 * case insensitive.
 *
 * \param lhs const void * Pointer to const char *
 * \param rhs const void * Pointer to const char *
 * \return return < 0 if lhs < rhs, return 0 if lhs == rhs,
 * return > 0 if lhs > rhs.
 * */
MCR_API int mcr_name_compare ( const void * lhs, const void * rhs ) ;
/*! \brief compare integers referenced by each pointer.
 *
 * \param lhs const void * Pointer to int.
 * \param rhs const void * Pointer to int.
 * \return return < 0 if lhs < rhs, return 0 if lhs == rhs,
 * return > 0 if lhs > rhs.
 * */
MCR_API int mcr_int_compare ( const void * lhs, const void * rhs ) ;
/*! \brief compare unsigned integers referenced by each pointer.
 *
 * \param lhs const void * Pointer to unsigned int.
 * \param rhs const void * Pointer to unsigned int.
 * \return return < 0 if lhs < rhs, return 0 if lhs == rhs,
 * return > 0 if lhs > rhs.
 * */
MCR_API int mcr_unsigned_compare ( const void * lhs, const void * rhs ) ;
/*! \brief compare size_t referenced by each pointer.
 *
 * \param lhs const void * Pointer to size_t.
 * \param rhs const void * Pointer to size_t.
 * \return return < 0 if lhs < rhs, return 0 if lhs == rhs,
 * return > 0 if lhs > rhs.
 * */
MCR_API int mcr_size_t_compare ( const void * lhs, const void * rhs ) ;
/*! \brief compare void * referenced by each pointer.
 *
 * \param lhs const void * Pointer to void *.
 * \param rhs const void * Pointer to void *.
 * \return return < 0 if lhs < rhs, return 0 if lhs == rhs,
 * return > 0 if lhs > rhs.
 * */
MCR_API int mcr_ref_compare ( const void * lhs, const void * rhs ) ;

/*!
 * \brief Free used memory of registered mcr_ISignals.
 * */
MCR_API void mcr_signal_cleanup ( ) ;
/*!
 * \brief Initialize containers for mcr_ISignal registration.
 * */
MCR_API void mcr_signal_initialize ( ) ;

# endif
