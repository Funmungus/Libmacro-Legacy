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
//! \brief Pair of signal interface instance and data to send
typedef struct mcr_Signal mcr_Signal ;
/*! \brief Function to send or dispatch signal
 *
 * \param signalData Signal to send or dispatch
 * \return Boolean success or to block intercept
 * */
typedef int ( * mcr_signal_fnc ) ( mcr_Signal * signalData ) ;

/*! \brief Signal interface, common members for unique signal type
 *
 * Logically a signal should be anything that can be "dispatched"
 * to another entity, or "sent" to cause an action.
 * */
typedef struct mcr_ISignal
{
	//! \brief Common members for interface registration.
	mcr_Interface iface ;
	//
	// Internal
	//
	/*! \brief Function to intercept before sending. Returns 0
	 * to not block signals from sending, otherwise do block the signal.
	 * */
	mcr_signal_fnc dispatch ;
	/*! \brief Object which receives dispatch, and decides to block
	 * or not. */
	void * dispatch_object ;
	//! \brief Function to send, when not interrupted
	mcr_signal_fnc send ;
} mcr_ISignal ;

/*! \brief Use functions in \ref mcr_ISignal to dispatch and
 * send the whole object, ISignal + data members.
 * */
typedef struct mcr_Signal
{
	//
	// Interface
	//
	//! \brief Signal interface
	mcr_ISignal * type ;
	//
	// Instance
	//
	mcr_Data data ;
	//
	// Signal specific
	//
	/*! \brief 0 to not dispatch, otherwise do dispatch
	 *
	 * Default is 1 */
	int is_dispatch ;
} mcr_Signal ;

//
// General use: initializer and send function
//
/*! \brief ctor
 *
 * \param type Signal interface to set
 * */
MCR_API void mcr_Signal_init ( mcr_Signal * sigPt, mcr_ISignal * type ) ;
/*! \brief \ref mcr_Signal_init with data member included
 *
 * \param type Signal interface
 * \param data \ref mcr_Data#data
 * \param isHeap \ref mcr_Data#is_heap
 * \param isDispatch \ref mcr_Signal#is_dispatch
 * */
MCR_API void mcr_Signal_init_with ( mcr_Signal * sigPt,
		mcr_ISignal * type, void * data, int isHeap, int isDispatch ) ;
/*! \brief \ref mcr_ifree
 * */
MCR_API void mcr_Signal_free ( mcr_Signal * sigPt ) ;
//! \brief Variadic \ref mcr_Signal_free, unused extra arguments
# define mcr_Signal_free_foreach( sigPt, ... ) mcr_Signal_free ( sigPt )
/*!
 * \brief Functions are taken from object's type, and signalData
 * is taken from signal's data.
 *
 * \return 0 on failure, otherwise successful
 * */
MCR_API int mcr_send ( mcr_Signal * sigPt ) ;
//! \brief Variadic \ref mcr_send, unused extra arguments
# define mcr_send_foreach( sigPt, ... ) mcr_send ( sigPt )

//
// Some helpful things and abstractions.
//
//! \brief \ref mcr_mkdata_data
MCR_API void * mcr_ISignal_mkdata_data ( mcr_ISignal * isigPt ) ;
//! \brief \ref mcr_mkdata
MCR_API mcr_Data mcr_ISignal_mkdata ( mcr_ISignal * isigPt ) ;
/*! \brief \ref mcr_Signal#iface.id, or -1 for no isignal set
 * \return size_t */
# define mcr_Signal_id( sig ) \
	( ( sig ).type ? ( sig ).type->iface.id : ( size_t ) -1 )
/*! \brief \ref mcr_icpy, plus copying isignal reference and
 * \ref mcr_Signal#is_dispatch
 *
 * \param dstPt Destination to copy to
 * \param srcPt Source to copy from
 * */
MCR_API void mcr_Signal_copy ( mcr_Signal * dstPt, mcr_Signal * srcPt ) ;
/*! \brief Compare two signals
 *
 * \param lhsSigPt mcr_Signal **
 * \param rhsSigPt mcr_Signal **
 * \return <0 if lhs < rhs, 0 if equal, >0 if lhs > rhs
 * */
