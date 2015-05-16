/*
 * Copyright ( C ) Jonathan Pelletier 2013
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license, visit
 * http://creativecommons.org/licenses/by/4.0/.
 * */

/*! \file signal/native/hidecho.h
 * Native mouse click, screen tap, etc. spatial position activation.
 * */

# ifndef MCR_NATIVE_HIDECHO_H
# define MCR_NATIVE_HIDECHO_H

# include "signal/def.h"

namespace macro
{
	// ! \brief ctor, ( int )
	struct NativeHIDEcho ;
	// ! \brief Set the Native event code, platform-dependent.
	void setEvent ( NativeHIDEcho &, const int event ) ;
	// ! \brief Set the Native event code, platform-dependent.
	int getEvent ( const NativeHIDEcho & ) ;
	// ! \brief Send the Native event code, platform-dependent.
	bool send ( NativeHIDEcho & ) ;
}

# ifdef LNX
# include "signal/lnx/hidecho.h"
# elif WIN
# include "signal/win/hidecho.h"
# else
# pragma WARNING(Native undefined: HIDEcho will not be usable.)
# include "signal/nativeless/hidecho.h"
# endif

# endif // HIDECHO_H
