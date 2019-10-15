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
 *  \brief \ref mcr_MoveCursor - Change spatial coordinates of cursor.
 */

#ifndef MCR_STANDARD_MOVE_CURSOR_H_
#define MCR_STANDARD_MOVE_CURSOR_H_

#include "mcr/standard/def.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! Change spatial coordinates of ursor.*/
struct mcr_MoveCursor {
	/*! Spatial coordinates to set or modify with */
	mcr_SpacePosition pos;
	/*! If false coordinates will be set, otherwise coordinates
	 *  adjust the cursor for current position. */
	bool is_justify;
};
/*! \ref mcr_MoveCursor shorthand */
typedef struct mcr_MoveCursor mcr_MC;

/*! Set initial values */
MCR_API void mcr_MoveCursor_set_all(struct mcr_MoveCursor *mcPt,
									const mcr_SpacePosition pos, bool flagJustify);
/*! \pre Signal data is  \ref mcr_MoveCursor
 *  \brief Move HID cursor position.
 *
 *  \return \ref reterr
 */
MCR_API int mcr_MoveCursor_send(struct mcr_Signal *sigPt);
/*! \ref mcr_MoveCursor_send
 *
 *  \ref mcr_is_platform
 *  \return \ref reterr
 */
MCR_API int mcr_MoveCursor_send_data(struct mcr_MoveCursor *mcPt);
/* Default init, deinit, compare, and copy */

/*! Current cursor position
 *
 *  \ref mcr_is_platform
 */
MCR_API void mcr_cursor_position(mcr_SpacePosition buffer);
/*! If justified then \ref mcr_resembles_justified,
 *  else \ref mcr_resembles_absolute
 *
 *  \param lhs \ref opt
 *  \param rhs \ref opt
 *  \param measurementError Only used in absolute
 *  \return If false cursors do not resemble each other, otherwise they do
 */
MCR_API bool mcr_resembles(const struct mcr_MoveCursor *lhs,
						   const struct mcr_MoveCursor *rhs, const unsigned int measurementError);

/* mcr_MoveCursor -> mcr_MC */
/*! \ref mcr_MoveCursor_set_all */
#define mcr_MC_set_all mcr_MoveCursor_set_all
/*! \ref mcr_MoveCursor_send */
#define mcr_MC_send mcr_MoveCursor_send
/*! \ref mcr_MoveCursor_send_data */
#define mcr_MC_send_data mcr_MoveCursor_send_data

/*! Signal interface of \ref mcr_MoveCursor */
MCR_API struct mcr_ISignal *mcr_iMoveCursor(struct mcr_context *ctx);
/*! Signal interface of \ref mcr_MoveCursor */
#define mcr_iMC mcr_iMoveCursor
/*! Signal data casted \ref mcr_MoveCursor * */
#define mcr_MoveCursor_data(sigPt) \
mcr_castpt(struct mcr_MoveCursor, mcr_Instance_data(sigPt))
/*! Signal data casted \ref mcr_MoveCursor * */
#define MCR_MOVECURSOR_DATA(sig) \
mcr_castpt(struct mcr_MoveCursor, (sig).instance.data.data)

/*! Signal data casted \ref mcr_MoveCursor * */
#define mcr_MC_data(sigPt) mcr_MoveCursor_data (sigPt)
/*! Signal data casted \ref mcr_MoveCursor * */
#define MCR_MC_DATA(sig) MCR_MOVECURSOR_DATA (sig)

#ifdef __cplusplus
}
#endif

#endif
