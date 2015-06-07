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
	mcr_Interface interface ;
	//
	// Internal
	//
	/*! \brief Function to intercept before sending. Returns 0
	 * to not block signals from sending, otherwise do block the signal.
	 **/
	mcr_signal_fnc dispatch ;
	void * dispatch_object ;
	//! \brief Function to send, when not interrupted.
	mcr_signal_fnc send ;
} mcr_ISignal ;

/*!
 * \brief Use functions in \ref mcr_ISignal to dispatch and
 * send the whole object, ISignal + data members.
 * */
typedef struct mcr_Signal
{
	//
	// Interface
	//
	//! \brief Signal interface, or vtable.
	mcr_ISignal * type ;
	//
	// Instance
	//
	mcr_Data data ;
} mcr_Signal ;

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
 * \param isHeap 0 if data stack reference, otherwise allocated on heap.
 * */
MCR_API void mcr_Signal_init_with ( mcr_Signal * sigPt,
		mcr_ISignal * type, void * data, int isHeap ) ;
/*! \brief If ISignal has data free function, that is used. If data is
 * heap allocated, then free will also be called on data.
 * */
MCR_API void mcr_Signal_free ( mcr_Signal * sigPt ) ;
MCR_API void mcr_Signal_free_foreach ( mcr_Signal * sigPt, ... ) ;
/*!
 * \brief mcr_send
 * Functions are taken from object's type, and signalData
 * is taken from signal's data.
 *
 * \return 0 on failure, otherwise successful.
 * */
MCR_API int mcr_send ( mcr_Signal * sigPt ) ;

//
// Some helpful things and abstractions.
//
MCR_API void * mcr_ISignal_mkdata_data ( mcr_ISignal * isigPt ) ;
MCR_API mcr_Data mcr_ISignal_mkdata ( mcr_ISignal * isigPt ) ;
MCR_API void mcr_Signal_copy ( mcr_Signal * dstPt, mcr_Signal * srcPt ) ;
MCR_API int mcr_Signal_compare ( const void * lhsSigPt,
		const void * rhsSigPt ) ;

//
// Signal development: Add signal type to end, get signal type,
//
MCR_API void mcr_Signal_load_contract ( ) ;
/*!
 * \brief mcr_ISignal ctor, Set required members of new type,
 * except the dispatcher, which is not modified.
 *
 * \param newType New signal interface to initialize.
 * \param sender Send function.
 * \param dataSize Byte size of data members.
 * */
MCR_API void mcr_ISignal_init ( mcr_ISignal * newType,
		mcr_signal_fnc sender, size_t dataSize ) ;
/*!
 * \brief mcr_ISignal ctor, Set all members of new type.
 *
 * \param newType New signal interface to initialize.
 * */
MCR_API void mcr_ISignal_init_with ( mcr_ISignal * newType,
		mcr_compare_fnc compare,
		void ( * copy ) ( void *, void * ),
		size_t dataSize, void ( * init ) ( void * ),
		void ( * free ) ( void * ),
		mcr_signal_fnc dispatch, mcr_signal_fnc sender ) ;
/*!
 * \brief Id is set as a unique value.
 *
 * \param newType The signal interface to register as new ISignal type.
 * \return Value of id for new type. -1 if not successful.
 * */
MCR_API size_t mcr_ISignal_register ( mcr_ISignal * newType ) ;
MCR_API size_t mcr_ISignal_register_with ( mcr_ISignal * newType,
		const char * name, const char ** addNames,
		size_t bufferLen ) ;
/*! \brief Get the \ref mcr_ISignal for given id.
 *
 * \param typeId Id of given \ref mcr_ISignal
 * \return Pointer to Signal interface. NULL of id is not registered.
 * */
MCR_API mcr_ISignal * mcr_ISignal_get ( size_t typeId ) ;
/*! \brief Get the ISignal from the name of signal type.
 *
 * \param typeName Name of given \ref mcr_ISignal.
 * \return Pointer to signal interface. NULL if not found.
 * */
MCR_API mcr_ISignal * mcr_ISignal_from_name ( const char * typeName ) ;
/*! \brief Get the id of signal interface from a signal name.
 *
 * \param typeName Name of given \ref mcr_ISignal.
 * \return Id of named signal type, or -1 if not found.
 * */
MCR_API size_t mcr_ISignal_get_id ( const char * typeName ) ;
/*! \brief Get the signal name from id of signal interface.
 * */
MCR_API const char * mcr_ISignal_get_name ( size_t id ) ;
/*!
 * \brief Signal to a name, and vice versa.
 *
 * \param id Id of signal to set name for.
 * \param name This name will be mapped to the signal.
 * \return 0 on failure, otherwise success.
 * */
MCR_API int mcr_ISignal_set_name ( mcr_ISignal * sigPt,
		const char * name ) ;
/*!
 * \brief Map an additional name to this signal.
 *
 * \param sigPt Pointer to signal to add name for.
 * \param addName This name will be mapped to the signal.
 * \return 0 on failure, otherwise success.
 * */
MCR_API int mcr_ISignal_add_name ( mcr_ISignal * sigPt,
		const char * name ) ;
