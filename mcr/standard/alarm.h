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
 * \brief \ref mcr_Alarm - Pause execution until a specific date-time.
 */

#ifndef MCR_ALARM_H
#define MCR_ALARM_H

#include "mcr/standard/def.h"

/*! \brief Pause execution until a specific date-time. */
struct mcr_Alarm {
	/*! \brief Pause execution until this time. */
	struct tm time;
};

/*! \brief Set second, minute, hour, day of month, month, and day of week. */
MCR_API void mcr_Alarm_set_all(struct mcr_Alarm *almPt, int sec, int minute,
			       int hour, int mday, int mon, int wday);
/*! \pre Signal data is \ref mcr_Alarm
 * \brief Pause execution until a time point.
 *
 * \return \ref reterr
 */
MCR_API int mcr_Alarm_send(struct mcr_Signal *sigPt);
/*! \brief \ref std::this_thread::sleep_until defined in C++ */
MCR_API int mcr_Alarm_send_data(struct mcr_Alarm *dataPt);
/*! \brief Compare two \ref mcr_Alarm or struct tm
 *
 * \param lhs \ref opt \ref mcr_Alarm * or struct tm *
 * \param rhs \ref opt \ref mcr_Alarm * or struct tm *
 * \return \ref retcmp
 */
MCR_API int mcr_tm_compare(const void *lhs, const void *rhs);
/*! \brief Compare two \ref mcr_Alarm or struct tm
 *
 * \param lhs \ref opt \ref mcr_Alarm * or struct tm *
 * \param rhs \ref opt \ref mcr_Alarm * or struct tm *
 * \return \ref retcmp
 */
#define mcr_Alarm_compare mcr_tm_compare
/* Default init, deinit, and copy */

#define MCR_ALARM_SET_ALL(alm, sec, minute, hour, mday, mon, wday) \
(alm).time.tm_sec = sec; \
(alm).time.tm_min = minute; \
(alm).time.tm_hour = hour; \
(alm).time.tm_mday = mday; \
(alm).time.tm_mon = mon; \
(alm).time.tm_wday = wday;

/*! \brief Get the signal interface of \ref mcr_Alarm */
MCR_API struct mcr_ISignal *mcr_iAlarm(struct mcr_context *ctx);
/*! \brief Signal data casted \ref mcr_Alarm * */
#define mcr_Alarm_data(sigPt) \
((struct mcr_Alarm *)mcr_Instance_data (sigPt))
/*! \brief Signal data casted \ref mcr_Alarm * */
#define MCR_ALARM_DATA(sig) \
((struct mcr_Alarm *)(sig).instance.data.data)

#endif