MCR_API int mcr_Signal_compare ( const void * lhsSigPt,
		const void * rhsSigPt ) ;

//
// Signal development: Add signal type to end, get signal type,
//
/*! \brief Load names of standard signal types. */
MCR_API void mcr_Signal_load_contract ( ) ;
/*! \brief ctor, Set required members of new type,
 * except the dispatcher, which is not modified.
 *
 * \param newType New signal interface to initialize
 * \param sender \ref mcr_ISignal#send
 * \param dataSize \ref mcr_Interface#data_size
 * */
MCR_API void mcr_ISignal_init ( mcr_ISignal * newType,
		mcr_signal_fnc sender, size_t dataSize ) ;
/*! \brief ctor, Set all members of new type.
 *
 * \param newType New signal interface to initialize
 * \param compare \ref mcr_Interface#compare
 * \param copy \ref mcr_Interface#copy
 * \param dataSize \ref mcr_Interface#data_size
 * \param free \ref mcr_Interface#free
 * \param dispatch \ref mcr_ISignal#dispatch
 * \param sender \ref mcr_ISignal#send *
 * */
MCR_API void mcr_ISignal_init_with ( mcr_ISignal * newType,
		mcr_compare_fnc compare,
		void ( * copy ) ( void *, void * ),
		size_t dataSize, void ( * init ) ( void * ),
		void ( * free ) ( void * ),
		mcr_signal_fnc dispatch, mcr_signal_fnc sender ) ;
/*! \brief Id is set as a unique value.
 *
 * \param newType The signal interface to register as new ISignal type
 * \return Value of id for new type. -1 if not successful.
 * */
MCR_API size_t mcr_ISignal_register ( mcr_ISignal * newType ) ;
/*! \brief \ref mcr_ISignal_register, and also map names
 *
 * \param newType The signal interface to register as new ISignal type
 * \param name Name to map to and from newType
 * \param addNames Set of additional names to map to newType
 * \param bufferLen Number of names to map in addNames
 * \return Value of id for new type. -1 if not successful.
 * */
MCR_API size_t mcr_ISignal_register_with ( mcr_ISignal * newType,
		const char * name, const char ** addNames,
		size_t bufferLen ) ;
/*! \brief Get the \ref mcr_ISignal for given id.
 *
 * \param typeId Id of given \ref mcr_ISignal
 * \return Pointer to Signal interface, NULL of id is not registered
 * */
MCR_API mcr_ISignal * mcr_ISignal_get ( size_t typeId ) ;
/*! \brief Get the ISignal from the name of signal type.
 *
 * \param typeName Name of given \ref mcr_ISignal
 * \return Pointer to signal interface. NULL if not found
 * */
MCR_API mcr_ISignal * mcr_ISignal_from_name ( const char * typeName ) ;
/*! \brief Get the id of signal interface from a signal name.
 *
 * \param typeName Name of given \ref mcr_ISignal
 * \return Id of named signal type, or -1 if not found
 * */
MCR_API size_t mcr_ISignal_get_id ( const char * typeName ) ;
/*! \brief Get the signal name from id of signal interface.
 *
 * \param id Id of interface to find
 * \return Name of interface, or NULL if not found
 * */
MCR_API const char * mcr_ISignal_get_name ( size_t id ) ;
/*! \brief Map Signal type to a name, and vice versa
 *
 * \param id Id of signal to set name for
 * \param name This name will be mapped to the signal.
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_ISignal_set_name ( mcr_ISignal * sigPt,
		const char * name ) ;
/*! \brief Map an additional name to this signal.
 *
 * \param sigPt Pointer to signal to add name for
 * \param addName This name will be mapped to the signal.
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_ISignal_add_name ( mcr_ISignal * sigPt,
		const char * name ) ;
/*! \brief \ref mcr_ISignal_add_name with a set of names to add
 *
 * \param names Set of names to add
 * \param bufferLen Number of names to add
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_ISignal_add_names ( mcr_ISignal * sigPt,
		const char ** names, size_t bufferLen ) ;
/*! \brief \ref mcr_ISignal_set_name and \ref mcr_ISignal_add_names
 *
 * \param name This name will be mapped to the signal, and vice versa
 * \param extraNames Set of names to add
 * \param bufferLen Number of names to add
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_ISignal_set_names ( mcr_ISignal * sigPt,
		const char * name,
		const char ** extraNames, size_t bufferLen ) ;
/*! \brief Reset the mapped name for a signal. First that signal
 * will be found by the old name.
 *
 * \param oldName Name to search for signal to replace name for.
 * \param newName Name to map to signal and vice versa
 * \return 0 on failure, otherwise success
 * */
