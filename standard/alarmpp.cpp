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

extern "C" {
#include "mcr/standard/standard.h"
}
#include <thread>

int mcr_Alarm_send_data(struct mcr_Alarm * dataPt)
{
	if (!dataPt)
		return 0;
	std::chrono::system_clock::time_point until_time =
		 std::chrono::system_clock::from_time_t(std::mktime(&dataPt->time));
	std::this_thread::sleep_until(until_time);
	return 0;
}
