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

/*!
 * \file
 * \brief \ref mcr_mod_standard - Standard signal and trigger types module
 *
 * In cases of extreme complexity, please break glass.
 */

#ifndef MCR_STANDARD_MOD_STANDARD_H
#define MCR_STANDARD_MOD_STANDARD_H

#include "mcr/macro/macro.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mcr_IsStage;
struct mcr_mod_standard {
	/* Signal types */
	struct mcr_ISignal ihid_echo;
	struct mcr_ISignal ikey;
	struct mcr_CtxISignal imods;
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
	/*! \brief Set of \ref mcr_IsStage */
	struct mcr_Array stage_matchers;
	/*!
	 * \brief Stage matcher for no specific type
	 *
	 * By default this matches signal by address */
	struct mcr_IsStage *stage_generic;
	/* name reg */
	struct mcr_StringIndex key_name_index;
	mcr_String key_name_any;
	struct mcr_StringIndex echo_name_index;
	mcr_String echo_name_any;
};

MCR_API int mcr_standard_initialize(struct mcr_context *ctx);
MCR_API int mcr_standard_deinitialize(struct mcr_context *ctx);
MCR_API int mcr_standard_load_contract(struct mcr_context *ctx);
MCR_API void mcr_standard_trim(struct mcr_context *ctx);

/* Platform directory */
#define MCR_STANDARD_PLATFORM_INC \
MCR_STR(mcr/standard/MCR_PLATFORM/nstandard.h)
/*!
 * \brief Initialize platform requirements for standard signals.
 *
 * Called by \ref mcr_standard_initialize
 */
MCR_API int mcr_standard_platform_initialize(struct mcr_context *ctx);
/*!
 * \brief Clean up platform requirements for standard signals.
 *
 * Called by \ref mcr_standard_deinitialize
 */
MCR_API int mcr_standard_platform_deinitialize(struct mcr_context *ctx);
MCR_API int mcr_standard_platform_load_contract(struct mcr_context *ctx);

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
