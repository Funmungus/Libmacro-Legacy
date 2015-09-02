/* Macrolibrary - A multi-platform, extendable macro and hotkey C library.
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

/*! \file signal/def.h
 * \brief Definitions for signals
 * */

# ifndef MCR_SIGNAL_DEFINES_H
# define MCR_SIGNAL_DEFINES_H

# include "util/util.h"

// Usable spatial dimensions for cursor/spatial events
# define MCR_DIMENSION_MIN 0
# define MCR_X 0
# define MCR_Y1 0
# define MCR_Y 1
# define MCR_Y2 1
# define MCR_Z 2
# define MCR_Y3 2
# define MCR_W 3
# define MCR_Y4 3
# ifndef MCR_DIMENSION_MAX
# define MCR_DIMENSION_MAX MCR_Z
# endif
# define MCR_DIMENSION_CNT (MCR_DIMENSION_MAX + 1)

/*!
 * \brief A signal is set, released, or both
 */
typedef enum mcr_KeyUpType
{
	MCR_DOWN = 0,
	MCR_UP,
	MCR_BOTH,
	MCR_TOGGLE
} mcr_KeyUpType ;

/*! \brief Spatial position, indices are coordinates.
 * \ref MCR_DIMENSION_CNT
 * This is logically a spatial vector for MCR_DIMENSION_CNT coordinates.
 * */
typedef long long mcr_SpacePosition [ MCR_DIMENSION_CNT ] ;
//! \brief \ref mcr_SpacePosition
typedef mcr_SpacePosition mcr_Dimensions ;

# endif
