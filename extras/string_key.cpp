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

#include "mcr/extras/extras.h"

#include <cstring>

namespace mcr
{
IKeyProvider *StringKey::_keyProvider = NULL;

StringKey &StringKey::operator =(const StringKey &copytron)
{
	if (&copytron != this) {
		clear();
		interval = copytron.interval;
		string = copytron.string;
	}
	return *this;
}

int StringKey::compare(const StringKey &rhs) const
{
	int cmp;
	if (&rhs == this)
		return 0;
	if ((cmp = std::memcmp(&interval, &rhs.interval, sizeof(interval))))
		return cmp;
	return string.compare(rhs.string);
}

void StringKey::copy(const mcr::ISignalData *copytron)
{
	const StringKey *cPt = dynamic_cast<const StringKey *>(copytron);
	if (!cPt)
		throw EFAULT;
	if (cPt != this) {
		interval = cPt->interval;
		string = cPt->string;
	}
}

void StringKey::send()
{
	Libmacro *context = Libmacro::instance();
	size_type i, j, err;
	mcr::string mem = stringText();
	Signal *c;
	size_t count;
	for (i = 0; i < mem.size(); i++) {
		if (mem[i] < (signed)context->characterCount()) {
			count = context->characterCount(mem[i]);
			c = context->character(mem[i]);
			for (j = 0; j < count; j++) {
				if ((err = mcr_send(context->ptr(), c[j].ptr())))
					throw err;
			}
			mcr_NoOp_send_data(&interval);
		}
	}
}

StringKeyRef::StringKeyRef(Libmacro *context, mcr_Signal *sigPt)
	: SignalManager(context, sigPt)
{
	init(context->iStringKey().ptr());
}
}
