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
 *  \brief Definitions for standard types.
 */

#ifndef MCR_STANDARD_DEF_H_
#define MCR_STANDARD_DEF_H_

#include "mcr/macro/macro.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Usable spatial dimensions for cursor/spatial events */
#define MCR_DIMENSION_MIN 0
#define MCR_X 0
#define MCR_Y1 MCR_X
#define MCR_Y 1
#define MCR_Y2 MCR_Y
#define MCR_Z 2
#define MCR_Y3 MCR_Z
#define MCR_W 3
#define MCR_Y4 MCR_W
#ifndef MCR_DIMENSION_MAX
#define MCR_DIMENSION_MAX MCR_Z
#endif
#define MCR_DIMENSION_CNT (MCR_DIMENSION_MAX + 1)

#define MCR_DIMENSIONS_ZERO(posArray) \
(posArray)[MCR_X] = 0; \
(posArray)[MCR_Y] = 0; \
(posArray)[MCR_Z] = 0;

/*! A value state is set or unset */
enum mcr_ApplyType {
	/*! Set, value is applied. */
	MCR_SET = 0,
	/*! Unset, value is released or not applied. */
	MCR_UNSET,
	/*! Value is both set and unset, in that order. */
	MCR_BOTH,
	/*! Set or unset is swapped.  Other mcr_ApplyType are ignored. */
	MCR_TOGGLE
};

/*! Spatial position, indices are coordinates.
 *  \ref MCR_DIMENSION_CNT
 *
 *  This is logically a spatial vector for MCR_DIMENSION_CNT coordinates.
 */
typedef long long mcr_SpacePosition[MCR_DIMENSION_CNT];
/*! \ref mcr_SpacePosition */
typedef mcr_SpacePosition mcr_Dimensions;

/*! Any or invalid echo code */
#define MCR_ECHO_ANY mcr_cast(size_t, -1)
/*! Any or invalid key code */
#define MCR_KEY_ANY mcr_cast(int, 0)

#ifdef __cplusplus
}
#endif

#endif
