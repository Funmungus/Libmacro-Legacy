
// Not yet available in linux
# include "signal/screencrop.h"

namespace macro
{
	const std::string ScreenCrop::name = "screencrop" ;
	bool ( * ScreenCrop::dispatcher ) ( ScreenCrop & ) ;

	std::string ScreenCrop::type ( ) const
	{
		return ScreenCrop::name ;
	}
}
