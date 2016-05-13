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
 * \brief Receive dispatch and trigger actions.
 *
 * Receive dispatch with \ref mcr_Trigger_receive_fnc, which will
 * redirect to \ref mcr_ITrigger.receive.
 */

#ifndef MCR_TRIGGER_H
#define MCR_TRIGGER_H

#include "mcr/macro/itrigger.h"

/*! \brief Register to be triggered for \ref mcr_Signal or modifiers */
struct mcr_Trigger {
	/* Instance */
	union {
		const struct mcr_ITrigger *itrig;
		struct mcr_Instance inst;
		const struct mcr_Interface *iface;
	};
	/* Trigger specific */
	/*!
	 * \brief Function to call when trigger is completed.
	 *
	 * First argument is the \ref mcr_Trigger object.
	 */
	mcr_Dispatcher_receive_fnc trigger;
	/*! \brief Object which receives trigger action */
	void *actor;
	/*!
	 * \brief 0 to not block intercepted signals, otherwise do
	 * block intercepted signals
	 bool block;
	 */
};
/*!
 * \brief \ref mcr_Dispatcher_receive_fnc for triggers.
 *
 * This is the default receiver function, when none is provided to
 * add dispatcher receiver.
 */
MCR_API bool mcr_Trigger_receive(void *trigReceiverPt,
	struct mcr_Signal *dispatchSignal, unsigned int mods);

/*! \brief Data interface of trigger structures. */
MCR_API const struct mcr_Interface *mcr_Trigger_iface();

/*! ctor */
MCR_API void mcr_Trigger_init(void *trigDataPt);
MCR_API void mcr_Trigger_set_all(struct mcr_Trigger *trigPt,
	mcr_Dispatcher_receive_fnc trigger, void *actor);
MCR_API void mcr_Trigger_free(void *trigDataPt);
#define mcr_Trigger_free_foreach(trigPt, ignore) \
mcr_Trigger_free(trigPt)

MCR_API int mcr_Trigger_copy(void *dstPt, void *srcPt);
MCR_API int mcr_Trigger_compare(const void *lhsTrigPt, const void *rhsTrigPt);
MCR_API int mcr_Triggerref_compare(const void *lhsPtPt, const void *rhsPtPt);

#endif
