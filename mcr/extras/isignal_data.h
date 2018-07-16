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

/*! \file
 *  \brief \ref ISignalData - Data type for signal instances
 *  C++.
 */

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_ISIGNAL_DATA_H_
#define MCR_EXTRAS_ISIGNAL_DATA_H_

#include "mcr/extras/def.h"

namespace mcr
{
/*! Signal instance data, such as \ref mcr_Key.
 *
 *  Functions to overload: \ref ISignalData.compare, \ref ISignalData.copy,
 *  \ref ISignalData.name, and \ref ISignalData.send \n
 *  Optional overload: \ref ISignalData.addNamesCount and
 *  \ref ISignalData.addNames
 */
class MCR_EXTRAS_API ISignalData
{
public:
	virtual ~ISignalData() {}
	inline bool operator ==(const ISignalData &rhs)
	{
		return !compare(rhs);
	}
	inline bool operator <(const ISignalData &rhs)
	{
		return compare(rhs) < 0;
	}
	inline bool operator >(const ISignalData &rhs)
	{
		return compare(rhs) > 0;
	}
	inline void operator ()()
	{
		send();
	}

	/* Instance */
	/*! \ref mcr_Signal_compare */
	virtual int compare(const ISignalData &rhs) const = 0;
	/*! \ref mcr_Signal_copy
	 *  \param copytron \ref opt
	 */
	virtual void copy(const ISignalData *copytron) = 0;
	/* Signal */
	/*! \ref mcr_ISignal_set_name */
	virtual const char *name() const = 0;
	/*! \ref mcr_ISignal_add_names */
	virtual size_t addNamesCount() const
	{
		return 0;
	}
	/*! \ref mcr_ISignal_add_names */
	virtual void addNames(const char **bufferOut, size_t bufferLength) const
	{
		UNUSED(bufferOut);
		UNUSED(bufferLength);
	}
	/*! \ref mcr_send We highly suggest you inline this function,
	 *  as it will be called as the fully quallified class from \ref SignalType.send.
	 *
	 *  Inline is only suggested if the send function is
	 *  not complex.
	 */
	virtual void send() = 0;
};
}

#endif
