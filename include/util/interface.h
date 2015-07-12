/* include/util/interface.h - Interface helper functions.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

# ifndef MCR_INTERFACE_H
# define MCR_INTERFACE_H

# include "util/map.h"

//! \brief An interface for functions on data.
typedef struct mcr_Interface
{
	//! \brief Unique identifier for this signal type.
	size_t id ;
	//! \brief \ref mcr_compare_fnc for data. Default memcmp.
	mcr_compare_fnc compare ;
	/*! \pre Destination must be initialized.
	 * \brief Copy data from source. Default memcpy.
	 *
	 * data is assumed to be initialized already. */
	void ( * copy ) ( void * dst, void * src ) ;
	//! \brief Byte size of a data element.
	size_t data_size ;
	//! \brief Initializer for data. Default memset 0.
	void ( * init ) ( void * ) ;
	//! \brief Free function for data. Default do nothing.
	void ( * free ) ( void * ) ;
} mcr_Interface ;

typedef struct mcr_Data
{
	void * data ;
	int is_heap ;
} mcr_Data ;

typedef struct mcr_IRegistry
{
	mcr_Array set ;
	mcr_Array names ;
	mcr_Map name_map ;
} mcr_IRegistry ;

//
// Some helpful things and abstractions.
//
MCR_API void * mcr_mkdata_data ( mcr_Interface * iPt ) ;
MCR_API mcr_Data mcr_mkdata ( mcr_Interface * iPt ) ;
MCR_API void mcr_icpy ( mcr_Interface * iPt, mcr_Data * dstPt,
		mcr_Data * srcPt ) ;
MCR_API int mcr_icmp ( mcr_Interface * iPt, mcr_Data * lhs,
		mcr_Data * rhs ) ;
MCR_API void mcr_ifree ( mcr_Interface * iPt, mcr_Data * dataPt ) ;

//
// Interface development
//
MCR_API void mcr_iinit ( mcr_Interface * newType,
		size_t dataSize ) ;
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
MCR_API void mcr_iinit_maps ( mcr_IRegistry * iRegPt ) ;
MCR_API size_t mcr_iregister ( mcr_IRegistry * iRegPt, mcr_Interface * newType ) ;
MCR_API mcr_Interface * mcr_iget ( mcr_IRegistry * iRegPt, size_t typeId ) ;
MCR_API mcr_Interface * mcr_ifrom_name (
		mcr_IRegistry * iRegPt, const char * typeName ) ;
MCR_API size_t mcr_iget_id ( mcr_IRegistry * iRegPt,
		const char * typeName ) ;
# define mcr_iget_name( iRegPt, id ) \
	( id < ( iRegPt )->names.used ? \
		( ( mcr_Array * ) MCR_ARR_AT ( \
				& ( iRegPt )->names, id ) )->array : \
	NULL )
MCR_API int mcr_iset_name ( mcr_IRegistry * iRegPt,
		mcr_Interface * iPt, const char * name ) ;
MCR_API int mcr_iadd_name ( mcr_IRegistry * iRegPt, mcr_Interface * iPt,
		const char * name ) ;
MCR_API int mcr_irename ( mcr_IRegistry * iRegPt, const char * oldName,
		const char * newName ) ;
# define mcr_icount( iRegPt ) \
	( ( iRegPt )->set.used )
MCR_API void mcr_iget_all ( mcr_IRegistry * iRegPt, mcr_Interface ** buffer,
		size_t bufferLength ) ;
MCR_API void mcr_iclear ( mcr_IRegistry * iRegPt ) ;

# define MCR_IFREE( iPt, dataPt ) \
	if ( ( dataPt )->data ) \
	{ \
		if ( ( iPt ) && ( iPt )->free ) \
			( iPt )->free ( ( dataPt )->data ) ; \
		if ( ( dataPt )->is_heap ) \
			free ( ( dataPt )->data ) ; \
		( dataPt )->data = NULL ; \
		( dataPt )->is_heap = 0 ; \
	}

# define MCR_IMKDATA( iPt, dataOut ) \
	if ( ( dataOut )->data && ( iPt )->free ) \
	{ /* Do not want to free heap, just uninitialize it. */ \
		( iPt )->free ( ( dataOut )->data ) ; \
	} \
	if ( ! ( dataOut )->data ) \
	{ \
		( dataOut )->data = malloc ( ( iPt )->data_size ) ; \
		if ( ( dataOut )->data ) \
			( dataOut )->is_heap = 1 ; \
	} \
	if ( ( dataOut )->data ) \
	{ \
		if ( ( iPt )->init ) \
			( iPt )->init ( ( dataOut )->data ) ; \
		else \
			memset ( ( dataOut )->data, 0, ( iPt )->data_size ) ; \
	} \

# define MCR_ICPY( iPt, dataOutPt, dataInPt ) \
	if ( ( dataInPt )->data ) \
	{ \
		MCR_IMKDATA ( iPt, dataOutPt ) ; \
		if ( ( dataOutPt )->data ) \
		{ \
			if ( ( iPt )->copy ) \
				( iPt )->copy ( ( dataOutPt )->data, \
						( dataInPt )->data ) ; \
			else \
				memcpy ( ( dataOutPt )->data, ( dataInPt )->data, \
						( iPt )->data_size ) ; \
		} \
	} \
	else \
	{ \
		MCR_IFREE ( iPt, dataOutPt ) ; \
	}

# define MCR_ICMP( iPt, lDataPt, rDataPt ) \
	( ! ( lDataPt )->data || ! ( rDataPt )->data ? \
		( rDataPt )->data ? \
			-1 : \
		( lDataPt )->data && 1 : \
	( iPt )->compare ? \
		( iPt )->compare ( ( lDataPt )->data, ( rDataPt )->data ) : \
	memcmp ( ( lDataPt )->data, ( rDataPt )->data, \
			( iPt )->data_size ) )

# define MCR_IINIT( iPt, comparison, copier, \
		dataSize, initializer, freer ) \
	( iPt )->id = ( size_t ) -1 ; \
	( iPt )->compare = ( comparison ) ; \
	( iPt )->copy = ( copier ) ; \
	( iPt )->data_size = ( dataSize ) ; \
	( iPt )->init = ( initializer ) ; \
	( iPt )->free = ( freer ) ;

# endif // MCR_INTERFACE_H
