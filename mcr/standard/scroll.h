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
 * \brief \ref mcr_Scroll
 */

#ifndef MCR_SCROLL_H
#define MCR_SCROLL_H

#include "mcr/standard/def.h"

/*! \brief Scroll visible area. */
struct mcr_Scroll;

/*! \brief Initialize \ref mcr_Scroll */
MCR_API int mcr_Scroll_init(void *scrollPt);
/*!
 * \brief All coordinate dimensions are copied into the buffer.
 *
 * \param buffer Values are copied here
 */
MCR_API void mcr_Scroll_dimensions(const struct mcr_Scroll *scrollPt,
	mcr_Dimensions buffer);
/*!
 * \brief Set all coordinate dimensions.
 *
 * \param pos Values to set from
 */
MCR_API void mcr_Scroll_set_dimensions(struct mcr_Scroll *scrollPt,
	const mcr_Dimensions pos);
/*!
 * \brief Get the value of a single coordinate dimension.
 *
 * \param coordinate x, y, z coordinate type, \ref MCR_DIMENSION_MAX
 */
MCR_API long long mcr_Scroll_coordinate(const struct mcr_Scroll *scrollPt,
	int coordinate);
/*!
 * \brief Set a single coordinate dimension.
 *
 * \param coordinate x, y, z coordinate type, \ref MCR_DIMENSION_MAX
 * \param value Coordinate value to set
 */
MCR_API void mcr_Scroll_set_coordinate(struct mcr_Scroll *scrollPt,
	int coordinate, long long value);
/*!
 * \brief Scroll through visible area.
 *
 * \param signalData Has data member \ref mcr_Scroll
 * \return \ref reterr
 */
MCR_API int mcr_Scroll_send(struct mcr_Signal *signalData);
/*! \brief \ref mcr_Scroll_send */
MCR_API int mcr_Scroll_send_data(struct mcr_Scroll *dataPt);
/*! \brief Compare \ref mcr_Scroll */
MCR_API int mcr_Scroll_compare(const void *lhs, const void *rhs);
/*! \brief Copy \ref mcr_Scroll */
MCR_API int mcr_Scroll_copy(void *dstPt, void *srcPt);
/* Default deinit */

MCR_API struct mcr_ISignal *mcr_iScroll(struct mcr_context *ctx);
/*! \brief Signal data casted \ref mcr_Scroll * */
#define mcr_Scroll_data(sigPt) \
((struct mcr_Scroll *)mcr_Instance_data(sigPt))
/*! \brief Signal data casted \ref mcr_Scroll * */
#define MCR_SCROLL_DATA(sig) \
((struct mcr_Scroll *)(sig).instance.data.data)

#endif
