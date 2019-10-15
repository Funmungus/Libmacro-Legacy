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
 *  \brief Definitions for macros
 */

#ifndef MCR_MACRO_DEF_H_
#define MCR_MACRO_DEF_H_

#include "mcr/signal/signal.h"

#ifndef MCR_THREAD_MAX
	/*! Maximum thread count for macros
	*
	*  For better security macros will never create more than this number of
	*  threads
	*/
	#define MCR_THREAD_MAX 0x10
#endif

#ifndef MCR_MACRO_JOIN_TIMEOUT
	/*! Seconds to wait for macros to respond when they are being
	*  interrupted or paused
	*/
	#define MCR_MACRO_JOIN_TIMEOUT 2
#endif

#ifndef MCR_MAX_PAUSE_COUNT
	/*! The number of times a macro will wait while paused until it resumes
	*/
	#define MCR_MAX_PAUSE_COUNT 5
#endif

#endif
