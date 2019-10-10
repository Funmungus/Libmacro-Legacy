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
 *  \brief \ref IData - Data type for instances
 *  C++.
 */

#ifndef MCR_EXTRAS_IDATA_H_
#define MCR_EXTRAS_IDATA_H_

#include "mcr/extras/def.h"

namespace mcr
{
/*! Instance data, such as \ref mcr_Data member of \ref mcr_Instance.
 *
 *  Functions to overload: \ref IData.compare, \ref IData.copy,
 *  \ref IData.name \n
 *  Optional overload: \ref IData.addNameCount and
 *  \ref IData.addNames
 */
class MCR_API IData
{
public:
	IData() = default;
	IData(const IData &) = default;
	virtual ~IData() {}
	IData &operator =(const IData &) = default;

	inline bool operator ==(const IData &rhs)
	{
		return !compare(rhs);
	}
	inline bool operator <(const IData &rhs)
	{
		return compare(rhs) < 0;
	}
	inline bool operator >(const IData &rhs)
	{
		return compare(rhs) > 0;
	}

	/* Instance */
	/*! \ref mcr_Instance_compare */
	virtual int compare(const IData &rhs) const = 0;
	/*! \ref mcr_Instance_copy
	 *  \param copytron \ref opt
	 */
	virtual void copy(const IData *copytron) = 0;
	/* Instance */
	/*! \ref mcr_Interface_set_name */
	virtual const char *name() const = 0;
	/*! \ref mcr_Interface_add_names */
	virtual size_t addNameCount() const
	{
		return 0;
	}
	/*! \ref mcr_Interface_add_names */
	virtual void addNames(const char **, size_t) const
	{
	}
};
}

#endif
