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
 * \brief \ref mcr_HidEcho
 */

#ifndef MCR_HID_ECHO_H
#define MCR_HID_ECHO_H

#include "mcr/standard/def.h"

/*!
 * \brief Simulate mouse click, screen tap, etc.
 * spatial position activation
 */
struct mcr_HidEcho;
/*! \brief \ref mcr_HidEcho shorthand */
typedef struct mcr_HidEcho mcr_Echo;
/*!
 * \brief Initialize \ref mcr_HidEcho
 *
 * Native
 */
MCR_API int mcr_HidEcho_init(void *echoPt);
/*!
 * \brief Get echo code
 * \return Echo event code
 */
MCR_API int mcr_HidEcho_echo(const struct mcr_HidEcho *echoPt);
/*!
 * \brief Set echo code
 * \param echo Echo event code
 */
MCR_API void mcr_HidEcho_set_echo(struct mcr_HidEcho *echoPt, int echo);
/*!
 * \brief Send human interface impersonation.
 *
 * \param signalData Has data member \ref mcr_HidEcho
 * \return \ref reterr
 */
MCR_API int mcr_HidEcho_send(struct mcr_Signal *signalData);
/*!
 * \brief \ref mcr_HidEcho_send
 *
 * Native
 */
MCR_API int mcr_HidEcho_send_data(struct mcr_HidEcho *dataPt);
/*! \brief Compare \ref mcr_HidEcho */
MCR_API int mcr_HidEcho_compare(const void *lhs, const void *rhs);
/*! \brief Copy \ref mcr_HidEcho */
MCR_API int mcr_HidEcho_copy(void *dstPt, void *srcPt);
/* Default deinit */

/* Echo names */
/*!
 * \brief Get event code from its name.
 *
 * \param eventName Name of echo code
 * \return Echo code, or \ref MCR_ECHO_ANY if it is not registered
 */
MCR_API int mcr_HidEcho_name_echo(struct mcr_context *ctx,
	const char *eventName);
/*! \brief Get the name of an echo code. */
MCR_API const char *mcr_HidEcho_name(struct mcr_context *ctx, int eventCode);
/*! \brief Get the number of registered echo codes. */
MCR_API size_t mcr_HidEcho_count(struct mcr_context *ctx);

/* Code development */
/*!
 * \brief Set the name mapped by an echo code.
 *
 * The name will also map to this echo code.
 * \return \ref reterr
 */
MCR_API int mcr_HidEcho_set_name(struct mcr_context *ctx, int eventCode,
	const char *eventName);
/*!
 * \brief Add a set of names to map to an echo code.
 *
 * \return \ref reterr
 */
MCR_API int mcr_HidEcho_add(struct mcr_context *ctx, int eventCode,
	const char **addNames, size_t bufferLen);
/*!
 * \brief \ref mcr_Echo_set_name and \ref mcr_Echo_add_names
 *
 * \return \ref reterr
 */
MCR_API int mcr_HidEcho_map(struct mcr_context *ctx, int eventCode,
	const char *eventName, const char **addNames, size_t bufferLen);
/*!
 * \brief Rename an echo code
 *
 * \return \ref reterr
 */
MCR_API int mcr_HidEcho_reecho(struct mcr_context *ctx, int eventCode,
	int newCode);
/*!
 * \brief Rename an echo code. The code is first found from its
 * old name.
 *
 * \return \ref reterr
 */
MCR_API int mcr_HidEcho_rename(struct mcr_context *ctx, const char *oldName,
	const char *newName);
MCR_API void mcr_HidEcho_trim(struct mcr_context *ctx);
/*! \brief Remove all mapped echo codes. */
MCR_API void mcr_HidEcho_clear(struct mcr_context *ctx);

/* mcr_HidEcho -> mcr_Echo */
/*! \brief \ref mcr_HidEcho_init */
#define mcr_Echo_init mcr_HidEcho_init
/*! \brief \ref mcr_HidEcho_echo */
#define mcr_Echo_echo mcr_HidEcho_echo
/*! \brief \ref mcr_HidEcho_set_echo */
#define mcr_Echo_set_echo mcr_HidEcho_set_echo
/*! \brief \ref mcr_HidEcho_send */
#define mcr_Echo_send mcr_HidEcho_send
/*! \brief \ref mcr_HidEcho_send_data */
#define mcr_Echo_send_data mcr_HidEcho_send_data
/*! \brief \ref mcr_HidEcho_compare */
#define mcr_Echo_compare mcr_HidEcho_compare
/*! \brief \ref mcr_HidEcho_copy */
#define mcr_Echo_copy mcr_HidEcho_copy
#define mcr_Echo_name_echo mcr_HidEcho_name_echo
#define mcr_Echo_name mcr_HidEcho_name
#define mcr_Echo_count mcr_HidEcho_count
#define mcr_Echo_set_name mcr_HidEcho_set_name
#define mcr_Echo_add mcr_HidEcho_add
#define mcr_Echo_map mcr_HidEcho_map
#define mcr_Echo_reecho mcr_HidEcho_reecho
#define mcr_Echo_rename mcr_HidEcho_rename
#define mcr_Echo_trim mcr_HidEcho_trim
#define mcr_Echo_clear mcr_HidEcho_clear

MCR_API struct mcr_ISignal *mcr_iHidEcho(struct mcr_context *ctx);
#define mcr_iEcho mcr_iHidEcho
/*! \brief Signal data casted \ref mcr_HidEcho * */
#define mcr_HidEcho_data(sigPt) \
((struct mcr_HidEcho *)mcr_Instance_data(sigPt))
/*! \brief Signal data casted \ref mcr_HidEcho * */
#define MCR_HIDECHO_DATA(sig) \
((struct mcr_HidEcho *)(sig).instance.data.data)

/*! \brief Signal data casted \ref mcr_HidEcho * */
#define mcr_Echo_data(sigPt) mcr_HidEcho_data(sigPt)
/*! \brief Signal data casted \ref mcr_HidEcho * */
#define MCR_ECHO_DATA(sig) MCR_HIDECHO_DATA(sig)

#endif
