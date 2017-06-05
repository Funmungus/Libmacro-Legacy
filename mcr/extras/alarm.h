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

/*!
 * \file
 * \brief \ref Alarm - Pause execution until a specific date-time.
 */

#ifndef __cplusplus
#pragma message "C++ support is required for extras module"
#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_ALARM_H
#define MCR_EXTRAS_ALARM_H

#include "mcr/extras/isignal.h"

namespace mcr
{
/*! \brief Pause execution until a specific date-time. */
struct MCR_API Alarm : public ISignalData {
	tm time;

	Alarm()
		: time()
	{
	}
	Alarm(const Alarm &) = default;
	Alarm &operator =(const Alarm &) = default;

	/*!
	 * \brief Get an alarm from signal
	 *
	 * \param sigPt \ref opt \ref mcr_Signal *
	 */
	static inline Alarm *data(mcr_Signal *sigPt)
	{
		return (Alarm *)(sigPt ? sigPt->instance.data.data : NULL);
	}

	/*! \brief Seconds
	 *
	 * [0-60] (1 leap second)
	 */
	inline int &sec()
	{
		return time.tm_sec;
	}
	/*! \brief Seconds
	 *
	 * [0-60] (1 leap second)
	 */
	inline int sec() const
	{
		return time.tm_sec;
	}
	/*! \brief Minutes
	 *
	 * [0-59]
	 */
	inline int &min()
	{
		return time.tm_min;
	}
	/*! \brief Minutes
	 *
	 * [0-59]
	 */
	inline int min() const
	{
		return time.tm_min;
	}
	/*! \brief Hours
	 *
	 * [0-23]
	 */
	inline int &hour()
	{
		return time.tm_hour;
	}
	/*! \brief Hours
	 *
	 * [0-23]
	 */
	inline int hour() const
	{
		return time.tm_hour;
	}
	/*! \brief Day
	 *
	 * [1-31]
	 */
	inline int &mday()
	{
		return time.tm_mday;
	}
	/*! \brief Day
	 *
	 * [1-31]
	 */
	inline int mday() const
	{
		return time.tm_mday;
	}
	/*! \brief Month
	 *
	 * [0-11]
	 */
	inline int &mon()
	{
		return time.tm_mon;
	}
	/*! \brief Month
	 *
	 * [0-11]
	 */
	inline int mon() const
	{
		return time.tm_mon;
	}
	/*! \brief Year, 0 = 1900 */
	inline int &year()
	{
		return time.tm_year;
	}
	/*! \brief Year, 0 = 1900 */
	inline int year() const
	{
		return time.tm_year;
	}
	/*! \brief Day of week
	 *
	 * [0-6]
	 */
	inline int &wday()
	{
		return time.tm_wday;
	}
	/*! \brief Day of week
	 *
	 * [0-6]
	 */
	inline int wday() const
	{
		return time.tm_wday;
	}
	/*! \brief Days in year
	 *
	 * [0-365]
	 */
	inline int &yday()
	{
		return time.tm_yday;
	}
	/*! \brief Days in year
	 *
	 * [0-365]
	 */
	inline int yday() const
	{
		return time.tm_yday;
	}
	/*! \brief DST
	 *
	 * [-1,0,1]
	 */
	inline int &isdst()
	{
		return time.tm_isdst;
	}
	/*! \brief DST
	 *
	 * [-1,0,1]
	 */
	inline int isdst() const
	{
		return time.tm_isdst;
	}

	/*! \brief \ref mcr_Signal_compare */
	virtual int compare(const ISignalData &rhs) const throw(int) override
	{
		return compare(dynamic_cast<const Alarm &>(rhs));
	}
	/*! \brief \ref mcr_Signal_compare */
	inline int compare(const Alarm &rhs) const
	{
		tm lMem = time, rMem = rhs.time;
		time_t lT = std::mktime(&lMem), rT = std::mktime(&rMem);
		return lT < rT ? -1 : rT > lT;
	}
	/*! \brief \ref mcr_Signal_copy
	 * \param copytron \ref opt
	 */
	virtual void copy(const ISignalData *copytron) throw(int) override;
	/*! \brief \ref mcr_ISignal_set_name */
	virtual const char *name() override
	{
		return "Alarm";
	}
	/*! \brief \ref mcr_send */
	virtual inline void send() throw(int) override
	{
		std::chrono::system_clock::time_point until_time =
			std::chrono::system_clock::from_time_t(std::mktime(&time));
		std::this_thread::sleep_until(until_time);
	}

	void zero()
	{
		std::memset(&time, 0, sizeof(time));
	}
	void now()
	{
		time_t t = std::chrono::system_clock::to_time_t(
				   std::chrono::system_clock::now());
		time = *std::localtime(&t);
	}
};

/*! \brief Modify \ref Alarm signals */
class MCR_API AlarmRef : public SignalManager
{
public:
	AlarmRef(Libmacro *context = NULL, mcr_Signal *sigPt = NULL) throw(int);

	inline const Alarm *data() const
	{
		return SignalManager::data<Alarm>();
	}
	inline Alarm *data()
	{
		return SignalManager::data<Alarm>();
	}

	inline tm time() const
	{
		if (data())
			return data()->time;
		return tm();
	}
	inline void setTime(tm &val) throw(int)
	{
		mkdata();
		data()->time = val;
	}

	inline int sec() const
	{
		if (data())
			return data()->sec();
		return 0;
	}
	inline void setSec(int val) throw(int)
	{
		mkdata();
		data()->sec() = val;
	}

	inline int min() const
	{
		if (data())
			return data()->min();
		return 0;
	}
	inline void setMin(int val) throw(int)
	{
		mkdata();
		data()->min() = val;
	}

	inline int hour() const
	{
		if (data())
			return data()->hour();
		return 0;
	}
	inline void setHour(int val) throw(int)
	{
		mkdata();
		data()->hour() = val;
	}

	inline int mday() const
	{
		if (data())
			return data()->mday();
		return 0;
	}
	inline void setMday(int val) throw(int)
	{
		mkdata();
		data()->mday() = val;
	}

	inline int mon() const
	{
		if (data())
			return data()->mon();
		return 0;
	}
	inline void setMon(int val) throw(int)
	{
		mkdata();
		data()->mon() = val;
	}

	inline int year() const
	{
		if (data())
			return data()->year();
		return 0;
	}
	inline void setYear(int val) throw(int)
	{
		mkdata();
		data()->year() = val;
	}

	inline int wday() const
	{
		if (data())
			return data()->wday();
		return 0;
	}
	inline void setWday(int val) throw(int)
	{
		mkdata();
		data()->wday() = val;
	}

	inline int yday() const
	{
		if (data())
			return data()->yday();
		return 0;
	}
	inline void setYday(int val) throw(int)
	{
		mkdata();
		data()->yday() = val;
	}

	inline int isdst() const
	{
		if (data())
			return data()->isdst();
		return 0;
	}
	inline void setIsdst(int val) throw(int)
	{
		mkdata();
		data()->isdst() = val;
	}
};
}

#endif
