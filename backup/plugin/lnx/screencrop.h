/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file signal/lnx/screencrop.h
 * \brief Linux implementation for screen cropping.
 */

# ifndef MCR_LNX_SCREEN_CROP_H
# define MCR_LNX_SCREEN_CROP_H

# include "signal/native/screencrop.h"

namespace macro
{
	struct NativeScreenCrop
	{
		NativeScreenCrop ( const SpacePosition &, const Dimensions &,
						const std::string & )
		{

		}
	} ;
	inline SpacePosition getPosition ( const NativeScreenCrop & )
	{
		return SpacePosition ( ) ;
	}
	inline void setPosition ( NativeScreenCrop &, const SpacePosition & )
	{
		return ;
	}
	inline Dimensions getDimensions ( const NativeScreenCrop & )
	{
		return Dimensions ( ) ;
	}
	inline void setDimensions ( NativeScreenCrop &, const Dimensions & )
	{
		return ;
	}
	inline std::string getFileName ( const NativeScreenCrop & )
	{
		return "" ;
	}
	inline void setFileName ( NativeScreenCrop &, const std::string & )
	{
		return ;
	}
	inline FILE * getFileDescriptor ( const NativeScreenCrop & )
	{
		return NULL ;
	}
	inline void setFileDescriptor ( NativeScreenCrop &, FILE * const )
	{
		return ;
	}
	inline bool send ( NativeScreenCrop & )
	{
		return false ;
	}
	inline bool send ( NativeScreenCrop &, std::unique_ptr < char [ ] > & )
	{
		return false ;
	}
}

# endif
