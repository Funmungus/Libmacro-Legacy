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
 * \brief \ref mcr_NoOp
 */

#ifndef MCR_NOOP_H
#define MCR_NOOP_H

#include "mcr/standard/def.h"

/*!
 * \brief Pause execution in seconds and nanoseconds.
 *
 * timespec members are : tv_sec, and tv_nsec
 */
typedef struct timespec mcr_NoOp;

/*! \brief Set both seconds and nanoseconds. */
MCR_API void mcr_NoOp_set_all(mcr_NoOp * noopPt, int sec, int nsec);
/*!
 * \brief Pause execution in seconds and nanoseconds.
 *
 * \param signalData Has data member \ref mcr_NoOp
 * or struct timespec
 * \return \ref reterr
 */
MCR_API int mcr_NoOp_send(struct mcr_Signal *signalData);
/*! \brief \ref mcr_NoOp_send */
MCR_API int mcr_NoOp_send_data(mcr_NoOp * dataPt);
/* Default init, deinit, copy, and compare */

#define MCR_NOOP_SET_ALL(noop, sec, nsec) \
(noop).tv_sec = sec; \
(noop).tv_nsec = nsec;

MCR_API struct mcr_ISignal *mcr_iNoOp(struct mcr_context *ctx);
/*! \brief Signal data casted \ref mcr_NoOp * */
#define mcr_NoOp_data(sigPt) \
((mcr_NoOp *)mcr_Instance_data(sigPt))
/*! \brief Signal data casted \ref mcr_NoOp * */
#define MCR_NOOP_DATA(sig) \
((mcr_NoOp *)(sig).instance.data.data)

#endif
