/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file signal/def.h
 * \brief Definitions for signals.
 * TODO Remove xml from signals and put xml in its own library.
 */

# ifndef MCR_SIGNAL_DEFINES_H
# define MCR_SIGNAL_DEFINES_H

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
# define MCR_DIMENSION_CNT MCR_DIMENSION_MAX + 1

# include <chrono>
# include <cstdlib>
# include <cstring>
# include <memory>
# include <mutex>
# include <thread>
# include <vector>

# include "util/util.h"

namespace macro
{
	/*! \brief To signify a key is being set to either pressed down,
	 * released, or both pressed and released.
	 */
	enum KeyUpType
	{
		DOWN = 0,
		UP = 1,
		BOTH = 2
	} ;
	/*! \brief Spatial position, indices are coordinates.
	 * \ref \def MCR_DIMENSION_CNT
	 * This is logically a spatial vector for MCR_DIMENSION_CNT coordinates.
	 **/
	typedef long long SpacePosition [ MCR_DIMENSION_CNT ] ;
	typedef SpacePosition Dimensions ;
}

# endif
