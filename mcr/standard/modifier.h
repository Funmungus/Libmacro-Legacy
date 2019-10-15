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
 *  \brief \ref mcr_Modifier - Signal to modify internal modifiers
 */

#ifndef MCR_STANDARD_MODIFIER_H_
#define MCR_STANDARD_MODIFIER_H_

#include "mcr/standard/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Pair of modifier bits and whether to set or release those
 *  modifiers
 */
struct mcr_Modifier {
	/*! Bit values representing a set of modifiers. */
	unsigned int modifiers;
	/*! \ref MCR_UNSET will set the modifier, \ref MCR_TOGGLE
	 *  will change the state from what it currently is, otherwise it is
	 *  released */
	enum mcr_ApplyType apply;
};

/*! Set initial values */
MCR_API void mcr_Modifier_set_all(struct mcr_Modifier *modPt,
								  unsigned int modifiers,
								  enum mcr_ApplyType applyType);
/*! \pre Signal data is \ref mcr_Modifier
 *  \brief Modify internal modifiers
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Modifier_send(struct mcr_Signal *sigPt);
/* Default init, deinit, compare, and copy */

/*! Modify an instance of \ref mcr_Modifier
 *
 *  \param modifier Modifiers to set or release
 *  \param applyModifier Set or release modifiers
 */
MCR_API void mcr_Modifier_modify(struct mcr_Modifier *modPt,
								 unsigned int modifier, enum mcr_ApplyType applyModifier);

/*! Get the signal interface for \ref mcr_Modifier */
MCR_API struct mcr_ISignal *mcr_iModifier(struct mcr_context *ctx);
/*! Get modifier data of a signal */
#define mcr_Modifier_data( sigPt ) \
mcr_castpt(struct mcr_Modifier, mcr_Instance_data(sigPt))
/*! Get modifier data of a signal */
#define MCR_MODIFIER_DATA(signal) \
mcr_castpt(struct mcr_Modifier, (signal).instance.data->data)

#ifdef __cplusplus
}
#endif

#endif
