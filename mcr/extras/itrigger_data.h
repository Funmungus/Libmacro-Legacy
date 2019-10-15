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
 *  \brief \ref ITriggerData - Data type for trigger instances
 *  C++.
 */

#ifndef MCR_EXTRAS_ISIGNAL_DATA_H_
#define MCR_EXTRAS_ISIGNAL_DATA_H_

#include "mcr/extras/idata.h"

namespace mcr
{
/*! Trigger instance data, such as \ref mcr_Data member of \ref mcr_Instance.
 *
 *  Functions to overload: \ref IData.compare, \ref IData.copy,
 *  \ref IData.name \n
 *  Optional overload: \ref IData.addNameCount and
 *  \ref IData.addNames
 */
class MCR_API ITriggerData: public IData
{
public:
	ITriggerData() = default;
	ITriggerData(const ITriggerData &) = default;
	virtual ~ITriggerData() override {}
	ITriggerData &operator =(const ITriggerData &) = default;

	inline bool operator ==(const ITriggerData &rhs)
	{
		return !compare(rhs);
	}
	inline bool operator <(const ITriggerData &rhs)
	{
		return compare(rhs) < 0;
	}
	inline bool operator >(const ITriggerData &rhs)
	{
		return compare(rhs) > 0;
	}
};
}

#endif
