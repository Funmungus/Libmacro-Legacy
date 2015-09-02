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

/*! \file hotkey/hot.h
 * \brief mcr_Hot, hotkeys used for Dispatchers
 * */

# ifndef MCR_HOT_H
# define MCR_HOT_H

# include "hotkey/def.h"

typedef struct mcr_Hot mcr_Hot ;
//! \brief Function to call when hotkey is triggered.
typedef void ( * mcr_trigger_fnc ) ( mcr_Hot *, mcr_Signal *,
		unsigned int ) ;

/*! \brief Interface type for hotkeys
 * */
typedef struct mcr_IHot
{
	mcr_Interface iface ;
	//
	// Trigger interface
	//
	/*! \brief This trigger is used for internal trigger logic.
	 *
	 * If not available the instance trigger will be called directly. */
	mcr_trigger_fnc trigger ;
} mcr_IHot ;

//! \brief Register to be triggered for \ref mcr_Signal or modifiers
typedef struct mcr_Hot
{
	//
	// Interface
	//
	mcr_IHot * type ;
	//
	// Hotkey instance
	//
	mcr_Data data ;
	//
	// Hotkey instance specific
	//
	/*! \brief 0 to not block intercepted signals, otherwise do
	 * block intercepted signals
	 * */
	int block ;
	//! \brief Function to call when trigger is completed
	mcr_trigger_fnc trigger ;
	//! \brief Object which receives hotkey trigger
	void * trigger_data ;
} mcr_Hot ;

/*! \brief Initialize hotkey with no data, not blocking, no trigger,
 * and no trigger data. */
MCR_API void mcr_Hot_init ( mcr_Hot * hotPt, mcr_IHot * ihotPt ) ;
//! \brief Initialize hotkey with member values.
MCR_API void mcr_Hot_init_with ( mcr_Hot * hotPt, mcr_IHot * ihotPt,
		void * data, int isHeap, int block, mcr_trigger_fnc trigger,
		void * triggerData ) ;
/*! \brief If IHot has data free function, that is used. If data is
 * heap allocated, then free will also be called on data.
 * */
MCR_API void mcr_Hot_free ( mcr_Hot * hotPt ) ;
# define mcr_Hot_free_foreach( hotPt, ... ) mcr_Hot_free ( hotPt )
//
// Some helpful things and abstractions.
//
MCR_API void * mcr_IHot_mkdata_data ( mcr_IHot * ihotPt ) ;
MCR_API mcr_Data mcr_IHot_mkdata ( mcr_IHot * ihotPt ) ;
MCR_API void mcr_Hot_copy ( mcr_Hot * dstPt, mcr_Hot * srcPt ) ;
MCR_API int mcr_Hot_compare ( const void * lhsHotPt,
		const void * rhsHotPt ) ;

//
// Mostly internal.
//
/*! \brief Call the hotkey trigger, and get the
 * blocking status.
 *
 * \param signalPt Intercepted signal
 * \param mods Intercepted modifiers
 * \return 0 To not block, otherwise block intercepted signal
 */
MCR_API int mcr_Hot_trigger ( mcr_Hot * hotPt, mcr_Signal * signalPt,
		unsigned int mods ) ;
/*!
 * \brief \ref mcr_Hot_trigger for all members
 * in given array
 *
 * \param hotArray Set of hotkeys to trigger, unconditionally
 * \param count Number of members to iterate in hotArray
 * \param signalPt Intercepted signal
 * \param mods Intercepted modifiers
 * \return 0 To not block, otherwise block intercepted signal
 */
MCR_API int mcr_Hot_trigger_array ( mcr_Hot ** hotArray, size_t count,
		mcr_Signal * signalPt, unsigned int mods ) ;

//
// Hotkey type development.
//
MCR_API void mcr_IHot_init ( mcr_IHot * ihotPt, size_t dataSize ) ;
MCR_API void mcr_IHot_init_with ( mcr_IHot * ihotPt,
		mcr_compare_fnc compare, void ( * copy ) ( void *, void * ),
		size_t dataSize, void ( * init ) ( void * ),
		void ( * free ) ( void * ), mcr_trigger_fnc trigger ) ;
MCR_API size_t mcr_IHot_register ( mcr_IHot * newType ) ;
MCR_API mcr_IHot * mcr_IHot_get ( size_t typeId ) ;
MCR_API mcr_IHot * mcr_IHot_from_name ( const char * typeName ) ;
MCR_API size_t mcr_IHot_get_id ( const char * typeName ) ;
MCR_API const char * mcr_IHot_get_name ( size_t id ) ;
MCR_API int mcr_IHot_set_name ( mcr_IHot * ihotPt,
		const char * name ) ;
