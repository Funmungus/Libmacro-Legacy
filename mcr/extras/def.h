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

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_DEF_H_
#define MCR_EXTRAS_DEF_H_

#include "mcr/standard/standard.h"

#include <cstring>
#include <string>
#include <map>
#include <vector>

namespace mcr
{
struct Libmacro;
struct Signal;
struct Trigger;
typedef std::vector<Signal> SignalSet;
typedef std::vector<std::vector<Signal>> SignalSetSet;
typedef std::vector<Trigger> TriggerSet;
typedef std::vector<std::vector<Trigger>> TriggerSetSet;

#ifdef MCR_DEBUG
inline void dthrow(bool condition, int throwable)
{
	if (condition)
		throw throwable;
#else
inline void dthrow(bool, int)
{
#endif
}
}

#endif
