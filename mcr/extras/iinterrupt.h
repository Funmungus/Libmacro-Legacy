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
 *  \brief
 */

#ifndef MCR_EXTRAS_IINTERRUPT_H_
#define MCR_EXTRAS_IINTERRUPT_H_

#include "mcr/extras/def.h"

namespace mcr
{
class MCR_API IInterrupt
{
public:
	IInterrupt() = default;
	IInterrupt(const IInterrupt &) = default;
	virtual ~IInterrupt() = default;
	IInterrupt &operator =(const IInterrupt &) = default;

	virtual void interrupt(const char *target, int type) = 0;
};
}

#endif
