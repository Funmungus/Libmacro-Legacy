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

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_QT_TYPES_H_
#define MCR_EXTRAS_QT_TYPES_H_

#include "mcr/extras/def.h"

#include <QtCore>

#include "mcr/util/def.h"

namespace mcr
{
typedef int size_type;
typedef QByteArray string;
template <typename T>
using vector = QVector<T>;
template <typename T>
using map = QMap<T>;

inline char *bytes(string &str)
{
	return str.data();
}
inline const char *bytes(const string &str)
{
	return str.constData();
}
}

#endif
