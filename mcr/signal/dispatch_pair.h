/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software

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

/*! \file
 *  \brief \ref mcr_DispatchPair - Receiver and receiving function pair which
 *  intercepts signals before they are sent
 */

#ifndef MCR_SIGNAL_DISPATCH_PAIR_H_
#define MCR_SIGNAL_DISPATCH_PAIR_H_

#include "mcr/signal/def.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mcr_Signal;
/*! Receive dispatch before signal is sent
 *
 *  \param receiver Object to receive signal.  The receiver might
 *  be optional for some receiving functions.
 *  \param dispatchSignal Intercepted signal
 *  \param mods Intercepted modifiers
 *  \return True to block from sending
 */
typedef bool(*mcr_Dispatcher_receive_fnc) (void *receiver,
		struct mcr_Signal * dispatchSignal, unsigned int mods);
/*! Generic receiver and function that may block or intercept a signal
 *  being sent.
 *
 *  Receiver is first, to compare with \ref mcr_ref_compare.
 */
struct mcr_DispatchPair {
	/*! Object to receive signal */
	void *receiver;
	/*! Function to act on receiver */
	mcr_Dispatcher_receive_fnc dispatch;
};
/*! Create new \ref mcr_DispatchPair
 *
 *  \param receiver \ref opt \ref mcr_DispatchPair.receiver
 *  \param dispatch \ref opt \ref mcr_DispatchPair.dispatch
 *  \return New \ref mcr_DispatchPair
 */
MCR_API struct mcr_DispatchPair mcr_DispatchPair_new(void *receiver,
		mcr_Dispatcher_receive_fnc dispatch);
/*! Compare \ref mcr_DispatchPair, only compare receivers. */
#define mcr_DispatchPair_compare mcr_ref_compare

/*! Interface for a \ref mcr_Array of \ref mcr_DispatchPair structures.
 *
 *  Dispatch maps should always end with this array.
 */
MCR_API const struct mcr_Interface *mcr_Array_DispatchPair_interface();

#ifdef __cplusplus
}
#endif

#endif
