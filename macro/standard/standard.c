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

#include "mcr/standard/standard.h"

bool mcr_resembles_justified(const mcr_Dimensions first,
							 const mcr_Dimensions second)
{
	dassert(first);
	dassert(second);
	int i;
	for (i = MCR_DIMENSION_CNT; i--;) {
		if (first[i] == 0 || second[i] == 0)
			continue;
		if ((first[i] > 0) != (second[i] > 0))
			return false;
	}
	return true;
}

bool mcr_resembles_absolute(const mcr_Dimensions first,
							const mcr_Dimensions second, const unsigned int measurementError)
{
	dassert(first);
	dassert(second);
	long long err;
	int i;
	for (i = MCR_DIMENSION_CNT; i--;) {
		err = first[i] - second[i];
		if (err < 0)
			err *= -1;
		if (err > measurementError)
			return false;
	}
	return true;
}
