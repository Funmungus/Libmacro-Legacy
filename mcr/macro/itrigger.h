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
 * \brief mcr_ITrigger, interface for triggers.
 *
 * Use \ref mcr_Dispatcher_receive_fnc to dispatch into triggers.
 */

#ifndef MCR_ITRIGGER_H
#define MCR_ITRIGGER_H

#include "mcr/macro/def.h"

/*! \brief Interface type for \ref mcr_Trigger */
struct mcr_ITrigger {
	struct mcr_Interface iface;
	/* Trigger interface */
	/*!
	 * \brief Filter dispatch before triggering.
	 *
	 * This is to dispatch and do required logic before the final
	 * trigger.  It cannot be NULL.  First argument is \ref mcr_Trigger.
	 */
	mcr_Dispatcher_receive_fnc receive;
};

/* Trigger type development. */
MCR_API void mcr_ITrigger_init(void *itrigDataPt);

MCR_API struct mcr_IRegistry *mcr_ITrigger_reg(struct mcr_context *ctx);
MCR_API struct mcr_ITrigger *mcr_ITrigger_from_id(struct mcr_context *ctx,
	size_t id);
MCR_API struct mcr_ITrigger *mcr_ITrigger_from_name(struct mcr_context *ctx,
	const char *name);

#endif
