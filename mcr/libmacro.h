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
 *  \brief Include all Libmacro functionality.
 */

/*! \namespace mcr
 *  \brief Libmacro, by Jonathan Pelletier, New Paradigm Software. Alpha version.
 *
 *  1. \ref mcr_Signal is dispatched to \ref mcr_Dispatcher using \ref mcr_dispatch.\n
 *  		1.0.a Disable dispatch for a signal by setting \ref mcr_Signal.is_dispatch to false.\n
 *  		1.0.b Disable dispatch of an ISignal type by setting \ref mcr_ISignal.dispatcher to NULL.\n
 *  		1.0.c Disable Libmacro generic dispatch (listen to all types) by setting \ref mcr_context.signal.is_generic_dispatcher to false.\n
 *  	1.1 Dispatching may be received by \ref mcr_DispatchPair.\n
 *  	1.2 \ref mcr_Trigger_receive may be used to dispatch into \ref mcr_Trigger.\n
 *  	1.3 Triggered action may be a \ref mcr_Macro, which sends a list of \ref mcr_Signal.\n
 *  2. If signal is not blocked by dispatching, it is then sent to cause an action.\n
 */

/// \todo Separate context and functions to context.h?

#ifndef MCR_LIBMACRO_H_
#define MCR_LIBMACRO_H_

#include "mcr/util/util.h"
#include "mcr/signal/signal.h"
#include "mcr/macro/macro.h"
#include "mcr/standard/standard.h"
#include "mcr/intercept/intercept.h"

#ifdef __cplusplus
#include "mcr/extras/extras.h"
extern "C" {
#endif

/*! Libmacro library context, required for Libmacro functions
 *
 *  In cases of extreme complexity please break glass.
 */
struct mcr_context {
	struct mcr_signal signal;
	struct mcr_macro macro;
	struct mcr_standard standard;
	struct mcr_intercept intercept;
};

/*! \ref malloc and \ref mcr_initialize a \ref mcr_context
 *
 *  Will also \ref mcr_load_contracts and \ref mcr_trim.
 *  Will set \ref mcr_err.
 *  \return Dynamic and initialized Libmacro context, or NULL on error
 */
MCR_API struct mcr_context *mcr_allocate();
/*! \ref mcr_deinitialize and \ref free.
 *
 *  Only use with a context created by malloc or \ref mcr_allocate.
 *  Because of threading do not deallocate in a deconstructor or on program
 *  exit.
 *  Will set \ref mcr_err.
 *  \param ctx Libmacro context
 *  \return \ref reterr
 */
MCR_API int mcr_deallocate(struct mcr_context *ctx);
/*! Initialize Libmacro resources
 *
 *  Will set \ref mcr_err.
 *  \param ctx Libmacro context
 *  \return \ref reterr
 */
MCR_API int mcr_initialize(struct mcr_context *ctx);
/*! Clean all resources used by Libmacro.
 *
 *  Because of threading do not deinitialize in a deconstructor or on program
 *  exit.
 *  Will set \ref mcr_err.
 *  \param ctx Libmacro context
 *  \return \ref reterr
 */
MCR_API int mcr_deinitialize(struct mcr_context *ctx);
/*! Load string contracts
 *
 *  String contracts map string names or keys to types and instances.
 *  Will set \ref mcr_err.
 *  \param ctx Libmacro context
 *  \return \ref reterr
 */
MCR_API int mcr_load_contracts(struct mcr_context *ctx);
/*! Minimize allocation used by Libmacro.
 *
 *  \param ctx Libmacro context
 */
MCR_API void mcr_trim(struct mcr_context *ctx);

#ifdef __cplusplus
}
#endif

#endif
