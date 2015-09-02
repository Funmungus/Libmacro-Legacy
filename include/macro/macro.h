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

/*! \file macro/macro.h
 * \brief Macro: When the macro's hotkey is triggered,
 * its set of signals is sent one after the other.
 */

# ifndef MCR_MACRO_H
# define MCR_MACRO_H

# include "macro/def.h"

typedef enum mcr_Interrupt
{
	//! \brief Able to trigger
	MCR_CONTINUE = 0,
	/*! \brief One queued macro from one thread will cancel.
	 * After one is cancelled, MCR_CONTINUE is set.
	 **/
	MCR_INTERRUPT,
	/*! \brief All threads and queued items will cancel.
	 * After all are cancelled, MCR_CONTINUE is set.
	 **/
	MCR_INTERRUPT_ALL,
	//! \brief Not able to trigger
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
//	mcr_Map thread_finish ;
	unsigned int thread_count ;
	unsigned int queued ;
} mcr_Macro ;

//typedef struct
//{
//	mcr_Macro * mcr_pt ;
//	thrd_t trd ;
//} mcr_MacroThread ;

MCR_API void mcr_Macro_init ( mcr_Macro * mcrPt ) ;
MCR_API void mcr_Macro_init_with ( mcr_Macro * mcrPt,
		int sticky, unsigned int threadMax, mcr_Hot * hotPt,
		mcr_Signal * signalSet, size_t signalCount, int enable ) ;
MCR_API void mcr_Macro_copy ( void * dstPt, void * srcPt ) ;
MCR_API void mcr_Macro_free ( mcr_Macro * mcrPt ) ;
/*! \brief Copy a hotkey into macro's hotkey, then
 * \ref mcr_Macro_set_trigger. */
MCR_API void mcr_Macro_set_hotkey ( mcr_Macro * mcrPt,
		mcr_Hot * hotPt ) ;
/*! \brief Trigger this macro with a hotkey.
 *
 * This will also install this macro into the hotkey. That means
 * \ref mcr_Macro_trigger will be set with \ref mcr_Hot#set_trigger.
 * */
MCR_API void mcr_Macro_set_trigger ( mcr_Macro * mcrPt,
		mcr_Hot * hotPt ) ;
/*! \brief Stop macro execution according to \ref mcr_Interrupt.
 * */
MCR_API void mcr_Macro_interrupt ( mcr_Macro * mcrPt,
		mcr_Interrupt interruptType ) ;
MCR_API mcr_Signal * mcr_Macro_signal ( mcr_Macro * mcrPt,
		size_t index ) ;
//! \brief Copy all macro signals into a buffer.
MCR_API void mcr_Macro_signals ( mcr_Macro * mcrPt,
		mcr_Signal * signalbuffer, size_t bufferCount ) ;
MCR_API void mcr_Macro_set_signal ( mcr_Macro * mcrPt,
		mcr_Signal * copySig, size_t index ) ;
/*! \brief Set all signals for this macro. To avoid buffer overflow,
 * the macro will be completely disabled while modifying the set
 * of signals.
 * */
MCR_API void mcr_Macro_set_signals ( mcr_Macro * mcrPt,
		mcr_Signal * signalSet, size_t signalCount ) ;
MCR_API void mcr_Macro_insert_signal ( mcr_Macro * mcrPt,
		mcr_Signal * copySig, size_t index ) ;
MCR_API void mcr_Macro_remove_signal ( mcr_Macro * mcrPt,
		size_t index ) ;
MCR_API void mcr_Macro_push_signal ( mcr_Macro * mcrPt,
		mcr_Signal * newSig ) ;
MCR_API void mcr_Macro_pop_signal ( mcr_Macro * mcrPt ) ;
MCR_API void mcr_Macro_set_enabled ( mcr_Macro * mcrPt, int enable ) ;
//! \brief Macro function called for hotkey trigger.
MCR_API void mcr_Macro_trigger ( mcr_Hot * hotPt,
		mcr_Signal * sigPt, unsigned int mods ) ;

# define MCR_MACRO_ENABLED( mcrPt ) \
	( ! ( mcrPt )->interruptor )

# endif
