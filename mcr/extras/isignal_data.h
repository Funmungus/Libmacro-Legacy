/* Libmacro - A multi-platform, extendable macro and hotkey C library
  Copyright (C) 2013  Jonathan D. Pelletier

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

/*!
 * \file
 * \brief \ref ISignalData - Data type for signal instances
 * C++.
 */

#ifndef __cplusplus
#pragma message "C++ support is required for extras module"
#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_ISIGNAL_DATA_H
#define MCR_EXTRAS_ISIGNAL_DATA_H

#include "mcr/extras/def.h"

namespace mcr
{
/*!
 * \brief Signal instance data, such as \ref mcr_Key.
 *
 * Functions to overload: \ref ISignalData.compare, \ref ISignalData.copy,
 * \ref ISignalData.name, and \ref ISignalData.send
 */
class MCR_API ISignalData
{
public:
	virtual ~ISignalData() {}
	inline bool operator ==(const ISignalData &rhs)
	{
		return !compare(rhs);
	}
	inline bool operator !=(const ISignalData &rhs)
	{
		return !operator ==(rhs);
	}
	inline bool operator <(const ISignalData &rhs)
	{
		return compare(rhs) < 0;
	}
	inline bool operator >(const ISignalData &rhs)
	{
		return compare(rhs) > 0;
	}
	inline bool operator <=(const ISignalData &rhs)
	{
		return !operator >(rhs);
	}
	inline bool operator >=(const ISignalData &rhs)
	{
		return !operator <(rhs);
	}
	inline void operator ()() throw(int)
	{
		send();
	}

	/* Instance */
	/*! \brief \ref mcr_Signal_compare */
	virtual int compare(const ISignalData &rhs) const throw(int) = 0;
	/*! \brief \ref mcr_Signal_copy
	 * \param copytron \ref opt
	 */
	virtual void copy(const ISignalData *copytron) throw(int) = 0;
	/* Signal */
	/*! \brief \ref mcr_ISignal_set_name */
	virtual const char *name() = 0;
	/*! \brief \ref mcr_ISignal_add_names */
	virtual vector<string> addNames()
	{
		return vector<string>();
	}
	/*!
	 * \brief \ref mcr_send We highly suggest you inline this function,
	 * as it will be called as the fully quallified class from \ref SignalType.send.
	 *
	 * Inline is only suggested if the send function is
	 * not complex.
	 */
	virtual void send() throw(int) = 0;
};
}

#endif
