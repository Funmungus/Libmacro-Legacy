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
 * <br><br>
 * \ref \file signal/nativeless/screencrop.h <br>
 * \ref \file signal/lnx/screencrop.h <br>
 * \ref \file signal/win/screencrop.h
 */

# ifndef MCR_NATIVE_SCREEN_CROP_H
# define MCR_NATIVE_SCREEN_CROP_H

# include "signal/spaceposition.h"

namespace macro
{
	// ! \brief Native struct to crop screen shots.
	struct NativeScreenCrop ;
	// ! \brief Get position from native structure ( Please try
	// ! \brief to make this value match cursor position ).
	inline MCR_API SpacePosition getPosition ( const NativeScreenCrop * obj ) ;
	// ! \brief Set native position from cursor position.
	inline MCR_API void setPosition ( NativeScreenCrop * obj, const SpacePosition & pos ) ;
	// ! \brief Get spatial dimensions of cropping area.
	inline MCR_API Dimensions getDimensions ( const NativeScreenCrop * obj ) ;
	// ! \brief Set spatial dimensions of cropping area.
	inline MCR_API void setDimensions ( NativeScreenCrop * obj, const Dimensions & dm ) ;
	// ! \brief Get file name to save to.
	inline MCR_API std::string getFileName ( const NativeScreenCrop * obj ) ;
	// ! \brief Set file name to save to.
	inline MCR_API void setFileName ( NativeScreenCrop * obj, const std::string & filename ) ;
	// ! \brief Get file descriptor to save to.
	inline MCR_API FILE * getFileDescriptor ( const NativeScreenCrop * obj ) ;
	// ! \brief Set file descriptor to save to.
	inline MCR_API void setFileDescriptor ( NativeScreenCrop * obj, FILE * const fd ) ;
	// ! \brief Crop screen with obj parameters and save to file.
	inline MCR_API bool send ( NativeScreenCrop * obj ) ;
	// ! \brief Crop screen with obj parameters and copy to buffer.
	inline MCR_API bool send ( NativeScreenCrop * obj, std::unique_ptr < char [ ] > * buffer ) ;
}

# ifdef LNX
# include "signal/lnx/screencrop.h"
# elif WIN
# include "signal/win/screencrop.h"
# else
# pragma WARNING(Native undefined: ScreenCrop will not be usable.)
# include "signal/nativeless/screencrop.h"
# endif

# endif
