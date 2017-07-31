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
 * \brief \ref Command - Execute a shell command, similar to \ref execvp,
 * with restrictions and without closing current process.
 */

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_COMMAND_H
#define MCR_EXTRAS_COMMAND_H

#include "mcr/extras/wrap_signal.h"
#include "mcr/extras/isignal_data.h"
#include "mcr/extras/safe_string.h"

namespace mcr
{
/*! \brief Execute a shell command, similar to \ref execvp */
struct MCR_EXTRAS_API Command : public ISignalData {
	/*! \brief Executable file */
	SafeString file;

	Command(bool cryptic = false) MCR_THROWS
		: file(_keyProvider, "", cryptic), _args(NULL), _argsBufferLen(0),
		  _argsCount(0), _cryptic(cryptic && _keyProvider)
	{
	}
	Command(const Command &copytron) MCR_THROWS
		: file(copytron.file), _args(NULL), _argsBufferLen(0), _argsCount(0),
		  _cryptic(copytron.cryptic())
	{
		setArgsText(copytron.argsText());
	}
	Command &operator =(const Command &copytron) MCR_THROWS;

	/*!
	 * \brief Get command from signal
	 *
	 * \param sigPt \ref opt \ref mcr_Signal *
	 */
	static inline Command *data(mcr_Signal *sigPt)
	{
		return (Command *)(sigPt ? sigPt->instance.data.data : NULL);
	}

	/*!
	 * \brief \ref mcr_is_platform Same as standard execvp, except raised
	 * permissions are removed in the program that executes
	 * \param Filename Executable file
	 * \param ArgList Shell comman arguments
	 */
//	static void execvp(const char *Filename, char * const ArgList[]) MCR_THROWS;
	static void setKeyProvider(IKeyProvider *provider)
	{
		_keyProvider = provider;
	}

	/*! \brief If true, encrypt all command strings */
	inline bool cryptic() const
	{
		return _cryptic;
	}
	/*! \brief Set encryption state for all command strings */
	void setCryptic(bool val) MCR_THROWS;

	/*! \brief \ref file */
	inline string fileText() const MCR_THROWS
	{
		return file.text();
	}
	/*! \brief \ref file */
	inline void setFileText(const string &val) MCR_THROWS
	{
		file.setText(val);
	}

	/*! \brief Number of \ref args */
	inline size_t argsCount() const throw()
	{
		return _argsCount;
	}
	/*! \brief Shell command arguments, argv of execvp */
	void args(SafeString *bufferOut, size_t bufferLen) const MCR_THROWS;
	/*! \brief \ref args */
	inline vector<string> argsText() const MCR_THROWS
	{
		vector<string> ret;
		for (size_t i = 0; i < _argsCount; i++) {
			ret.push_back(_args[i].text());
		}
		return ret;
	}
	/*! \brief \ref args */
	void setArgs(SafeString *bufferIn, size_t bufferLen) MCR_THROWS;
	/*! \brief \ref args */
	inline void setArgsText(const vector<string> &val) MCR_THROWS
	{
		setArgsCount(val.size());
		for (size_t i = 0; i < _argsCount; i++) {
			_args[i].setText(val[i]);
		}
	}

	/*! \brief \ref mcr_Signal_compare */
	virtual int compare(const ISignalData &rhs) const MCR_THROWS override
	{
		return compare(dynamic_cast<const Command &>(rhs));
	}
	/*! \brief \ref mcr_Signal_compare */
	int compare(const Command &rhs) const MCR_THROWS;
	/*! \brief \ref mcr_Signal_copy
	 * \param copytron \ref opt
	 */
	virtual void copy(const ISignalData *copytron) MCR_THROWS override;
	/*! \brief \ref mcr_ISignal_set_name */
	virtual const char *name() const override
	{
		return "Command";
	}
	virtual size_t addNamesCount() const override
	{
		return 1;
	}
	virtual void addNames(const char **bufferOut,
			      size_t bufferLength) const override
	{
		if (bufferOut && bufferLength)
			bufferOut[0] = "Cmd";
	}
	/*! \brief \ref mcr_is_platform \ref mcr_send */
	virtual void send() MCR_THROWS override;

	inline void clear() throw()
	{
		file.clear();
		setArgsCount(0);
	}
private:
	static IKeyProvider *_keyProvider;
	SafeString *_args;
	size_t _argsBufferLen;
	size_t _argsCount;
	bool _cryptic;

	/* This will always clear all current args */
	void setArgsCount(size_t count);
};

/*! \brief Modify \ref Command signals */
class MCR_EXTRAS_API CommandRef : public SignalManager
{
public:
	CommandRef(Libmacro *context = NULL, mcr_Signal *sigPt = NULL) MCR_THROWS;

	inline const Command *data() const
	{
		return SignalManager::data<Command>();
	}
	inline Command *data()
	{
		return SignalManager::data<Command>();
	}

	inline bool cryptic() const
	{
		if (data())
			return data()->cryptic();
		return false;
	}
	inline void setCryptic(bool val) MCR_THROWS
	{
		mkdata();
		data()->setCryptic(val);
	}

	inline string file() const
	{
		if (data())
			return data()->fileText();
		return "";
	}
	inline void setFile(const string &val) MCR_THROWS
	{
		mkdata();
		data()->setFileText(val);
	}

	inline vector<string> args() const
	{
		if (data())
			return data()->argsText();
		return vector<string>();
	}
	inline void setArgs(const vector<string> &val) MCR_THROWS
	{
		mkdata();
		data()->setArgsText(val);
	}
};
}

#endif
