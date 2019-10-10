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
 *  \brief \ref mcr_Action - Conditional trigger from intercepted modifiers
 */

#ifndef MCR_STANDARD_ACTION_H_
#define MCR_STANDARD_ACTION_H_

#include "mcr/standard/trigger_flags.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Conditional trigger from intercepted modifiers */
struct mcr_Action {
	/*! \ref mcr_ModFlags */
	unsigned int modifiers;
	/*! \ref mcr_TriggerFlags */
	unsigned int trigger_flags;
};

/*! \ref mcr_Action ctor
 *
 *  Modifiers set to MCR_MF_NONE and flags set to MCR_TF_ALL
 *  \param actPt \ref opt \ref mcr_Action
 *  \return 0
 */
MCR_API int mcr_Action_init(void *actPt);
/* Default deinit, compare, copy */
/*! \pre Trigger data is \ref mcr_Action
 *  \brief \ref mcr_ITrigger.receive for \ref mcr_Action
 */
MCR_API bool mcr_Action_receive(void *trigPt, struct mcr_Signal *sigPt,
								unsigned int mods);

/*! \ref mcr_ITrigger for \ref mcr_Action */
MCR_API struct mcr_ITrigger *mcr_iAction(struct mcr_context *ctx);
/*! \ref mcr_Action data from \ref mcr_Trigger */
#define mcr_Action_data(trigPt) \
mcr_castpt(struct mcr_Action, mcr_Instance_data(trigPt))

#ifdef __cplusplus
}
#endif

#endif
