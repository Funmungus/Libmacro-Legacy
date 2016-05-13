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
 * \brief \ref mcr_Action
 */

#ifndef MCR_ACTION_H
#define MCR_ACTION_H

#include "mcr/standard/trigger_flags.h"

struct mcr_Action {
	unsigned int modifiers;
	int trigger_flags;
};

MCR_API void mcr_Action_init(void *actDataPt);
/* Default compare, copy, and free */
MCR_API bool mcr_Action_receive(void *actTrigPt, struct mcr_Signal *sigPt,
	unsigned int mods);

MCR_API struct mcr_ITrigger *mcr_iAction(struct mcr_context *ctx);
#define mcr_Action_data(trigPt) \
((struct mcr_Action *)mcr_inst_data(trigPt))

#endif
