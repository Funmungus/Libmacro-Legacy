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
 *  \brief \ref mcr_Scroll - Scroll visible area.
 */

#ifndef MCR_STANDARD_SCROLL_H_
#define MCR_STANDARD_SCROLL_H_

#include "mcr/standard/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Scroll visible area. */
struct mcr_Scroll {
	/*! Magnitude to scroll in each direciton */
	mcr_Dimensions dm;
};

/*! \pre Signal has data member \ref mcr_Scroll
 *  \brief Scroll through visible area.
 *
 *  \return \ref reterr
 */
MCR_API int mcr_Scroll_send(struct mcr_Signal *sigPt);
/*! \ref mcr_Scroll_send */
MCR_API int mcr_Scroll_send_data(struct mcr_Scroll *dataPt);
/* Default init, deinit, compare, copy */

/*! Signal interface of \ref mcr_Scroll */
MCR_API struct mcr_ISignal *mcr_iScroll(struct mcr_context *ctx);
/*! Signal data casted \ref mcr_Scroll * */
#define mcr_Scroll_data(sigPt) \
mcr_castpt(struct mcr_Scroll, mcr_Instance_data(sigPt))
/*! Signal data casted \ref mcr_Scroll * */
#define MCR_SCROLL_DATA(sig) \
mcr_castpt(struct mcr_Scroll, (sig).instance.data.data)

#ifdef __cplusplus
}
#endif

#endif
