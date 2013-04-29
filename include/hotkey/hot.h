/*! \file hotkey/hot.h
 * \brief Hot, hotkeys used for Dispatchers.
 */

# ifndef MCR_HOT_H
# define MCR_HOT_H

# include "hotkey/def.h"

typedef struct mcr_Hot mcr_Hot ;
//! Function to call when hotkey is triggered.
typedef void ( * mcr_trigger_fnc ) ( mcr_Hot *, mcr_Signal *,
		unsigned int * ) ;

/*!
 * \brief Register to be triggered for mcr_Signal or modifiers.
 *
 * block - int 0 to not block intercepted signals, otherwise
 * do block intercepted signals. <br>
 * trigger - mcr_trigger_fnc Function to call when triggered. <br>
 * data - void * Extra data that may or may not be used by trigger function. <br>
 * */
typedef struct mcr_Hot
{
	int block ;
	mcr_trigger_fnc trigger ;
	void * data ;
} mcr_Hot ;

/*!
 * \brief mcr_Hot_init All values nullified.
 *
 * \param hotPt mcr_Hot * Object to initialize.
 * */
MCR_API void mcr_Hot_init ( mcr_Hot * hotPt ) ;
/*!
 * \brief mcr_Hot_init_with Initialize hotkey with values.
 *
 * \param hotPt mcr_Hot *
 * \param block int 0 to not block intercepted signals, otherwise block.
 * \param trigger mcr_trigger_fnc Function to call when triggered.
 * \param data void * Extra data.
 */
MCR_API void mcr_Hot_init_with ( mcr_Hot * hotPt, int block,
		mcr_trigger_fnc trigger, void * data ) ;

/*!
 * \brief mcr_Hot_trigger If callback is available, send it. Otherwise
 * call the trigger.
 *
 * \param hotPt mcr_Hot *
 * \param signalPt mcr_Signal * Intercepted signal.
 * \param modsPt unsigned int * Intercepted modifiers.
 * \return 0 To not block, otherwise block intercepted signal.
 */
MCR_API int mcr_Hot_trigger ( mcr_Hot * hotPt, mcr_Signal * signalPt,
		unsigned int * modsPt ) ;
/*!
 * \brief mcr_Hot_trigger_array mcr_Hot_trigger for all members
 * in given array.
 *
 * \param hotArray mcr_Hot ** Set of hotkeys to trigger, unconditionally.
 * \param count size_t Number of members to iterate in hotArray.
 * \param signalPt mcr_Signal * Intercepted signal.
 * \param modsPt unsigned int * Intercepted modifiers.
 * \return 0 To not block, otherwise block intercepted signal.
 */
MCR_API int mcr_Hot_trigger_array ( mcr_Hot ** hotArray, size_t count,
		mcr_Signal * signalPt, unsigned int * modsPt ) ;

//! \brief \ref mcr_Hot_trigger Will not return blocking status.
# define MCR_HOT_TRIGGER( mcr_HotPt, mcr_SignalPt, modsPt ) \
	if ( ( mcr_HotPt )->trigger ) \
	{ \
		( mcr_HotPt )->trigger ( mcr_HotPt, mcr_SignalPt, modsPt ) ; \
	}

//! \brief \ref mcr_Hot_trigger_array Will set blocking status
//! into block parameter.
# define MCR_HOT_TRIGGER_ARRAY( hotPtArray, count, signalPt, modsPt, \
		block ) \
{ \
	if ( ! block ) \
	{ \
		for ( size_t _iterator_ = 0 ; _iterator_ < count ; _iterator_ ++ ) \
		{ \
			MCR_HOT_TRIGGER ( hotPtArray [ _iterator_ ], \
				signalPt, modsPt ) ; \
			if ( hotPtArray [ _iterator_ ]->block ) \
			{ \
				block = 1 ; \
				break ; \
			} \
		} \
	} \
}

# endif
