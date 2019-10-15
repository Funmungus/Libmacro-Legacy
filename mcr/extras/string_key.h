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

/*! \file
 *  \brief \ref StringKey - Turn a string into a set of signals, such as a key
 *  press
 */

#ifndef MCR_EXTRAS_STRING_KEY_H_
#define MCR_EXTRAS_STRING_KEY_H_

#include "mcr/extras/wrap_signal.h"
#include "mcr/extras/isignal_data.h"
#include "mcr/extras/safe_string.h"

namespace mcr
{
/*! Turn a string into a set of signals, such as a key press */
struct MCR_API StringKey: public ISignalData {
	/*! Pause in-between characters */
	mcr_NoOp interval;
	/*! Set of characters to turn into signals */
	SafeString string;

	StringKey(bool cryptic = false)
		: string(_keyProvider, "", cryptic)
	{
		interval.sec = 0;
		interval.msec = 40;
	}
	StringKey(const StringKey &copytron)
		: ISignalData(), interval(copytron.interval), string(copytron.string)
	{
	}
	virtual ~StringKey() override = default;
	StringKey &operator =(const StringKey &copytron)
	{
		if (&copytron != this) {
			interval = copytron.interval;
			string = copytron.string;
		}
		return *this;
	}

	/*! Get a StringKey from a signal
	 *
	 *  \param sigPt \ref opt \ref mcr_Signal *
	 */
	static inline StringKey *data(mcr_Signal *sigPt)
	{
		return static_cast<StringKey *>(mcr_Instance_data(sigPt));
	}

	static void setKeyProvider(IKeyProvider *provider)
	{
		_keyProvider = provider;
	}

	inline bool cryptic() const
	{
		return string.cryptic();
	}
	void setCryptic(bool val)
	{
		if (val != cryptic())
			string.setCryptic(val);
	}

	inline int sec() const
	{
		return interval.sec;
	}
	void setSec(int val)
	{
		interval.sec = val;
	}
	inline int msec() const
	{
		return interval.msec;
	}
	void setMsec(int val)
	{
		interval.msec = val;
	}

	/*! Set of characters to turn into signals */
	inline std::string text() const
	{
		return string.text();
	}
	/*! Set the string to turn into signals */
	inline void setText(const std::string &val)
	{
		string.setText(val);
	}

	/*! \ref mcr_Signal_compare */
	virtual int compare(const IData &rhs) const override
	{
		return compare(dynamic_cast<const StringKey &>(rhs));
	}
	/*! \ref mcr_Signal_compare */
	int compare(const StringKey &rhs) const;
	/*! \ref mcr_Signal_copy
	 *  \param copytron \ref opt
	 */
	virtual void copy(const IData *copytron) override;
	/*! \ref mcr_ISignal_set_name */
	virtual const char *name() const override
	{
		return "StringKey";
	}
	virtual size_t addNameCount() const override
	{
		return 2;
	}
	virtual void addNames(const char **bufferOut,
						  size_t bufferLength) const override
	{
		const char *names[] = {"String Key", "string_key"};
		size_t i, count = arrlen(names);
		if (!bufferOut)
			return;
		for (i = 0; i < count && i < bufferLength; i++) {
			bufferOut[i] = names[i];
		}
	}
	/*! \ref mcr_send */
	virtual void send() override;
private:
	static IKeyProvider *_keyProvider;
};
}

#endif
