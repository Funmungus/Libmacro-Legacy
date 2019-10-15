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
 *  \brief \ref mcr_Signal - Signal to send or dispatch
 *  \ref mcr_signal - Libmacro signal module
 *
 *  See \ref mcr_send
 *  See \ref mcr_dispatch
 */

#ifndef MCR_SIGNAL_SIGNAL_H_
#define MCR_SIGNAL_SIGNAL_H_

#include "mcr/signal/mod_flags.h"
#include "mcr/signal/generic_dispatcher.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Use functions in \ref mcr_ISignal to dispatch and
 *  send the whole object, mcr_ISignal + data members.
 */
struct mcr_Signal {
	/*! \ref mcr_Instance of \ref mcr_ISignal */
	union {
		struct mcr_ISignal *isignal;
		struct mcr_Instance instance;
		struct mcr_Interface *interface;
	};
	/* Signal specific */
	/*! False to not dispatch, otherwise do dispatch */
	bool is_dispatch;
};

/*! Data interface of mcr_Signal structures
 *
 *  \return Interface to manage mcr_Signal objects
 */
MCR_API const struct mcr_Interface *mcr_Signal_interface();

/*! \ref mcr_Signal ctor
 *
 *  \param signalPt \ref opt \ref mcr_Signal *
 *  \return 0
 */
MCR_API int mcr_Signal_init(void *signalPt);
/*! \ref mcr_Signal dtor
 *
 *  \param signalPt \ref opt \ref mcr_Signal *
 *  \return 0
 */
MCR_API int mcr_Signal_deinit(void *signalPt);
/*! Dispatch, and then call the signal interface send function.
 *
 *  If dispatch blocks, the interface send function will not be called.
 *  \param signalPt \ref opt Signal to dispatch and send
 *  \return \ref reterr
 */
MCR_API int mcr_send(struct mcr_context *ctx, struct mcr_Signal *signalPt);

/*! \ref mcr_Instance_copy, then copy \ref mcr_Signal.is_dispatch
 *
 *  \param dstPt \ref mcr_Signal * Destination to copy to
 *  \param srcPt \ref opt \ref mcr_Signal * Source to copy from
 *  \return \ref reterr
 */
MCR_API int mcr_Signal_copy(void *dstPt, const void *srcPt);
/*! Compare two signals
 *
 *  \param lhsSignalPt \ref opt mcr_Signal *
 *  \param rhsSignalPt \ref opt mcr_Signal *
 *  \return \ref retcmp
 */
MCR_API int mcr_Signal_compare(const void *lhsSignalPt,
							   const void *rhsSignalPt);
/*! Compare two signal references
 *
 *  \param lhsPtPt \ref opt \ref mcr_Signal **
 *  \param rhsPtPt \ref opt \ref mcr_Signal **
 *  \return \ref retcmp
 */
MCR_API int mcr_Signalref_compare(const void *lhsPtPt, const void *rhsPtPt);

/* In cases of extreme complexity please break glass. */

/*! Libmacro signal module
 *
 *  In cases of extreme complexity please break glass.
 */
struct mcr_signal {
	/*! \ref mcr_ISignal registry */
	struct mcr_IRegistry isignals;
	/*! Set of \ref mcr_Dispatcher * for each signal */
	struct mcr_Array dispatchers;
	/*! If enabled, the generic dispatcher will be used for all
	 *  signals */
	bool is_generic_dispatcher;
	/*! Generic dispatcher which may be used after the specific
	 *  dispatcher has been called */
	struct mcr_Dispatcher *generic_dispatcher_pt;
	/*! Default generic dispatcher with logic for all dispatches,
	 *  and signal reference-specific.
	 */
	struct mcr_GenericDispatcher generic_dispatcher;
	/*! All modifiers known by Libmacro to be set */
	unsigned int internal_modifiers;
	/*! Map from modifiers to names */
	struct mcr_Map map_modifier_name;
	/*! Map from names to modifiers */
	struct mcr_Map map_name_modifier;
};

#ifdef __cplusplus
}
#endif

#endif