MCR_API int mcr_IHot_add_name ( mcr_IHot * ihotPt,
		const char * name ) ;
MCR_API int mcr_IHot_add_names ( mcr_IHot * ihotPt,
		const char ** names, size_t bufferLen ) ;
MCR_API int mcr_IHot_set_names ( mcr_IHot * ihotPt,
		const char * name,
		const char ** addNames, size_t bufferLen ) ;
MCR_API int mcr_IHot_rename ( const char * oldName,
		const char * newName ) ;
MCR_API size_t mcr_IHot_count ( ) ;
MCR_API void mcr_IHot_get_all ( mcr_IHot ** buffer,
		size_t bufferLength ) ;
MCR_API void mcr_IHot_clear ( ) ;

MCR_API void mcr_hot_initialize ( ) ;
MCR_API void mcr_hot_cleanup ( ) ;

# define MCR_HOT_INIT( hot, typePt, dataPt, isHeap, blocking, trig, \
		trigData ) \
	( hot ).type = ( typePt ) ; \
	( hot ).data.data = ( dataPt ) ; \
	( hot ).data.is_heap = ( isHeap ) ; \
	( hot ).block = ( blocking ) ; \
	( hot ).trigger = ( trig ) ; \
	( hot ).trigger_data = ( trigData ) ;

# define MCR_HOT_FREE( hot ) \
	MCR_IFREE ( ( hot ).type->iface, ( hot ).data ) ;

# define MCR_IHOT_MKDATA( ihot, dataOut ) \
	MCR_IMKDATA ( ( ihot ).iface, dataOut ) ;

# define MCR_HOT_MKDATA( hot, dataOut ) \
	MCR_IMKDATA ( ( hot ).type->iface, dataOut ) ;

# define MCR_HOT_COPY( dst, src ) \
	dassert ( ( src ).type ) ; \
	if ( ( dst ).type && ( dst ).type \
			!= ( src ).type ) \
	{ \
		MCR_HOT_FREE ( dst ) ; \
	} \
	( dst ).type = ( src ).type ; \
	MCR_ICPY ( ( src ).type->iface, ( dst ).data, \
			( src ).data ) ; \
	( dst ).block = ( src ).block ; \
	( dst ).trigger = ( src ).trigger ; \
	( dst ).trigger_data = ( src ).trigger_data ;

# define MCR_HOT_CMP( lHot, rHot ) \
	( ( lHot ).type != ( rHot ).type ? \
		( lHot ).type < ( rHot ).type ? \
			-1 : \
		1 : \
	MCR_ICMP ( ( lHot ).type->iface, ( lHot ).data, \
				( rHot ).data ) ) \

# define MCR_IHOT_INIT( ihot, comparison, copier, \
		dataSize, initializer, freer, trig ) \
	MCR_IINIT ( ( ihot ).iface, comparison, copier, \
			dataSize, initializer, freer ) ; \
	( ihot ).trigger = ( trig ) ;

//! \brief \ref mcr_Hot_trigger, will not return blocking status
# define mcr_Hot_trigger_impl( hotPt, sigPt, mods ) \
	if ( ( hotPt )->type->trigger ) \
		( hotPt )->type->trigger ( hotPt, sigPt, mods ) ; \
	else if ( ( hotPt )->trigger ) \
		( hotPt )->trigger ( hotPt, sigPt, mods ) ;
//! \brief \ref mcr_Hot_trigger, will not return blocking status
# define MCR_HOT_TRIGGER( hot, sigPt, mods ) \
	if ( ( hot ).type->trigger ) \
		( hot ).type->trigger ( & hot, sigPt, mods ) ; \
	else if ( ( hot ).trigger ) \
		( hot ).trigger ( & hot, sigPt, mods ) ;

//! \brief \ref mcr_Hot_trigger_array, will set blocking status
//! into block parameter.
# define MCR_HOT_TRIGGER_REF_ARRAY( hotPtArray, count, signalPt, mods, \
		block ) \
{ \
	for ( size_t _iterator_ = 0 ; _iterator_ < count ; \
			_iterator_ ++ ) \
	{ \
		mcr_Hot_trigger_impl ( hotPtArray [ _iterator_ ], \
			signalPt, mods ) ; \
		if ( hotPtArray [ _iterator_ ]->block ) \
		{ \
			block = 1 ; \
			break ; \
		} \
	} \
}

# endif
