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

#include "mcr/extras/extras.h"

namespace mcr
{
IKeyProvider *StringKey::_keyProvider = nullptr;

int StringKey::compare(const StringKey &rhs) const
{
	int cmp;
	if (&rhs == this)
		return 0;
	if ((cmp = std::memcmp(&interval, &rhs.interval, sizeof(interval))))
		return cmp;
	return string.compare(rhs.string);
}

void StringKey::copy(const IData *copytron)
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
	std::string str = text();
	size_t totalCount = context->characterCount(), characterCount, j;
	for (auto i: str) {
		if (static_cast<size_t>(i) < totalCount) {
			auto chara = context->characterData(i);
			characterCount = context->characterCount(i);
			for (j = 0; j < characterCount; j++) {
				if (mcr_send(context->ptr(), chara[j].ptr()))
					throw mcr_err;
			}
			mcr_NoOp_send_data(&interval);
		}
	}
}
}
