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
 * \brief \ref mcr_MoveCursor
 */

#ifndef MCR_MOVE_CURSOR_H
#define MCR_MOVE_CURSOR_H

#include "mcr/standard/def.h"

/*! \brief \ref Change spatial coordinates of user's cursor. */
struct mcr_MoveCursor;
/*! \brief \ref mcr_MoveCursor shorthand */
typedef struct mcr_MoveCursor mcr_MC;

/*!
 * \brief Initialize \ref mcr_MoveCursor
 *
 * Native
 */
MCR_API int mcr_MoveCursor_init(void *mcPt);
/*! \brief Set both position and justification. */
MCR_API void mcr_MoveCursor_set_all(struct mcr_MoveCursor *mcPt,
	const mcr_SpacePosition pos, bool flagJustify);
/*!
 * \brief Get cursor spatial positions, copied into the buffer
 *
 * \param buffer Buffer to copy into
 */
MCR_API void mcr_MoveCursor_position(const struct mcr_MoveCursor *mcPt,
	mcr_SpacePosition buffer);
/*!
 * \brief Set cursor spatial positions
 *
 * \param pos Buffer to set positions from
 */
MCR_API void mcr_MoveCursor_set_position(struct mcr_MoveCursor *mcPt,
	const mcr_SpacePosition pos);
/*!
 * \brief Get the value of a single coordinate position.
 *
 * \param coordinate x, y, z coordinate type, \ref MCR_DIMENSION_MAX
 */
MCR_API long long mcr_MoveCursor_coordinate(const struct mcr_MoveCursor *mcPt,
	int coordinate);
/*!
 * \brief Set the position of one coordinate.
 *
 * \param coordinate x, y, z, coordinate type, \ref MCR_DIMENSION_MAX
 * \param value Coordinate value to set
 */
MCR_API void mcr_MoveCursor_set_coordinate(struct mcr_MoveCursor *mcPt,
	int coordinate, long long value);
/*!
 * \brief Get cursor justification
 *
 * \return 0 is absolute, otherwise cursor is justified
 */
MCR_API bool mcr_MoveCursor_is_justify(const struct mcr_MoveCursor *mcPt);
/*!
 * \brief Set cursor justification
 *
 * \param cursorJustify 0 absolute, otherwise cursor is justified
 */
MCR_API void mcr_MoveCursor_set_justify(struct mcr_MoveCursor *mcPt,
	bool flagJustify);
/*!
 * \brief Move HID cursor position.
 *
 * \param signalData Has data member \ref mcr_MoveCursor
 * \return \ref reterr
 */
MCR_API int mcr_MoveCursor_send(struct mcr_Signal *signalData);
/*!
 * \brief \ref mcr_MoveCursor_send
 *
 * Native
 */
MCR_API int mcr_MoveCursor_send_data(struct mcr_MoveCursor *mcPt);
/*! \brief Compare \ref mcr_MoveCursor */
MCR_API int mcr_MoveCursor_compare(const void *lhs, const void *rhs);
/*! \brief Copy \ref mcr_MoveCursor */
MCR_API int mcr_MoveCursor_copy(void *dstPt, void *srcPt);
/* Default deinit */

/*!
 * \brief Current cursor position
 *
 * Native
 */
MCR_API void mcr_cursor_position(mcr_SpacePosition buffer);
/*!
 * \brief If justified then \ref mcr_resembles_justified,
 * else \ref mcr_resembles_absolute
 *
 * \param measurementError Only used in
 * \ref mcr_resembles_absolute.
 */
MCR_API bool mcr_resembles(const struct mcr_MoveCursor *lhs,
	const struct mcr_MoveCursor *rhs, const unsigned int measurementError);

/* mcr_MoveCursor -> mcr_MC */
/*! \brief \ref mcr_MoveCursor_set_all */
#define mcr_MC_set_all mcr_MoveCursor_set_all
/*! \brief \ref mcr_MoveCursor_position */
#define mcr_MC_position mcr_MoveCursor_position
/*! \brief \ref mcr_MoveCursor_set_position */
#define mcr_MC_set_position mcr_MoveCursor_set_position
/*! \brief \ref mcr_MoveCursor_coordinate */
#define mcr_MC_coordinate mcr_MoveCursor_coordinate
/*! \brief \ref mcr_MoveCursor_set_coordinate */
#define mcr_MC_set_coordinate mcr_MoveCursor_set_coordinate
/*! \brief \ref mcr_MoveCursor_justify */
#define mcr_MC_is_justify mcr_MoveCursor_is_justify
/*! \brief \ref mcr_MoveCursor_set_justify */
#define mcr_MC_set_justify mcr_MoveCursor_set_justify
/*! \brief \ref mcr_MoveCursor_send */
#define mcr_MC_send mcr_MoveCursor_send
/*! \brief \ref mcr_MoveCursor_send_data */
#define mcr_MC_send_data mcr_MoveCursor_send_data
/*! \brief \ref mcr_MoveCursor_compare */
#define mcr_MC_compare mcr_MoveCursor_compare
/*! \brief \ref mcr_MoveCursor_copy */
#define mcr_MC_copy mcr_MoveCursor_copy
/*! \brief \ref mcr_MoveCursor_init */
#define mcr_MC_init mcr_MoveCursor_init

MCR_API struct mcr_ISignal *mcr_iMoveCursor(struct mcr_context *ctx);
#define mcr_iMC mcr_iMoveCursor
/*! \brief Signal data casted \ref mcr_MoveCursor * */
#define mcr_MoveCursor_data(sigPt) \
((struct mcr_MoveCursor *)mcr_Instance_data(sigPt))
/*! \brief Signal data casted \ref mcr_MoveCursor * */
#define MCR_MOVECURSOR_DATA(sig) \
((struct mcr_MoveCursor *)(sig).instance.data.data)

/*! \brief Signal data casted \ref mcr_MoveCursor * */
#define mcr_MC_data(sigPt) mcr_MoveCursor_data (sigPt)
/*! \brief Signal data casted \ref mcr_MoveCursor * */
#define MCR_MC_DATA(sig) MCR_MOVECURSOR_DATA (sig)

#endif
