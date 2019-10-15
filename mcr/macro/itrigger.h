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
 *  \brief \ref mcr_ITrigger - Interface for triggers
 *
 *  Use \ref mcr_Dispatcher_receive_fnc to dispatch into triggers.
 */

#ifndef MCR_MACRO_ITRIGGER_H_
#define MCR_MACRO_ITRIGGER_H_

#include "mcr/macro/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Interface for \ref mcr_Trigger */
struct mcr_ITrigger {
	/*! \ref mcr_Trigger interface */
	struct mcr_Interface interface;
	/*! Filter dispatch before triggering.
	 *
	 *  This is to dispatch and do required logic before the final
	 *  trigger.  It cannot be NULL.  First argument is \ref mcr_Trigger. */
	mcr_Dispatcher_receive_fnc receive;
};

/*! \ref mcr_ITrigger ctor
 *
 *  \param itrigPt \ref opt \ref mcr_ITrigger
 *  \return 0
 */
MCR_API int mcr_ITrigger_init(void *itrigPt);
/*! Construct a trigger interface and set \ref mcr_ITrigger.receive
 *
 *  \param receiveFnc \ref mcr_ITrigger.receive
 *  \return Constructed trigger interface
 */
MCR_API struct mcr_ITrigger mcr_ITrigger_new(mcr_Dispatcher_receive_fnc
		receiveFnc);

/*! Get the \ref mcr_IRegistry of \ref mcr_ITrigger */
MCR_API struct mcr_IRegistry *mcr_ITrigger_reg(struct mcr_context *ctx);
/*! Get the id of a trigger interface.
 *
 *  \param itrigPt \ref opt \ref mcr_ITrigger *
 *  \return \ref retid
 */
#define mcr_ITrigger_id(itrigPt) mcr_iid(itrigPt)
/*! Get the name of a trigger interface.
 *
 *  \param itrigPt \ref opt
 *  \return Name of the trigger interface, or null if not found
 */
MCR_API const char *mcr_ITrigger_name(struct mcr_context *ctx,
									  struct mcr_ITrigger *itrigPt);
/*! Get a trigger interface from its id
 *
 *  \param id Id of the trigger interface
 *  \return Trigger interface, or null if not found
 */
MCR_API struct mcr_ITrigger *mcr_ITrigger_from_id(struct mcr_context *ctx,
		size_t id);
/*! Get a trigger interface from its name
 *
 *  \param name \ref opt Name of the trigger interface
 *  \return Trigger interface, or null if not found
 */
MCR_API struct mcr_ITrigger *mcr_ITrigger_from_name(struct mcr_context *ctx,
		const char *name);

#ifdef __cplusplus
}
#endif

#endif
