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
 * \brief In cases of extreme complexity, please break glass.
 */

#ifndef MCR_STANDARD_PRIVATE_H
#define MCR_STANDARD_PRIVATE_H

#include "mcr/util/util.h"

MCR_API int mcr_standard_initialize(struct mcr_context *ctx);
MCR_API void mcr_standard_cleanup(struct mcr_context *ctx);
MCR_API int mcr_standard_load_contract(struct mcr_context *ctx);
MCR_API void mcr_standard_trim(struct mcr_context *ctx);
/* Key-related, but not interface */
MCR_API int mcr_Key_initialize(struct mcr_context *ctx);
MCR_API void mcr_Key_cleanup(struct mcr_context *ctx);
/* Echo-related, but not interface */
MCR_API int mcr_HidEcho_initialize(struct mcr_context *ctx);
MCR_API void mcr_HidEcho_cleanup(struct mcr_context *ctx);

/* Native directory */
#define MCR_STANDARD_NATIVE_INC \
MCR_STR(mcr/standard/MCR_NATIVE_DIR/nstandard.h)
/*!
 * \brief Initialize native requirements for standard signals.
 *
 * Called by \ref mcr_standard_initialize
 */
MCR_API int mcr_standard_native_initialize(struct mcr_context *ctx);
/*!
 * \brief Clean up native requirements for standard signals.
 *
 * Called by \ref mcr_standard_cleanup
 */
MCR_API void mcr_standard_native_cleanup(struct mcr_context *ctx);
/*! \brief Load native key names and key <=> modifier mapping. */
MCR_API int mcr_Key_load_contract(struct mcr_context *ctx);
/*! \brief Load native echo names. */
MCR_API int mcr_HidEcho_load_contract(struct mcr_context *ctx);

/* Native signal */
MCR_API void mcr_HidEcho_init(void *dataPt);
MCR_API int mcr_HidEcho_send_data(struct mcr_HidEcho *dataPt);
MCR_API void mcr_Key_init(void *dataPt);
MCR_API int mcr_Key_send_data(struct mcr_Key *dataPt);
MCR_API void mcr_MoveCursor_init(void *dataPt);
MCR_API int mcr_MoveCursor_send_data(struct mcr_MoveCursor *dataPt);
MCR_API void mcr_Scroll_init(void *dataPt);
MCR_API int mcr_Scroll_send_data(struct mcr_Scroll *dataPt);
/*
 * Native signal data
 * Inline data manipulation macros. Do not define if already defined.
 */
/* mcr_HidEcho */
#ifndef MCR_HIDECHO_ECHO
/*! \ref mcr_HidEcho_echo */
#define MCR_HIDECHO_ECHO(echo)
/*! \ref mcr_HidEcho_set_echo */
#define MCR_HIDECHO_SET_ECHO(echo, echoCode)
/*! \ref MCR_HIDECHO_ECHO */
#define MCR_ECHO_ECHO(echo) MCR_HIDECHO_ECHO(echo)
/*! \ref MCR_HIDECHO_SET_ECHO */
#define MCR_ECHO_SET_ECHO(echo, echoCode) \
MCR_HIDECHO_SET_ECHO(echo, echoCode)
#define MCR_HIDECHO_SET_ALL(echo, echoCode) \
MCR_HIDECHO_SET_ECHO(echo, echoCode)
#define mcr_Echo_load_contract mcr_HidEcho_load_contract

/* mcr_Key */
/*! \ref mcr_Key_key */
#define MCR_KEY_KEY(key)
/*! \ref mcr_Key_set_key */
#define MCR_KEY_SET_KEY(key, keyCode)
/*! \ref mcr_Key_scan */
#define MCR_KEY_SCAN(key)
/*! \ref mcr_Key_set_scan */
#define MCR_KEY_SET_SCAN(key, scan)
/*! \ref mcr_Key_up_type */
#define MCR_KEY_UP_TYPE(key)
/*! \ref mcr_Key_set_up_type */
#define MCR_KEY_SET_UP_TYPE(key, keyUp)
#define MCR_KEY_SET_ALL(key, keyCode, scan, keyUp) \
MCR_KEY_SET_KEY(key, keyCode); \
MCR_KEY_SET_SCAN(key, scan); \
MCR_KEY_SET_UP_TYPE(key, keyUp);
/* mcr_MoveCursor */
/*! \ref mcr_MoveCursor_position */
#define MCR_MOVECURSOR_POSITION(mc, buffer)
/*! \ref mcr_MoveCursor_set_position */
#define MCR_MOVECURSOR_SET_POSITION(mc, buffer)
/*! \ref mcr_MoveCursor_coordinate */
#define MCR_MOVECURSOR_COORDINATE(mc, posType)
/*! \ref mcr_MoveCursor_set_coordinate */
#define MCR_MOVECURSOR_SET_COORDINATE(mc, posType, val)
/*! \ref mcr_MoveCursor_justify */
#define MCR_MOVECURSOR_JUSTIFY(mc)
/*! \ref mcr_MoveCursor_set_justify */
#define MCR_MOVECURSOR_SET_JUSTIFY(mc, enable)
#define MCR_MOVECURSOR_SET_ALL(mc, buffer, isJustify) \
MCR_MOVECURSOR_SET_POSITION(mc, buffer); \
MCR_MOVECURSOR_SET_JUSTIFY(mc, isJustify);
/* mcr_Scroll */
/*! \ref mcr_Scroll_dimensions */
#define MCR_SCROLL_DIMENSIONS(scroll, buffer)
/*! \ref mcr_Scroll_set_dimensions */
#define MCR_SCROLL_SET_DIMENSIONS(scroll, buffer)
/*! \ref mcr_Scroll_coordinate */
#define MCR_SCROLL_COORDINATE(scroll, posType)
/*! \ref mcr_Scroll_set_coordinate */
#define MCR_SCROLL_SET_COORDINATE(scroll, posType, val)
#define MCR_SCROLL_SET_ALL(scroll, buffer) \
MCR_SCROLL_SET_DIMENSIONS(scroll, buffer);
/*! \brief \ref MCR_MOVECURSOR_POSITION */
#define MCR_MC_POSITION(mc, buffer) \
MCR_MOVECURSOR_POSITION(mc, buffer)
/*! \brief \ref MCR_MOVECURSOR_SET_POSITION */
#define MCR_MC_SET_POSITION(mc, buffer) \
MCR_MOVECURSOR_SET_POSITION(mc, buffer)
/*! \brief \ref MCR_MOVECURSOR_COORDINATE */
#define MCR_MC_COORDINATE(mc, posType) \
MCR_MOVECURSOR_COORDINATE(mc, posType)
/*! \brief \ref MCR_MOVECURSOR_SET_COORDINATE */
#define MCR_MC_SET_COORDINATE(mc, posType, val) \
MCR_MOVECURSOR_SET_COORDINATE (mc, posType, val)
/*! \brief \ref MCR_MOVECURSOR_JUSTIFY */
#define MCR_MC_IS_JUSTIFY(mc) MCR_MOVECURSOR_IS_JUSTIFY(mc)
/*! \brief \ref MCR_MOVECURSOR_SET_JUSTIFY */
#define MCR_MC_SET_JUSTIFY(mc, enable) \
MCR_MOVECURSOR_SET_JUSTIFY(mc, enable)
#endif

#endif
