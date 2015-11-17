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

/*! \file util/interface.h
 * \brief Interface to be extended by structures, and grouped by typedef
 * */

# ifndef MCR_INTERFACE_H
# define MCR_INTERFACE_H

# include "util/map.h"

//! \brief An interface for functions on data
typedef struct mcr_Interface
{
	//! \brief Unique identifier for this type
	size_t id ;
	//! \brief \ref mcr_compare_fnc for data, default memcmp
	mcr_compare_fnc compare ;
	/*! \pre Destination must be initialized
	 * \brief Copy data from source, default memcpy
	 *
	 * data is assumed to be initialized already. */
	void ( * copy ) ( void * dst, void * src ) ;
	//! \brief Byte size of a data element
	size_t data_size ;
	//! \brief Initializer for data, default memset 0
	void ( * init ) ( void * ) ;
	//! \brief Free function for data, default do nothing
	void ( * free ) ( void * ) ;
} mcr_Interface ;

/*! \brief Data allocated with this structure can be automatically
 * freed. */
typedef struct mcr_Data
{
	//! \brief Pointer to an object, which may or may not need to be freed
	void * data ;
	/*! \brief If true, then data should be freed before data is
	 * reassigned. */
	int is_heap ;
} mcr_Data ;

/*! \brief For a given category of interfaces, register an id and name
 * for each interface definition. */
typedef struct mcr_IRegistry
{
	/*! \brief Each index is a pointer to an interface definition.
	 * The id of that interface should also be the same index. */
	mcr_Array set ;
	/*! \brief The index of this array is the id of interface.
	 * Every element is a mcr_Array, which is the name of
	 * that interface. */
	mcr_Array names ;
	/*! \brief Map from name to interface pointer, many names
	 * may map to the same pointer.
	 *
	 * \ref mcr_name_compare */
	mcr_Map name_map ;
} mcr_IRegistry ;

//
// Some helpful things and abstractions.
//
/*! \brief Heap-allocate data of given interface.
 * This is only the pointer, and is_heap must be elsewhere. */
MCR_API void * mcr_mkdata_data ( mcr_Interface * iPt ) ;
/*! \brief Heap-allocate data, and set is_heap to 1. */
MCR_API mcr_Data mcr_mkdata ( mcr_Interface * iPt ) ;
/*! \brief Copy data for given interface type.
 *
 * \param dstPt Data object in which data is copied into
 * \param srcPt Data object from which data is copied
 * */
MCR_API void mcr_icpy ( mcr_Interface * iPt, mcr_Data * dstPt,
		mcr_Data * srcPt ) ;
/*! \brief Compare data for given interface type.
 *
 * \param lhs Left side of comparison
 * \param rhs Right side of comparison
 * \return <0 if lhs < rhs, >0 if lhs > rhs, 0 if equal
 * */
MCR_API int mcr_icmp ( mcr_Interface * iPt, mcr_Data * lhs,
		mcr_Data * rhs ) ;
/*! \brief Free data of given interface.  If is_heap \ref free
 * will be called on the data.  \ref mcr_Data#data will always be
 * set to null, and \ref mcr_Data#is_heap will always be set to 0.
 *
 * \param dataPt Data to free, if \ref mcr_Data#is_heap
 * */
MCR_API void mcr_ifree ( mcr_Interface * iPt, mcr_Data * dataPt ) ;

//
// Interface development
//
/*! \brief Initialize interface with given size of data.
 * All function pointers will be null, and default standard functions
 * will be used.
 *
 * \param dataSize Size of interface data type
 * */
MCR_API void mcr_iinit ( mcr_Interface * newType,
		size_t dataSize ) ;
/*! \brief Initialize interface with data size and functions.
 *
 * \param compare Comparison function for data
 * \param copy Function to copy data
 * \param dataSize Size of data
 * \param free Function to free allocated data
 * */
MCR_API void mcr_iinit_with ( mcr_Interface * newType,
		mcr_compare_fnc compare,
		void ( * copy ) ( void *, void * ),
		size_t dataSize, void ( * init ) ( void * ),
		void ( * free ) ( void * ) ) ;

//
// All arrays are assumed character arrays, and all maps
// are assumed maps from character arrays to mcr_Interface reference
// with mcr_name_compare.
//
/*! \brief Initialize mapping for an interface type.
 * */
MCR_API void mcr_iinit_maps ( mcr_IRegistry * iRegPt ) ;
/*! \brief Register a new interface.  The interface id will be
 * set as the next possible id to be registered.
 *
 * \param newType The new interface to register
 * */
MCR_API size_t mcr_iregister ( mcr_IRegistry * iRegPt, mcr_Interface * newType ) ;
/*! \brief Get a reference to the interface of the same id.
 *
 * \param typeId Id of interface
 * \return Interface of given id, or NULL
 * */
MCR_API mcr_Interface * mcr_iget ( mcr_IRegistry * iRegPt, size_t typeId ) ;
/*! \brief Get a reference to the interface from its registered name.
 *
 * \param typeName Name of interface
 * \return Interface of given name, or NULL
 * */
MCR_API mcr_Interface * mcr_ifrom_name (
		mcr_IRegistry * iRegPt, const char * typeName ) ;
/*! \brief Get the id of the interface of given name.
 *
 * \param Name of interface
 * \return Id of interface
 * */
MCR_API size_t mcr_iget_id ( mcr_IRegistry * iRegPt,
		const char * typeName ) ;
/*! \brief Get the name of the interface of given id.
 *
 * \param iRegPt \ref mcr_IRegistry *
 * \param id size_t
 * \return char *
 * */