MCR_API int mcr_ISignal_rename ( const char * oldName,
		const char * newName ) ;
//! \brief Get the number of all \ref mcr_ISignal references registered.
MCR_API size_t mcr_ISignal_count ( ) ;
/*! \brief Copy into buffer all registered mcr_ISignals starting
 * with id 0.
 *
 * \param buffer Container to copy all pointers to
 * \ref mcr_ISignals
 * \param bufferLength Number of elements available in buffer
 * */
MCR_API void mcr_ISignal_get_all ( mcr_ISignal ** buffer,
		size_t bufferLength ) ;
/*! \brief Remove all registered \ref mcr_ISignal. */
MCR_API void mcr_ISignal_clear ( ) ;

//! \brief Initialize everything for signal module.
MCR_API void mcr_signal_initialize ( ) ;
/*! \brief Initialize containers for \ref mcr_ISignal registration.
 * */
MCR_API void mcr_signalreg_initialize ( ) ;
//! \brief Clean up everything for signal module.
MCR_API void mcr_signal_cleanup ( void ) ;
/*! \brief Free used memory of \ref mcr_ISignal registration.
 * */
MCR_API void mcr_signalreg_cleanup ( void ) ;

/*! \brief \ref mcr_Signal_init_with
 *
 * \param signal \ref mcr_Signal
 * \param typePt \ref mcr_ISignal *
 * \param dataPt void *
 * \param isHeap int
 * \param isDispatch int
 * */
# define MCR_SIGNAL_INIT( signal, typePt, dataPt, isHeap, \
		isDispatch ) \
	( signal ).type = ( typePt ) ; \
	( signal ).data.data = ( dataPt ) ; \
	( signal ).data.is_heap = ( isHeap ) ; \
	( signal ).is_dispatch = ( isDispatch ) ;

/*! \brief \ref mcr_Signal_free
 *
 * \param signal \ref mcr_Signal
 * */
# define MCR_SIGNAL_FREE( signal ) \
	if ( ( signal ).type ) \
		MCR_IFREE ( ( signal ).type->iface, ( signal ).data )

/*! \brief \ref mcr_mkdata_data
 *
 * \param isignal \ref mcr_ISignal
 * \param dataOut \ref mcr_Data
 * */
# define MCR_ISIGNAL_MKDATA( isignal, dataOut ) \
	MCR_IMKDATA ( ( isignal ).iface, dataOut )

/*! \brief \ref mcr_mkdata_data
 *
 * \param signal \ref mcr_Signal
 * \param dataOut \ref mcr_Data
 * */
# define MCR_SIGNAL_MKDATA( signal, dataOut ) \
	if ( ( signal ).type ) \
		MCR_IMKDATA ( ( signal ).type->iface, dataOut ) ;

/*! \brief \ref mcr_icpy, including \ref mcr_Signal#is_dispatch
 *
 * \param dst \ref mcr_Signal
 * \param src \ref mcr_Signal
 * */
# define MCR_SIGNAL_COPY( dst, src ) \
	if ( ( src ).type ) \
	{ \
		if ( ( dst ).type != ( src ).type ) \
		{ \
			MCR_SIGNAL_FREE ( dst ) ; \
		} \
		( dst ).type = ( src ).type ; \
		MCR_ICPY ( ( src ).type->iface, ( dst ).data, \
				( src ).data ) ; \
	} \
	else \
		MCR_SIGNAL_FREE ( dst ) ; \
	( dst ).is_dispatch = ( src ).is_dispatch ;

