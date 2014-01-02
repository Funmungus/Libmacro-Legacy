/*! \file hotkey/hot.h
 * \brief mcr_Hot, hotkeys used for Dispatchers.
 */

# ifndef MCR_HOT_H
# define MCR_HOT_H

# include "hotkey/def.h"

typedef struct mcr_Hot mcr_Hot ;
//! \brief Function to call when hotkey is triggered.
typedef void ( * mcr_trigger_fnc ) ( mcr_Hot *, mcr_Signal *,
		unsigned int * ) ;

//! \brief Register to be triggered for \ref mcr_Signal or modifiers.
typedef struct mcr_Hot
{
	/*! \brief 0 to not block intercepted signals, otherwise do
	 * block intercepted signals.
	 **/
	int block ;
	//! \brief Function to call when triggered.
	mcr_trigger_fnc trigger ;
	//! \brief Extra data that may or may not be used by trigger function.
	void * data ;
} mcr_Hot, mcr_Hotkey ;

//! \brief mcr_Hot_init All values nullified.
MCR_API void mcr_Hot_init ( mcr_Hot * hotPt ) ;
//! \brief mcr_Hot_init_with Initialize hotkey with values.
MCR_API void mcr_Hot_init_with ( mcr_Hot * hotPt, int block,
		mcr_trigger_fnc trigger, void * data ) ;
/*!
 * \brief mcr_Hot_trigger Call the hotkey trigger, and get the
 * blocking status.
 *
 * \param signalPt Intercepted signal.
 * \param modsPt Intercepted modifiers.
 * \return 0 To not block, otherwise block intercepted signal.
 */
MCR_API int mcr_Hot_trigger ( mcr_Hot * hotPt, mcr_Signal * signalPt,
		unsigned int * modsPt ) ;
/*!
 * \brief mcr_Hot_trigger_array \ref mcr_Hot_trigger for all members
 * in given array.
 *
 * \param hotArray Set of hotkeys to trigger, unconditionally.
 * \param count Number of members to iterate in hotArray.
 * \param signalPt Intercepted signal.
 * \param modsPt Intercepted modifiers.
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
