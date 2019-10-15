/* Libmacro - A multi-platform, extendable macro and hotkey C library
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
 *  \brief \ref mcr_Mods - Signal to modify internal modifiers
 */

#ifndef MCR_STANDARD_MODS_H_
#define MCR_STANDARD_MODS_H_

#include "mcr/standard/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Pair of modifier bits and whether to set or release those
 *  modifiers
 */
struct mcr_Mods {
	/*! Bit values representing a set of modifiers. */
	unsigned int modifiers;
	/*! \ref MCR_DOWN will set the modifier, \ref MCR_TOGGLE
	 *  will change the state from what it currently is, otherwise it is
	 *  released */
	enum mcr_KeyUpType up_type;
};

/*! Set initial values */
MCR_API void mcr_Mods_set_all(struct mcr_Mods *modPt, unsigned int modifiers,
							  enum mcr_KeyUpType upType);
/*! \pre Signal data is \ref mcr_Mods
 *  \brief Modify internal modifiers
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Mods_send(struct mcr_Signal *sigPt);
/* Default init, deinit, compare, and copy */

/*! Modify an instance of \ref mcr_Mods
 *
 *  \param modifier Modifiers to set or release
 *  \param modifierKeyUp Set or release modifiers
 */
MCR_API void mcr_Mods_modify(struct mcr_Mods *modPt,
							 unsigned int modifier, enum mcr_KeyUpType modifierKeyUp);

/*! Get the signal interface for \ref mcr_mods */
MCR_API struct mcr_ISignal *mcr_iMods(struct mcr_context *ctx);
/*! Get mods data of a signal */
#define mcr_Mods_data( sigPt ) \
((struct mcr_Mods *)mcr_Instance_data(sigPt))
/*! Get mods data of a signal */
#define MCR_MODS_DATA(signal) \
((struct mcr_Mods *)(signal).instance.data->data)

#ifdef __cplusplus
}
#endif

#endif
