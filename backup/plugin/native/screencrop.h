/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file signal/native/screencrop.h
 * \brief Important, please implement these functions for given
 * programming environment to crop screen.
 */

# ifndef MCR_NATIVE_SCREEN_CROP_H
# define MCR_NATIVE_SCREEN_CROP_H

# include "signal/def.h"

namespace macro
{
	// ! \brief ctor, ( SpacePosition, Dimensions, string )
	// ! \brief ctor, ( string )
	struct NativeScreenCrop ;
	// ! \brief Get position from native structure ( Please try
	// ! \brief to make this value match cursor position ).
	inline void getPosition ( const NativeScreenCrop & obj, SpacePosition posOut ) ;
	// ! \brief Set native position from cursor position.
	inline void setPosition ( NativeScreenCrop & obj, const SpacePosition pos ) ;
	inline long long getPosition ( const NativeScreenCrop & obj,
								const unsigned int positionType ) ;
	inline void setPosition ( const NativeScreenCrop & obj,
								const unsigned int positionType,
								const long long positionValue ) ;
	// ! \brief Get spatial dimensions of cropping area.
	inline void getDimensions ( const NativeScreenCrop & obj ) ;
	// !, Dimensions dmOut \brief Set spatial dimensions of cropping area.
	inline void setDimensions ( NativeScreenCrop & obj, const Dimensions dm ) ;
	inline long long getDimension ( const NativeScreenCrop & obj,
								const unsigned int positionType ) ;
	inline void setDimension ( const NativeScreenCrop & obj,
								const unsigned int positionType,
								const long long dimensionValue ) ;
	// ! \brief Get file name to save to.
	inline std::string getFileName ( const NativeScreenCrop & obj ) ;
	// ! \brief Set file name to save to.
	inline void setFileName ( NativeScreenCrop & obj, const std::string & filename ) ;
	// ! \brief Get file descriptor to save to.
	inline FILE * getFileDescriptor ( const NativeScreenCrop & obj ) ;
	// ! \brief Set file descriptor to save to.
	inline void setFileDescriptor ( NativeScreenCrop & obj, FILE * const fd ) ;
	// ! \brief Crop screen with obj parameters and save to file.
	inline bool send ( NativeScreenCrop & obj ) ;
	// ! \brief Crop screen with obj parameters and copy to buffer.
	inline bool send ( NativeScreenCrop & obj, std::unique_ptr < char [ ] > & buffer ) ;
}

MCR_INCLUDE_MACRO ( INSERT_NATIVE_DIR ( signal/, /screencrop.h ) )

//# ifdef LNX
//# include "signal/lnx/screencrop.h"
//# elif WIN
//# include "signal/win/screencrop.h"
//# else
//# pragma WARNING ( Native undefined: ScreenCrop will not be usable. )
//# include "signal/nativeless/screencrop.h"
//# endif

# endif
