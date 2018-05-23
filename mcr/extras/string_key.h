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
 * \brief \ref StringKey - Turn a string into a set of signals, such as a key
 * press
 */

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_STRING_KEY_H_
#define MCR_EXTRAS_STRING_KEY_H_

#include "mcr/extras/wrap_signal.h"
#include "mcr/extras/isignal_data.h"
#include "mcr/extras/safe_string.h"

namespace mcr
{
/*! Turn a string into a set of signals, such as a key press */
struct MCR_EXTRAS_API StringKey: public ISignalData {
	/*! Pause in-between characters */
	mcr_NoOp interval;
	/*! Set of characters to turn into signals */
	SafeString string;

	StringKey(bool cryptic = false)
		: string(_keyProvider, "", cryptic), _cryptic(cryptic && _keyProvider)
	{
		interval.sec = 0;
		interval.msec = 40;
	}
	StringKey(const StringKey &copytron)
		: interval(copytron.interval), string(_keyProvider, "", copytron.cryptic()),
		  _cryptic(copytron.cryptic())
	{
	}
	StringKey &operator =(const StringKey &copytron);

	/*! Get a StringKey from a signal
	 *
	 * \param sigPt \ref opt \ref mcr_Signal *
	 */
	static inline StringKey *data(mcr_Signal *sigPt)
	{
		return (StringKey *)(sigPt ? sigPt->instance.data.data : NULL);
	}

	static void setKeyProvider(IKeyProvider *provider)
	{
		_keyProvider = provider;
	}

	inline bool cryptic() const
	{
		return _cryptic;
	}
	void setCryptic(bool val)
	{
		if (val != cryptic() && _keyProvider) {
			_cryptic = val;
			string.setCryptic(val);
		}
	}

	/*! Set of characters to turn into signals */
	inline mcr::string stringText() const
	{
		return string.text();
	}
	/*! Set the string to turn into signals */
	inline void setStringText(const mcr::string &val)
	{
		string.setText(val);
	}

	/*! \ref mcr_Signal_compare */
	virtual int compare(const ISignalData &rhs) const override
	{
		return compare(dynamic_cast<const StringKey &>(rhs));
	}
	/*! \ref mcr_Signal_compare */
	int compare(const StringKey &rhs) const;
	/*! \ref mcr_Signal_copy
	 * \param copytron \ref opt
	 */
	virtual void copy(const ISignalData *copytron) override;
	/*! \ref mcr_ISignal_set_name */
	virtual const char *name() const override
	{
		return "StringKey";
	}
	virtual size_t addNamesCount() const override
	{
		return 3;
	}
	virtual void addNames(const char **bufferOut,
						  size_t bufferLength) const override
	{
		const char *names[] = {"String Key", "string_key", "SK"};
		size_t i, count = arrlen(names);
		if (!bufferOut)
			return;
		for (i = 0; i < count && i < bufferLength; i++) {
			bufferOut[i] = names[i];
		}
	}
	/*! \ref mcr_send */
	virtual void send() override;

	inline void clear()
	{
		string.clear();
	}
private:
	static IKeyProvider *_keyProvider;
	bool _cryptic;
};

/*! Modify \ref StringKey signals */
class MCR_EXTRAS_API StringKeyRef : public SignalManager
{
public:
	StringKeyRef(Libmacro *context = NULL, mcr_Signal *sigPt = NULL);

	inline mcr_NoOp interval() const
	{
		if (data<StringKey>())
			return data<StringKey>()->interval;
		mcr_NoOp ret;
		ret.sec = 0;
		ret.msec = 0;
		return ret;
	}
	inline void setInterval(mcr_NoOp val)
	{
		mkdata();
		data<StringKey>()->interval = val;
	}
	inline int sec() const
	{
		if (data<StringKey>())
			return data<StringKey>()->interval.sec;
		return 0;
	}
	inline void setSec(int val)
	{
		mkdata();
		data<StringKey>()->interval.sec = val;
	}
	inline int msec() const
	{
		if (data<StringKey>())
			return data<StringKey>()->interval.msec;
		return 0;
	}
	inline void setMsec(int val)
	{
		mkdata();
		data<StringKey>()->interval.msec = val;
	}

	inline bool cryptic() const
	{
		if (data<StringKey>())
			return data<StringKey>()->cryptic();
		return false;
	}
	inline void setCryptic(bool val)
	{
		mkdata();
		data<StringKey>()->setCryptic(val);
	}

	inline mcr::string string() const
	{
		if (data<StringKey>())
			return data<StringKey>()->stringText();
		return "";
	}
	inline void setString(const mcr::string &val)
	{
		mkdata();
		data<StringKey>()->setStringText(val);
	}
};
}

#endif
