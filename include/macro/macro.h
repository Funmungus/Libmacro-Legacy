/* include/macro/macro.h - Trigger a set of signals from a hotkey.
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file macro/macro.h
 * \brief Macro, When the macro's hotkey is triggered,
 * its set of signals is sent one after the other.
 */

# ifndef MCR_MACRO_H
# define MCR_MACRO_H

# include "macro/def.h"

typedef enum mcr_Interrupt
{
	//! \brief Able to trigger.
	MCR_CONTINUE = 0,
	/*! \brief One queued macro from one thread will cancel.
	 * After one is cancelled, MCR_CONTINUE is set.
	 **/
	MCR_INTERRUPT,
	/*! \brief All threads will cancel all queued items.
	 * After all are cancelled, MCR_CONTINUE is set.
	 **/
	MCR_INTERRUPT_ALL,
	//! \brief Not able to trigger.
	MCR_INTERRUPT_DISABLE
} mcr_Interrupt ;

typedef struct mcr_Macro
{
	int sticky ;
	unsigned int thread_max ;
	//
	// Internal
	//
	cnd_t cnd ;
	mcr_Hot hot ;
	mcr_Interrupt interruptor ;
	mtx_t lock ;
	mcr_Array signal_set ;
	unsigned int threads ;
	unsigned int queued ;
} mcr_Macro ;

MCR_API void mcr_Macro_init ( mcr_Macro * mcrPt ) ;
MCR_API void mcr_Macro_init_with ( mcr_Macro * mcrPt,
		int sticky, unsigned int threadMax, mcr_Hot * hotPt,
		mcr_Signal * signalSet, size_t signalCount, int enable ) ;
MCR_API void mcr_Macro_free ( mcr_Macro * mcrPt ) ;
/*! \brief Set the macro hotkey.
 *
 * This will also install this macro into the hotkey. That means
 * \ref mcr_Macro_trigger will be set with \ref mcr_Hot#set_trigger.
 * This macro will also be removed from the previous hotkey.
 * \param mcrPt This will be set into \ref mcr_Hot#data.
 * */
MCR_API void mcr_Macro_set_hotkey ( mcr_Macro * mcrPt,
		mcr_Hot * hotPt ) ;
/*! \brief Stop macro execution according to \ref mcr_Interrupt.
 * */
MCR_API void mcr_Macro_interrupt ( mcr_Macro * mcrPt,
		mcr_Interrupt interruptType ) ;
//! \brief Copy all macro signals into a buffer.
MCR_API void mcr_Macro_get_signals ( mcr_Macro * mcrPt,
		mcr_Signal * signalbuffer, size_t bufferCount ) ;
/*! \brief Set all signals for this macro. To avoid buffer overflow,
 * the macro will be completely disabled while modifying the set
 * of signals.
 * */
MCR_API void mcr_Macro_set_signals ( mcr_Macro * mcrPt,
		mcr_Signal * signalSet, size_t signalCount ) ;
MCR_API void mcr_Macro_enable ( mcr_Macro * mcrPt, int enable ) ;
//! \brief Macro function called for hotkey trigger.
MCR_API void mcr_Macro_trigger ( mcr_Hot * hotPt,
		mcr_Signal * sigPt, unsigned int mods ) ;

# define MCR_MACRO_ENABLED( mcrPt ) \
	( ! ( mcrPt )->interruptor )

# endif