/*! \brief \ref mcr_Signal_cmp
 *
 * \param lsignal Left hand side
 * \param rsignal Right hand side
 * \return Logical expression
 * */
# define MCR_SIGNAL_CMP( lsignal, rsignal ) \
	( ( lsignal ).type != ( rsignal ).type ? \
		( lsignal ).type < ( rsignal ).type ? \
			-1 : \
		1 : \
	( lsignal ).type ? \
		MCR_ICMP ( ( lsignal ).type->iface, ( lsignal ).data, \
				( rsignal ).data ) : \
	memcmp ( & ( lsignal ).data.data, \
			& ( rsignal ).data.data, sizeof ( void * ) ) )

# define MCR_SIGNAL_DISPATCH( signal ) \
	( ( signal ).is_dispatch && \
			( signal ).type->dispatch && \
			( signal ).type->dispatch ( & signal ) )
# define mcr_Signal_dispatch_impl( sigPt ) \
	( ( sigPt )->is_dispatch && \
			( sigPt )->type->dispatch && \
			( sigPt )->type->dispatch ( sigPt ) )
/*! \brief If not \ref mcr_Signal#is_dispatch skip to sending signal.
 *
 * 1 ) if is_dispatch, dispatch specific, and if not blocking continue
 * 2 ) if is_dispatch, dispatch unspecific, and if not blocking continue
 * 3 ) send signal
 *
 * \param signal mcr_Signal
 * */
# define MCR_SEND( signal ) \
	( MCR_SIGNAL_DISPATCH ( signal ) ? \
		1 : \
	( signal ).type->send ( ( & signal ) ) )
# define mcr_Send_impl( sigPt ) \
	( mcr_Signal_dispatch_impl ( sigPt ) ? \
		1 : \
	( sigPt )->type->send ( ( sigPt ) ) )

/*! \brief \ref MCR_SEND with array of pointers to mcr_Signal
 *
 * \param signalPtArray mcr_Signal * [ ]
 * \param arrLen size_t Length of signalPtArray
 * \param success int Set to 0 on failure
 * */
# define MCR_SEND_REF_ARRAY( signalPtArray, arrLen, success ) \
	for ( size_t _arrayIterator_ = 0 ; _arrayIterator_ < ( arrLen ) ; \
			_arrayIterator_ ++ ) \
	{ \
		if ( ! MCR_SEND \
				( * ( ( signalPtArray ) [ _arrayIterator_ ] ) ) ) \
			( success ) = 0 ; \
	}

/*! \brief \ref MCR_SEND with array of mcr_Signal
 *
 * \param signalArray mcr_Signal [ ]
 * \param arrLen size_t Length of signalArray
 * \param success int Set to 0 on failure
 * */
# define MCR_SEND_ARRAY( signalArray, arrLen, success ) \
	for ( size_t _arrayIterator_ = 0 ; _arrayIterator_ < ( arrLen ) ; \
			_arrayIterator_ ++ ) \
	{ \
		if ( ! MCR_SEND ( ( signalPtArray ) [ _arrayIterator_ ] ) ) \
			( success ) = 0 ; \
	}

/*! \brief For mcr_ISignal set compare, copy, data size,
 * init, free, and send
 *
 * Should we reset id to 0?
 * \param isignal mcr_ISignal Object to set values into
 * \param comparison \ref mcr_Interface#compare
 * \param copier \ref mcr_Interface#copy
 * \param dataSize \ref mcr_Interface#data_size
 * \param initializer \ref mcr_Interface#init
 * \param freer \ref mcr_Interface#free
 * \param sender mcr_signal_fnc \ref mcr_ISignal#send
 * */
# define MCR_ISIGNAL_INIT( isignal, comparison, copier, \
		dataSize, initializer, freer, sender ) \
	MCR_IINIT ( ( isignal ).iface, comparison, copier, \
			dataSize, initializer, freer ) ; \
	( isignal ).send = ( sender ) ;

# endif
