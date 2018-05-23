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
 * \brief Standard signal and trigger types
 *
 * \ref mcr_Signal data types: \ref mcr_HidEcho,
 * \ref mcr_Key, \ref mcr_Mods, \ref mcr_MoveCursor, \ref mcr_NoOp,
 * and \ref mcr_Scroll \n
 * \ref mcr_ISignal: \ref mcr_iHidEcho, \ref mcr_iKey,
 * \ref mcr_iMoveCursor, \ref mcr_iNoOp, and \ref mcr_iScroll\n
 * \n
 * \ref mcr_Trigger date types: \ref mcr_Action, \ref mcr_Staged\n
 * \ref mcr_ITrigger: \ref mcr_iAction, \ref mcr_iStaged
 */

#ifndef MCR_STANDARD_STANDARD_H_
#define MCR_STANDARD_STANDARD_H_

#include "mcr/standard/def.h"
#include "mcr/standard/hid_echo.h"
#include "mcr/standard/key.h"
#include "mcr/standard/mods.h"
#include "mcr/standard/move_cursor.h"
#include "mcr/standard/noop.h"
#include "mcr/standard/scroll.h"
#include "mcr/standard/action.h"
#include "mcr/standard/staged.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Some extra, possibly useful, utilities */
/*! For both positions, either may be 0, or the same coordinate
 * array member must either be both negative, or both positive.
 */
MCR_API bool mcr_resembles_justified(const mcr_Dimensions first,
									 const mcr_Dimensions second);
/*! For both positions, the same coordinate array member must
 * be valued within the measurementError.
 */
MCR_API bool mcr_resembles_absolute(const mcr_Dimensions first,
									const mcr_Dimensions second, const unsigned int measurementError);

#ifdef __cplusplus
}
#endif

#endif
