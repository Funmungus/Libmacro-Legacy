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

#include <cstring>
#include "mcr/extras/mod_extras.h"

namespace mcr
{
void Alarm::copy(const mcr::ISignalData *copytron) MCR_THROWS
{
	if (copytron == this)
		return;
	if (copytron) {
		const Alarm *mem = dynamic_cast<const Alarm *>(copytron);
		if (!mem)
			throw EINVAL;
		time = mem->time;
	} else {
		memset(&time, 0, sizeof(time));
	}
}

AlarmRef::AlarmRef(Libmacro *context, mcr_Signal *sigPt) MCR_THROWS
	: SignalManager(context, sigPt)
{
	init(&this->context()->iAlarm().isignal);
}
}
