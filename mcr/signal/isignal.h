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
 *  \brief \ref mcr_ISignal - Interface for signals.
 *
 *  Use signals with \ref mcr_send.
 */

#ifndef MCR_SIGNAL_ISIGNAL_H_
#define MCR_SIGNAL_ISIGNAL_H_

#include "mcr/signal/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Types */
struct mcr_Signal;
struct mcr_Dispatcher;
/*! Function to send or dispatch signal
 *
 *  \param signalPt Signal to send
 *  \return \ref reterr for actions with errors, 0 for no error
 */
typedef int (*mcr_signal_fnc) (struct mcr_Signal * signalPt);

/*! Interface for signal types
 *
 *  Logically a signal should be anything that can be dispatched
 *  to another entity, or sent to cause an action.
 */
struct mcr_ISignal {
	/*! Common members for interface registration */
	struct mcr_Interface interface;
	/* Signal interface */
	/*! Intercept before sending. Returns false
	 *  to not block signals from sending, otherwise do block the signal.
	 */
	struct mcr_Dispatcher *dispatcher;
	/*! Function to send, when not interrupted */
	mcr_signal_fnc send;
};

/*! \ref mcr_ISignal ctor
 *
 *  \param isigPt \ref opt \ref mcr_ISignal
 *  \return 0
 */
MCR_API int mcr_ISignal_init(void *isigPt);
/*! \ref mcr_ISignal_init and mcr_ISignal_set_all
 *
 *  \param dispPt \ref opt \ref mcr_ISignal.dispatcher
 *  \param sender \ref opt \ref mcr_ISignal.send
 *  \return Newly created mcr_ISignal
 */
MCR_API struct mcr_ISignal mcr_ISignal_new(struct mcr_Dispatcher *dispPt,
		mcr_signal_fnc sender);
/*! Set initial values
 *
 *  Please use \ref mcr_iset_all for interface values.
 *  \param dispPt \ref opt \ref mcr_ISignal.dispatcher
 *  \param sender \ref opt \ref mcr_ISignal.send
 */
MCR_API void mcr_ISignal_set_all(struct mcr_ISignal *isigPt,
								 struct mcr_Dispatcher *dispPt, mcr_signal_fnc sender);

/*! Get the \ref mcr_IRegistry of \ref mcr_ISignal */
MCR_API struct mcr_IRegistry *mcr_ISignal_reg(struct mcr_context *ctx);
/*! Get the id of a mcr_ISignal.
 *
 *  \param isigPt \ref opt \ref mcr_ISignal *
 *  \return \ref retid
 */
#define mcr_ISignal_id(isigPt) mcr_iid(isigPt)
/*! Get the name of a mcr_ISignal.
 *
 *  \param isigPt \ref opt
 *  \return Name of the mcr_ISignal, or null if not found
 */
MCR_API const char *mcr_ISignal_name(struct mcr_context *ctx,
									 struct mcr_ISignal *isigPt);
/*! Get a \ref mcr_ISignal from its id
 *
 *  \param id Id of the signal interface
 *  \return Signal interface, or null if not found
 */
MCR_API struct mcr_ISignal *mcr_ISignal_from_id(struct mcr_context *ctx,
		size_t id);
/*! Get a \ref mcr_ISignal from its name
 *
 *  \param name \ref opt Name of the signal interface
 *  \return Signal interface, or null if not found
 */
MCR_API struct mcr_ISignal *mcr_ISignal_from_name(struct mcr_context *ctx,
		const char *name);

#ifdef __cplusplus
}
#endif

#endif
