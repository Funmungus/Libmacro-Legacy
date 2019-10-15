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
 *  \brief \ref mcr_HidEcho - Simulate human interface with spatial
 *  coordinates.
 */

#ifndef MCR_STANDARD_HID_ECHO_H_
#define MCR_STANDARD_HID_ECHO_H_

#include "mcr/standard/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Simulate human interface with spatial position activation. */
struct mcr_HidEcho {
	/*! \ref \ref retind Echo code */
	size_t echo;
};
/*! \ref mcr_HidEcho shorthand */
typedef struct mcr_HidEcho mcr_Echo;

/*! \pre Signal data is \ref mcr_HidEcho
 *  \brief Send human interface signal
 *
 *  \param signalPt Has data member \ref mcr_HidEcho
 *  \return \ref reterr
 */
MCR_API int mcr_HidEcho_send(struct mcr_Signal *signalPt);
/*! \ref mcr_HidEcho_send
 *
 *  \ref mcr_is_platform
 *  \return \ref reterr
 */
MCR_API int mcr_HidEcho_send_data(struct mcr_HidEcho *dataPt);
/* Default init, deinit, copy, compare */

/* Echo names */
/*! Get event code from its name.
 *
 *  \param eventName \ref opt Name of echo code
 *  \return Echo code, or \ref MCR_ECHO_ANY if it is not found
 */
MCR_API size_t mcr_HidEcho_name_echo(struct mcr_context *ctx,
										const char *eventName);
/*! Get the name of an echo code.
 *
 *  \param eventCode \ref mcr_HidEcho.echo
 *  \return Echo name, or null if not found
 */
MCR_API const char *mcr_HidEcho_name(struct mcr_context *ctx,
									 size_t eventCode);
/*! Get the number of registered echo codes. */
MCR_API size_t mcr_HidEcho_count(struct mcr_context *ctx);

/* Code development */
/*! Set the name mapped by an echo code.
 *
 *  The name will also map to this echo code.
 *  \param eventcode Echo code to map
 *  \param eventName Name to map
 *  \return \ref reterr
 */
MCR_API int mcr_HidEcho_set_name(struct mcr_context *ctx, size_t eventCode,
								 const char *eventName);
/*! Add a set of names to map to an echo code.
 *
 *  \param eventCode Event code to add names for
 *  \param addNames \ref opt Set of names to add
 *  \param bufferLen Number of names to add
 *  \return \ref reterr
 */
MCR_API int mcr_HidEcho_add(struct mcr_context *ctx, size_t eventCode,
							const char **addNames, size_t bufferLen);
/*! \ref mcr_Echo_set_name and \ref mcr_Echo_add_names
 *
 *  \return \ref reterr
 */
MCR_API int mcr_HidEcho_map(struct mcr_context *ctx, size_t eventCode,
							const char *eventName, const char **addNames, size_t bufferLen);
/*! Rename an echo code
 *
 *  \return \ref reterr
 */
MCR_API int mcr_HidEcho_reecho(struct mcr_context *ctx, size_t eventCode,
							   size_t newCode);
/*! Rename an echo code. The code is first found from its
 *  old name.
 *
 *  \return \ref reterr
 */
MCR_API int mcr_HidEcho_rename(struct mcr_context *ctx, const char *oldName,
							   const char *newName);
/*! Minimize name allocation */
MCR_API void mcr_HidEcho_trim(struct mcr_context *ctx);
/*! Remove all mapped echo codes. */
MCR_API void mcr_HidEcho_clear(struct mcr_context *ctx);

/* mcr_HidEcho -> mcr_Echo */
/*! \ref mcr_HidEcho_send */
#define mcr_Echo_send mcr_HidEcho_send
/*! \ref mcr_HidEcho_send_data */
#define mcr_Echo_send_data mcr_HidEcho_send_data
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

/*! Get the \ref mcr_ISignal of \ref mcr_HidEcho */
MCR_API struct mcr_ISignal *mcr_iHidEcho(struct mcr_context *ctx);
/*! Get the \ref mcr_ISignal of \ref mcr_HidEcho */
#define mcr_iEcho mcr_iHidEcho
/*! Signal data casted \ref mcr_HidEcho * */
#define mcr_HidEcho_data(sigPt) \
mcr_castpt(struct mcr_HidEcho, mcr_Instance_data(sigPt))
/*! Signal data casted \ref mcr_HidEcho * */
#define mcr_Echo_data(sigPt) mcr_HidEcho_data(sigPt)

#ifdef __cplusplus
}
#endif

#endif
