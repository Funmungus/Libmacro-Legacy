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
 * \brief Include all Libmacro functionality
 */

/*! \namespace mcr
 * \brief Libmacro, by Jonathan Pelletier 2013.  Alpha version.
 *
 * 1. \ref mcr_Signal is dispatched to \ref mcr_Dispatcher using
 * \ref mcr_dispatch.\n
 * 2. If signal is not blocked by dispatching, it is then sent to cause
 * action.\n
 * 3. Dispatching may be received by \ref mcr_DispatchPair.\n
 * 4. \ref mcr_Trigger_receive may be used to dispatch into \ref mcr_Trigger.
 * 5. Triggered action may be a \ref mcr_Macro, which sends a list of
 * \ref mcr_Signal.
 * 6. Repeat at step 1, or disable signal dispatch by
 * setting either \ref mcr_ISignal.dispatcher, or \ref mcr_Signal.is_dispatch
 * to 0.\n
 */

#ifndef MCR_LIBMACRO_H
#define MCR_LIBMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mcr/util/util.h"
#include "mcr/signal/signal.h"
#include "mcr/macro/macro.h"
#include "mcr/standard/standard.h"
#include "mcr/intercept/intercept.h"
#ifdef MCR_EXTRAS
#include "mcr/extras/extras.h"
#endif

/*! \brief Libmacro context, required for Libmacro functions */
struct mcr_context;
/*! \brief \ref malloc and \ref mcr_initialize
 *
 * \param flagLoadContracts If true, also load string contracts in
 * all modules.  Useful for string-mapped or scripted applications.
 * \param flagTrimFinish If true, \ref mcr_trim after initializing.
 * \return Initialized Libmacro context, or NULL on error
 */
MCR_API struct mcr_context *mcr_allocate(bool flagLoadContracts,
		bool flagTrimFinish);
/*! \brief \ref mcr_deinitialize and \ref free
 *
 * Because of threading do not deallocate in a deconstructor or on program
 * exit.
 * \param ctx Libmacro context
 */
MCR_API int mcr_deallocate(struct mcr_context *ctx);
/*! \brief Initialize Libmacro resources
 *
 * \param ctx Libmacro context
 * \return \ref reterr
 */
MCR_API int mcr_initialize(struct mcr_context *ctx,
			   bool flagLoadContracts, bool flagTrimFinish);
/*! \brief Clean all resources used by Libmacro.
 *
 * Because of threading do not deinitialize in a deconstructor or on program
 * exit.
 * \param ctx Libmacro context
 * \return \ref reterr
 */
MCR_API int mcr_deinitialize(struct mcr_context *ctx);
/*! \brief Load string contracts
 *
 * String contracts map string names or keys to types and instances.
 * \param ctx Libmacro context
 * \return \ref reterr
 */
MCR_API int mcr_load_contracts(struct mcr_context *ctx);
/*! \brief Minimize allocation used by Libmacro.
 *
 * \param ctx Libmacro context
 */
MCR_API void mcr_trim(struct mcr_context *ctx);

#ifndef MCR_PLATFORM_INC
/*! \brief Include this file to access platform declarations.
 *
 * In case of emergency break glass
 */
#define MCR_PLATFORM_INC MCR_STR(mcr/MCR_PLATFORM_DIR/nlibmacro.h)
#endif

#ifdef __cplusplus
}
#endif
#endif
