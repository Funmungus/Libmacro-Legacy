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
 *  \brief \ref mcr_NoOp - Pause execution in seconds and milliseconds.
 */

#ifndef MCR_STANDARD_NOOP_H_
#define MCR_STANDARD_NOOP_H_

#include "mcr/standard/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Pause execution in seconds and milliseconds. */
struct mcr_NoOp {
	/*! Seconds */
	int sec;
	/*! Milliseconds */
	int msec;
};

/*! Set both seconds and milliseconds. */
MCR_API void mcr_NoOp_set_all(struct mcr_NoOp *noopPt, int sec, int msec);
/*! \pre Signal has data member \ref mcr_NoOp
 *  \brief Pause execution in seconds and milliseconds.
 *
 *  \return \ref reterr
 */
MCR_API int mcr_NoOp_send(struct mcr_Signal *sigPt);
/*! \ref mcr_NoOp_send */
MCR_API int mcr_NoOp_send_data(struct mcr_NoOp *dataPt);
/* Default init, deinit, copy, and compare */

/*! Signal interface of \ref mcr_NoOp */
MCR_API struct mcr_ISignal *mcr_iNoOp(struct mcr_context *ctx);
/*! Signal data casted \ref mcr_NoOp * */
#define mcr_NoOp_data(sigPt) \
mcr_castpt(struct mcr_NoOp, mcr_Instance_data(sigPt))
/*! Signal data casted \ref mcr_NoOp * */
#define MCR_NOOP_DATA(sig) \
mcr_castpt(struct mcr_NoOp, (sig).instance.data.data)

#ifdef __cplusplus
}
#endif

#endif
