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
 * \brief Interface for signals, and signal structure of
 * interface and data. Use signal with \ref mcr_send.
 */

#ifndef MCR_ISIGNAL_H
#define MCR_ISIGNAL_H

#include "mcr/signal/def.h"

/* Types */
struct mcr_Signal;
struct mcr_Dispatcher;
/*!
 * \brief Function to send or dispatch signal
 *
 * \param signalData Signal to send
 * \return \ref reterr for actions with errors
 */
typedef int (*mcr_signal_fnc) (struct mcr_Signal * signalData);

/*!
 * \brief Signal interface, common members for unique signal type
 *
 * Logically a signal should be anything that can be dispatched
 * to another entity, or sent to cause an action.
 */
struct mcr_ISignal {
	/*! \brief Common members for interface registration */
	struct mcr_Interface iface;
	/* Internal */
	/*!
	 * \brief Intercept before sending. Returns 0
	 * to not block signals from sending, otherwise do block the signal.
	 */
	struct mcr_Dispatcher *dispatcher;
	/*! \brief Function to send, when not interrupted */
	mcr_signal_fnc send;
};

/* Signal development: Add signal type to end, get signal type, */
/*! ctor */
MCR_API void mcr_ISignal_init(void *data);
/*!
 * \brief Set all values of isignal.
 *
 * Please use \ref mcr_iset_all for interface values.
 * \param dispPt \ref mcr_ISignal.dispatcher (opt)
 * \param libmacro \ref mcr_ISignal.libmacro
 * \param sender \ref mcr_ISignal.send (opt)
 */
MCR_API void mcr_ISignal_set_all(struct mcr_ISignal *newType,
	struct mcr_Dispatcher *dispPt, mcr_signal_fnc sender);

MCR_API struct mcr_IRegistry *mcr_ISignal_reg(struct mcr_context *ctx);
MCR_API struct mcr_ISignal *mcr_ISignal_from_id(struct mcr_context *ctx,
	size_t id);
MCR_API struct mcr_ISignal *mcr_ISignal_from_name(struct mcr_context *ctx,
	const char *name);

#endif