# define mcr_iget_name( iRegPt, id ) \
( id < ( iRegPt )->names.used ? \
	( ( mcr_Array * ) MCR_ARR_AT ( \
			( iRegPt )->names, id ) )->array : \
NULL )
/*! \brief Map a name to an interface, and vice versa.
 *
 * \param iPt Interface to map
 * \param name Name to map
 * \return 0 for failure, otherwise success
 * */
MCR_API int mcr_iset_name ( mcr_IRegistry * iRegPt,
		mcr_Interface * iPt, const char * name ) ;
/*! \brief Add a mapping from name to interface.
 *
 * \param iPt Interface to map to
 * \param name Name to map from
 * \return 0 for failure, otherwise success
 * */
MCR_API int mcr_iadd_name ( mcr_IRegistry * iRegPt, mcr_Interface * iPt,
		const char * name ) ;
/*! \brief Remove mapping to an interface from one name to another.
 *
 * \param oldName Remove mapping from this name.
 * \param newName Add mapping from this name.
 * \return 0 for failure, otherwise success
 * */
MCR_API int mcr_irename ( mcr_IRegistry * iRegPt, const char * oldName,
		const char * newName ) ;
/*! \brief Get the number of registered interfaces.
 *
 * \param iRegPt \ref mcr_IRegistry
 * \ret size_t
 * */
# define mcr_icount( iRegPt ) \
( ( iRegPt )->set.used )
/*! \brief Get a reference to all registered interfaces.
 *
 * \param buffer References are copied here.
 * \param buffereLength Number of elements available in buffer
 * */
MCR_API void mcr_iget_all ( mcr_IRegistry * iRegPt, mcr_Interface ** buffer,
		size_t bufferLength ) ;
/*! \brief Remove all registered interfaces.
 * */
MCR_API void mcr_iclear ( mcr_IRegistry * iRegPt ) ;

/*! \brief \ref mcr_ifree
 *
 * \param interface \ref mcr_Interface
 * \param freeData \ref mcr_Data
 * */
# define MCR_IFREE( interface, freeData ) \
if ( ( freeData ).data ) \
{ \
	if ( ( interface ).free ) \
		( interface ).free ( ( freeData ).data ) ; \
	if ( ( freeData ).is_heap ) \
		free ( ( freeData ).data ) ; \
	( freeData ).data = NULL ; \
	( freeData ).is_heap = 0 ; \
}

/*! \brief \ref mcr_mkdata
 *
 * \param interface \ref mcr_Interface
 * \param dataOut \ref mcr_Data, Data will be uninitialized,
 * and initialized here.
 * */
# define MCR_IMKDATA( interface, dataOut ) \
if ( ( dataOut ).data && ( interface ).free ) \
{ /* Do not want to free heap, just uninitialize it. */ \
	( interface ).free ( ( dataOut ).data ) ; \
} \
if ( ! ( dataOut ).data ) \
{ \
	( dataOut ).data = malloc ( ( interface ).data_size ) ; \
	if ( ( dataOut ).data ) \
		( dataOut ).is_heap = 1 ; \
} \
if ( ( dataOut ).data ) \
{ \
	if ( ( interface ).init ) \
		( interface ).init ( ( dataOut ).data ) ; \
	else \
		memset ( ( dataOut ).data, 0, ( interface ).data_size ) ; \
}

/*! \brief \ref mcr_icpy
 *
 * \param interface \ref mcr_Interface
 * \param dataOut \ref mcr_Data
 * \param dataIn \ref mcr_Data
 * */
# define MCR_ICPY( interface, dataOut, dataIn ) \
if ( ( dataIn ).data ) \
{ \
	MCR_IMKDATA ( interface, dataOut ) ; \
	if ( ( dataOut ).data ) \
	{ \
		if ( ( interface ).copy ) \
			( interface ).copy ( ( dataOut ).data, \
					( dataIn ).data ) ; \
		else \
			memcpy ( ( dataOut ).data, ( dataIn ).data, \
					( interface ).data_size ) ; \
	} \
} \
else \
{ \
	MCR_IFREE ( interface, dataOut ) ; \
}

/*! \brief \ref mcr_icmp
 *
 * \param interface \ref mcr_Interface
 * \param lData \ref mcr_Data
 * \param rData \ref mcr_Data
 * */
# define MCR_ICMP( interface, lData, rData ) \
( ! ( lData ).data || ! ( rData ).data ? \
	( rData ).data ? \
		-1 : \
	( lData ).data && 1 : \
( interface ).compare ? \
	( interface ).compare ( ( lData ).data, ( rData ).data ) : \
memcmp ( ( lData ).data, ( rData ).data, ( interface ).data_size ) )

/*! \brief \ref mcr_iinit_with
 *
 * \param interface \ref mcr_Interface
 * \param comparison \ref mcr_Interface#compare
 * \param copier \ref mcr_Interface#copy
 * \param dataSize \ref mcr_Interface#data_size
 * \param initializer \ref mcr_Interface#init
 * \param freer \ref mcr_Interface#free
 * */
# define MCR_IINIT( interface, comparison, copier, \
		dataSize, initializer, freer ) \
( interface ).id = ( size_t ) -1 ; \
( interface ).compare = ( comparison ) ; \
( interface ).copy = ( copier ) ; \
( interface ).data_size = ( dataSize ) ; \
( interface ).init = ( initializer ) ; \
( interface ).free = ( freer ) ;

# endif // MCR_INTERFACE_H
