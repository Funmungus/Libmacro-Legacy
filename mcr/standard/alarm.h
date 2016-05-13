/* Libmacro - A multi-platform, extendable macro and hotkey C library.
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
 * \brief \ref mcr_Alarm
 */

#ifndef MCR_ALARM_H
#define MCR_ALARM_H

#include "mcr/standard/def.h"

/*!
 * \brief Pause execution until a day, hour and minute mark.
 *
 * tm members are tm_sec 0-60, tm_min 0-59,
 * tm_hour 0-23, tm_mday 1-31, tm_mon 0-11, tm_year, tm_wday 0-6,
 * tm_yday 0-365, tm_isdst (daylight savings)
 */
typedef struct tm mcr_Alarm;
MCR_API void mcr_Alarm_init(void *dataPt);
/*! \brief Set second, minute, hour, day of month, month, and day of week. */
MCR_API void mcr_Alarm_set_all(mcr_Alarm * almPt, int sec, int minute, int hour,
	int mday, int mon, int wday);
/*!
 * \brief Pause execution until a time point.
 *
 * \param signalData Has data member \ref mcr_Alarm or struct tm
 * \return \ref reterr
 */
MCR_API int mcr_Alarm_send(struct mcr_Signal *signalData);
/*! \brief \ref mcr_Alarm_send */
MCR_API int mcr_Alarm_send_data(mcr_Alarm * dataPt);
/*! \brief \ref mcr_compare_fnc to compare \ref mcr_Alarm or struct tm */
MCR_API int mcr_tm_compare(const void *lhs, const void *rhs);
#define mcr_Alarm_compare mcr_tm_compare
/* Default copy and free */

#define MCR_ALARM_SET_ALL(alm, sec, minute, hour, mday, mon, wday) \
(alm).tm_sec = sec; \
(alm).tm_min = minute; \
(alm).tm_hour = hour; \
(alm).tm_mday = mday; \
(alm).tm_mon = mon; \
(alm).tm_wday = wday;

MCR_API struct mcr_ISignal *mcr_iAlarm(struct mcr_context *ctx);
/*! \brief Signal data casted \ref mcr_Alarm * */
#define mcr_Alarm_data(sigPt) \
((mcr_Alarm *)mcr_inst_data (sigPt))
/*! \brief Signal data casted \ref mcr_Alarm * */
#define MCR_ALARM_DATA(sig) \
((mcr_Alarm *)(sig).inst.data.data)

#endif
