/* include/hotkey/hot.h - Hotkey base type definition.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file hotkey/hot.h
 * \brief mcr_Hot, hotkeys used for Dispatchers.
 * */

# ifndef MCR_HOT_H
# define MCR_HOT_H

# include "hotkey/def.h"

typedef struct mcr_Hot mcr_Hot ;
//! \brief Function to call when hotkey is triggered.
typedef void ( * mcr_trigger_fnc ) ( mcr_Hot *, mcr_Signal *,
		unsigned int ) ;

typedef struct mcr_IHot
{
	mcr_Interface interface ;
	//
	// Trigger interface
	//
	/*! \brief This trigger is used for internal trigger logic.
	 *
	 * If not available the instance trigger will be called directly. */
	mcr_trigger_fnc trigger ;
} mcr_IHot ;

//! \brief Register to be triggered for \ref mcr_Signal or modifiers.
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
	 * block intercepted signals.
	 **/
	int block ;
	//! \brief Function to call when trigger is completed.
	mcr_trigger_fnc trigger ;
	void * trigger_data ;
} mcr_Hot ;

//! \brief
MCR_API void mcr_Hot_init ( mcr_Hot * hotPt, mcr_IHot * ihotPt ) ;
//! \brief mcr_Hot_init_with Initialize hotkey with values.
MCR_API void mcr_Hot_init_with ( mcr_Hot * hotPt, mcr_IHot * ihotPt,
		void * data, int isHeap, int block, mcr_trigger_fnc trigger,
		void * triggerData ) ;
/*! \brief If IHot has data free function, that is used. If data is
 * heap allocated, then free will also be called on data.
 * */
MCR_API void mcr_Hot_free ( mcr_Hot * hotPt ) ;


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
/*!
 * \brief mcr_Hot_trigger Call the hotkey trigger, and get the
 * blocking status.
 *
 * \param signalPt Intercepted signal.
 * \param mods Intercepted modifiers.
 * \return 0 To not block, otherwise block intercepted signal.
 */
MCR_API int mcr_Hot_trigger ( mcr_Hot * hotPt, mcr_Signal * signalPt,
		unsigned int mods ) ;
/*!
 * \brief mcr_Hot_trigger_array \ref mcr_Hot_trigger for all members
 * in given array.
 *
 * \param hotArray Set of hotkeys to trigger, unconditionally.
 * \param count Number of members to iterate in hotArray.
 * \param signalPt Intercepted signal.
 * \param mods Intercepted modifiers.
 * \return 0 To not block, otherwise block intercepted signal.
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


# define MCR_HOT_INIT( hotPt, typePt, dataPt, isHeap, blocking, trig, \
		trigData ) \
	( hotPt )->type = ( typePt ) ; \
	( hotPt )->data.data = ( dataPt ) ; \
	( hotPt )->data.is_heap = ( isHeap ) ; \
	( hotPt )->block = ( blocking ) ; \
	( hotPt )->trigger = ( trig ) ; \
	( hotPt )->trigger_data = ( trigData ) ;

# define MCR_HOT_FREE( hotPt ) \
	MCR_IFREE ( & ( hotPt )->type->interface, & ( hotPt )->data ) ;

# define MCR_IHOT_MKDATA( ihotPt, dataPtOut ) \
	MCR_IMKDATA ( & ( ihotPt )->interface, dataPtOut ) ;

# define MCR_HOT_MKDATA( hotPt, dataPtOut ) \
	MCR_IMKDATA ( & ( hotPt )->type->interface, dataPtOut ) ;

# define MCR_HOT_COPY( dstPt, srcPt ) \
	dassert ( ( srcPt )->type ) ; \
	if ( ( dstPt )->type && ( dstPt )->type \
			!= ( srcPt )->type ) \
	{ \
		MCR_HOT_FREE ( dstPt ) ; \
	} \
	( dstPt )->type = ( srcPt )->type ; \
	MCR_ICPY ( & ( srcPt )->type->interface, & ( dstPt )->data, \
			& ( srcPt )->data ) ;

# define MCR_HOT_CMP( lHotPt, rHotPt ) \
	( ( lHotPt )->type != ( rHotPt )->type ? \
		( lHotPt )->type < ( rHotPt )->type ? \
			-1 : \
		1 : \
	MCR_ICMP ( & ( lHotPt )->type->interface, & ( lHotPt )->data, \
				& ( rHotPt )->data ) ) \

/*!
 * \brief For pointer to mcr_ISignal set data size, send,
 *
 * Should we reset id to 0?
 * \param isignalPt mcr_ISignal * Object to set values into.
 * \param dataSize size_t mcr_ISignal data_size.
 * \param sender mcr_signal_fnc mcr_ISignal send.
 * */
# define MCR_IHOT_INIT( ihotPt, comparison, copier, \
		dataSize, initializer, freer, trig ) \
	MCR_IINIT ( & ( ihotPt )->interface, comparison, copier, \
			dataSize, initializer, freer ) ; \
	( ihotPt )->trigger = ( trig ) ;

//! \brief \ref mcr_Hot_trigger Will not return blocking status.
# define MCR_HOT_TRIGGER( mcr_HotPt, mcr_SignalPt, mods ) \
	if ( ( mcr_HotPt )->type->trigger ) \
		( mcr_HotPt )->type->trigger ( mcr_HotPt, mcr_SignalPt, mods ) ; \
	else if ( ( mcr_HotPt )->trigger ) \
		( mcr_HotPt )->trigger ( mcr_HotPt, mcr_SignalPt, mods ) ;

//! \brief \ref mcr_Hot_trigger_array Will set blocking status
//! into block parameter.
# define MCR_HOT_TRIGGER_REF_ARRAY( hotPtArray, count, signalPt, mods, \
		block ) \
{ \
	for ( size_t _iterator_ = 0 ; _iterator_ < count ; \
			_iterator_ ++ ) \
	{ \
		MCR_HOT_TRIGGER ( hotPtArray [ _iterator_ ], \
			signalPt, mods ) ; \
		if ( hotPtArray [ _iterator_ ]->block ) \
		{ \
			block = 1 ; \
			break ; \
		} \
	} \
}

# endif
