/* include/signal/def.h
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file signal/def.h
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
 * \brief To signify a signal is being set released, or both.
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
