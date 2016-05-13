/* Libmacro - A multi-platform, extendable macro and hotkey C library.
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

/*! \file
 * \brief Receiver and receiving function pair
 */

#ifndef MCR_DISPATCHPAIR_H
#define MCR_DISPATCHPAIR_H

#include "mcr/signal/def.h"

struct mcr_Signal;
/*!
 * \brief Receive dispatch before signal is sent
 *
 * \param receiver Object to receive signal
 * \param dispatchSignal Intercepted signal
 * \param mods Intercepted modifiers
 * \return true to block sending
 */
typedef bool(*mcr_Dispatcher_receive_fnc) (void *receiver,
	struct mcr_Signal * dispatchSignal, unsigned int mods);
/*!
 * \brief Generic receiver and function that may block or intercept a sending signal
 *
 * Receiver is first, to compare with \ref mcr_ref_compare.
 */
struct mcr_DispatchPair {
	/*! \brief Object to receive signal */
	void *receiver;
	/*! \brief Function to act on receiver */
	mcr_Dispatcher_receive_fnc dispatch;
};
/*! Compare \ref mcr_DispatchPair, only compare receivers. */
#define mcr_DispatchPair_compare mcr_ref_compare

/*!
 * \brief Helper interface to create \ref mcr_Array
 * of \ref mcr_DispatchPair structures.
 *
 * Dispatch maps should always end with this array.
 */
MCR_API const struct mcr_Interface *mcr_Array_DispatchPair_iface();

#endif
