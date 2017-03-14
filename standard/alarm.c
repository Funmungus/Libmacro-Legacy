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
#include "mcr/modules.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>

void mcr_Alarm_set_all(struct mcr_Alarm *almPt, int sec, int minute, int hour,
	int mday, int mon, int wday)
{
	dassert(almPt);
	MCR_ALARM_SET_ALL(*almPt, sec, minute, hour, mday, mon, wday);
}

/* Send functions that are placed into ISignals. */
int mcr_Alarm_send(struct mcr_Signal *sigPt)
{
	dassert(sigPt);
	return mcr_Alarm_send_data(mcr_Alarm_data(sigPt));
}

int mcr_tm_compare(const void *lhs, const void *rhs)
{
	time_t lT, rT;
	if (rhs) {
		if (lhs) {
			lT = mktime((struct tm *)lhs);
			rT = mktime((struct tm *)rhs);
			return MCR_CMP_INTEGRAL(lT, rT);
		}
		return -1;
	}
	return ! !lhs;
}

struct mcr_ISignal *mcr_iAlarm(struct mcr_context *ctx)
{
	dassert(ctx);
	return &ctx->standard.ialarm;
}
