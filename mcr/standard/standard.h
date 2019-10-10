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
 *  \brief Standard signal and trigger types
 *
 *  \ref mcr_standard - Standard signal and trigger types module
 *  \ref mcr_Signal data types: \ref mcr_HidEcho,
 *  \ref mcr_Key, \ref mcr_Modifier, \ref mcr_MoveCursor, \ref mcr_NoOp,
 *  and \ref mcr_Scroll \n
 *  \ref mcr_ISignal: \ref mcr_iHidEcho, \ref mcr_iKey,
 *  \ref mcr_iMoveCursor, \ref mcr_iNoOp, and \ref mcr_iScroll\n
 *  \n
 *  \ref mcr_Trigger date types: \ref mcr_Action, \ref mcr_Staged\n
 *  \ref mcr_ITrigger: \ref mcr_iAction, \ref mcr_iStaged
 */

#ifndef MCR_STANDARD_STANDARD_H_
#define MCR_STANDARD_STANDARD_H_

#include "mcr/standard/def.h"
#include "mcr/standard/hid_echo.h"
#include "mcr/standard/key.h"
#include "mcr/standard/modifier.h"
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
 *  array member must either be both negative, or both positive.
 */
MCR_API bool mcr_resembles_justified(const mcr_Dimensions first,
									 const mcr_Dimensions second);
/*! For both positions, the same coordinate array member must
 *  be valued within the measurementError.
 */
MCR_API bool mcr_resembles_absolute(const mcr_Dimensions first,
									const mcr_Dimensions second, const unsigned int measurementError);

struct mcr_IsStage;
/*! Standard signal and trigger types module
 *
 *  In cases of extreme complexity, please break glass. */
struct mcr_standard {
	/* Signal types */
	struct mcr_ISignal ihid_echo;
	struct mcr_ISignal ikey;
	struct mcr_ISignal imodifier;
	struct mcr_ISignal imove_cursor;
	struct mcr_ISignal inoop;
	struct mcr_ISignal iscroll;
	/* Key dispatch */
	struct mcr_CtxDispatcher key_dispatcher;
	/* down, up, generic is set into both */
	struct mcr_Map key_dispatcher_maps[2];
	/* modifier <=> key */
	struct mcr_Map map_key_modifier;
	struct mcr_Map map_modifier_key;
	/* Trigger types */
	struct mcr_ITrigger iaction;
	struct mcr_ITrigger istaged;
	/*! Set of \ref mcr_IsStage */
	struct mcr_Array stage_matchers;
	/*! Stage matcher for no specific type
	 *
	 *  By default this matches signal by address */
	struct mcr_IsStage *stage_generic;
	/* name reg */
	struct mcr_StringIndex key_name_index;
	mcr_String key_name_any;
	struct mcr_StringIndex echo_name_index;
	mcr_String echo_name_any;
};

/* Platform signal */
struct mcr_HidEcho;
struct mcr_Key;
struct mcr_MoveCursor;
struct mcr_Scroll;
MCR_API int mcr_HidEcho_send_data(struct mcr_HidEcho *dataPt);
MCR_API int mcr_Key_send_data(struct mcr_Key *dataPt);
MCR_API int mcr_MoveCursor_send_data(struct mcr_MoveCursor *dataPt);
MCR_API int mcr_Scroll_send_data(struct mcr_Scroll *dataPt);

#ifdef __cplusplus
}
#endif

#endif