MCR_API int mcr_ISignal_add_names ( mcr_ISignal * sigPt,
		const char ** names, size_t bufferLen ) ;
MCR_API int mcr_ISignal_set_names ( mcr_ISignal * sigPt,
		const char * name,
		const char ** extraNames, size_t bufferLen ) ;
/*!
 * \brief Reset the mapped name for a signal. First that signal
 * will be found by the old name.
 *
 * This will also set the name member, \ref mcr_ISignal#name.
 * \param oldName Name to search for signal to replace name for.
 * \return 0 on failure, otherwise success.
 * */
MCR_API int mcr_ISignal_rename ( const char * oldName,
		const char * newName ) ;
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
MCR_API void mcr_ISignal_clear ( ) ;

/*!
 * \brief Initialize containers for \ref mcr_ISignal registration.
 * */
MCR_API void mcr_signal_initialize ( ) ;
MCR_API void mcr_signalreg_initialize ( ) ;
/*!
 * \brief Free used memory of registered \ref mcr_ISignal set.
 * */
MCR_API void mcr_signal_cleanup ( void ) ;
MCR_API void mcr_signalreg_cleanup ( void ) ;

# define MCR_SIGNAL_INIT( signalPt, typePt, dataPt, isHeap ) \
	( signalPt )->type = ( typePt ) ; \
	( signalPt )->data.data = ( dataPt ) ; \
	( signalPt )->data.is_heap = ( isHeap ) ;

# define MCR_SIGNAL_FREE( sigPt ) \
	MCR_IFREE ( & ( sigPt )->type->interface, & ( sigPt )->data ) ;

# define MCR_ISIGNAL_MKDATA( isigPt, dataPtOut ) \
	MCR_IMKDATA ( & ( isigPt )->interface, dataPtOut ) ;

# define MCR_SIGNAL_MKDATA( sigPt, dataPtOut ) \
	MCR_IMKDATA ( & ( sigPt )->type->interface, dataPtOut ) ;

# define MCR_SIGNAL_COPY( dstPt, srcPt ) \
	dassert ( ( srcPt )->type ) ; \
	if ( ( dstPt )->type && ( dstPt )->type \
			!= ( srcPt )->type ) \
	{ \
		MCR_SIGNAL_FREE ( dstPt ) ; \
	} \
	( dstPt )->type = ( srcPt )->type ; \
	MCR_ICPY ( & ( srcPt )->type->interface, & ( dstPt )->data, \
			& ( srcPt )->data ) ;

# define MCR_SIGNAL_CMP( lSigPt, rSigPt ) \
	( ( lSigPt )->type != ( rSigPt )->type ? \
		( lSigPt )->type < ( rSigPt )->type ? \
			-1 : \
		1 : \
	( lSigPt )->type ? \
		MCR_ICMP ( & ( lSigPt )->type->interface, & ( lSigPt )->data, \
				& ( rSigPt )->data ) : \
	memcmp ( & ( lSigPt )->data.data, \
			& ( rSigPt )->data.data, sizeof ( void * ) ) )

/*!
 * \brief Inline: 1 ) dispatch specific, if not blocking
 * 2 ) dispatch unspecific, if not blocking
 * 3 ) send signal.
 *
 * \param signalPt mcr_Signal *
 * */
# define MCR_SEND( signalPt ) \
	( ( signalPt )->type->dispatch && \
			( signalPt )->type->dispatch ( signalPt ) ? \
		1 : \
	( signalPt )->type->send ( ( signalPt ) ) ) \

/*!
 * \brief MCR_SEND with array of pointers to mcr_Signal
 *
 * \param signalPtArray mcr_Signal * [ ]
 * \param arrLen size_t Length of signalPtArray.
 * \param success int Set to 0 on failure.
 * */
# define MCR_SEND_REF_ARRAY( signalPtArray, arrLen, success ) \
	for ( size_t _arrayIterator_ = 0 ; _arrayIterator_ < ( arrLen ) ; \
			_arrayIterator_ ++ ) \
	{ \
		if ( ! MCR_SEND ( ( signalPtArray ) [ _arrayIterator_ ] ) ) \
			( success ) = 0 ; \
	}

# define MCR_SEND_ARRAY( signalArray, arrLen, success ) \
	for ( size_t _arrayIterator_ = 0 ; _arrayIterator_ < ( arrLen ) ; \
			_arrayIterator_ ++ ) \
	{ \
		if ( ! MCR_SEND ( ( signalPtArray ) + _arrayIterator_ ) ) \
			( success ) = 0 ; \
	}

/*!
 * \brief For pointer to mcr_ISignal set data size, send,
 *
 * Should we reset id to 0?
 * \param isignalPt mcr_ISignal * Object to set values into.
 * \param dataSize size_t mcr_ISignal data_size.
 * \param sender mcr_signal_fnc mcr_ISignal send.
 * */
# define MCR_ISIGNAL_INIT( isignalPt, comparison, copier, \
		dataSize, initializer, freer, sender ) \
	MCR_IINIT ( & ( isignalPt )->interface, comparison, copier, \
			dataSize, initializer, freer ) ; \
	( isignalPt )->send = ( sender ) ;

# endif
