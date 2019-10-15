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
 *  \brief \ref Command - Execute a shell command, similar to \ref execvp,
 *  with restrictions and without closing current process.
 */

#ifndef MCR_EXTRAS_COMMAND_H_
#define MCR_EXTRAS_COMMAND_H_

#include "mcr/extras/wrap_signal.h"
#include "mcr/extras/isignal_data.h"
#include "mcr/extras/safe_string.h"

namespace mcr
{
/*! Execute a shell command, similar to \ref execvp */
struct MCR_API Command : public ISignalData {
	Command(bool cryptic = false);
	Command(const Command &copytron);
	virtual ~Command() override;
	Command &operator =(const Command &copytron);

	static void setKeyProvider(IKeyProvider *provider)
	{
		_keyProvider = provider;
	}

	/*! exec file */
	inline std::string file() const
	{
		return _file.text();
	}
	/*! exec file */
	inline void setFile(const std::string &val)
	{
		_file.setText(val);
	}

	/*! Number of exec args */
	inline size_t argCount() const
	{
		return _argCount;
	}
	/*! exec args
	 *
	 *  Requires vector-equivalent functions reserve and push_back.
	 *  Requires std::string elements.
	 */
	template<class T = std::vector<std::string>>
	inline T args() const
	{
		T ret;
		auto ptr = argsData();
		size_t i, count = argCount();
		ret.reserve(count);
		for (i = 0; i < count; i++) {
			ret.push_back(ptr[i].text());
		}
		return ret;
	}
	/*! exec args
	 *
	 *  Requires vector or initializer-equivalent function size.
	 *  Requires std::string elements.
	 */
	template<class T = std::vector<std::string>>
	inline void setArgs(const T &val)
	{
		size_t i = 0;
		setArgCount(val.size());
		for (auto &iter: val) {
			setArg(i++, iter);
		}
	}
	inline void setArg(size_t index, const std::string &value)
	{
		setArg(index, value.c_str());
	}

	/*! If true, encrypt all command strings */
	inline bool cryptic() const
	{
		return _cryptic;
	}
	/*! Set encryption state for all command strings */
	void setCryptic(bool val);

	/*! \ref mcr_Signal_compare */
	virtual int compare(const IData &rhs) const override
	{
		return compare(dynamic_cast<const Command &>(rhs));
	}
	/*! \ref mcr_Signal_compare */
	virtual int compare(const Command &rhs) const;
	/*! \ref mcr_Signal_copy
	 *  \param copytron \ref opt
	 */
	virtual void copy(const IData *copytron) override;
	/*! \ref mcr_ISignal_set_name */
	virtual const char *name() const override
	{
		return "Command";
	}
	virtual size_t addNameCount() const override
	{
		return 1;
	}
	virtual void addNames(const char **bufferOut,
						  size_t bufferLength) const override
	{
		if (bufferOut && bufferLength)
			bufferOut[0] = "Cmd";
	}
	/*! \ref mcr_is_platform \ref mcr_send */
	virtual void send() override;

	inline void clear()
	{
		_file.clear();
		setArgCount(0);
	}
private:
	static IKeyProvider *_keyProvider;
	/*! Executable file */
	SafeString _file;
	/*! vector<SafeString> */
	void *_args;
	/*! Optimization, length of current args vector. */
	size_t _argCount;
	bool _cryptic;

	/* This will always clear all current args */
	void setArgCount(size_t count);
	SafeString *argsData() const;
	const char *arg(size_t index) const;
	void setArg(size_t index, const char *value);
	inline std::vector<SafeString> &argsRef() const
	{
		return *static_cast<std::vector<SafeString> *>(_args);
	}
	inline void setArgs(const std::vector<SafeString> &value)
	{
		auto &args = argsRef();
		args = value;
		_argCount = args.size();
	}
};
}

#endif
