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
 * \brief \ref mcr_Signal
 */

#ifndef MCR_SIGNAL_H
#define MCR_SIGNAL_H

#include "mcr/signal/mod_flags.h"
#include "mcr/signal/dispatcher.h"

/*!
 * \brief Use functions in \ref mcr_ISignal to dispatch and
 * send the whole object, mcr_ISignal + data members.
 */
struct mcr_Signal {
	/*! \brief \ref mcr_Instance of \ref mcr_ISignal */
	union {
		const struct mcr_ISignal *isig;
		struct mcr_Instance inst;
		const struct mcr_Interface *iface;
	};
	/* Signal specific */
	/*! \brief 0 to not dispatch, otherwise do dispatch */
	bool is_dispatch;
};

/*! \brief Data interface of signal structures. */
MCR_API const struct mcr_Interface *mcr_Signal_iface();
/* General use: initializer and send function */
/*! ctor */
MCR_API void mcr_Signal_init(void *sigDataPt);
MCR_API void mcr_Signal_free(void *sigDataPt);
/*! \ref mcr_Signal_free */
#define mcr_Signal_free_foreach(sigPt, ignore) mcr_Signal_free(sigPt)
/*!
 * \brief Cause dispatch, and then call the isignal send function.
 *
 * \return \ref reterr
 */
MCR_API int mcr_send(struct mcr_context *ctx, struct mcr_Signal *sigPt);

/*!
 * \brief \ref mcr_icpy, plus copying isignal reference and
 * \ref mcr_Signal.is_dispatch
 *
 * \param dstPt Destination to copy to
 * \param srcPt Source to copy from
 * \return \ref reterr
 */
MCR_API int mcr_Signal_copy(void *dstPt, void *srcPt);
/*!
 * \brief Compare two signals
 *
 * \param lhsSigPt mcr_Signal *
 * \param rhsSigPt mcr_Signal *
 * \return \ref retcmp
 */
MCR_API int mcr_Signal_compare(const void *lhsSigPt, const void *rhsSigPt);
/*!
 * \brief Compare two signal references
 *
 * \param lhsPtPt mcr_Signal **
 * \param rhsPtPt mcr_Signal **
 * \return \ref retcmp
 */
MCR_API int mcr_Signalref_compare(const void *lhsPtPt, const void *rhsPtPt);

#endif
