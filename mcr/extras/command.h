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
 * \brief \ref Command - Execute a shell command, similar to \ref execvp
 */

#ifndef __cplusplus
#pragma message "C++ support is required for extras module"
#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_COMMAND_H
#define MCR_EXTRAS_COMMAND_H

#include "mcr/extras/isignal.h"
#include "mcr/extras/safe_string.h"

namespace mcr
{
/*! \brief Execute a shell command, similar to \ref execvp */
struct MCR_API Command : public ISignalData {
	/*! \brief Executable file */
	SafeString file;
	/*! \brief Shell command arguments, argv of execvp */
	vector<SafeString> args;

	Command(bool cryptic = false) throw(int)
		: file(_keyProvider, "", cryptic), _cryptic(cryptic && _keyProvider)
	{
	}
	Command(const Command &copytron) throw(int)
		: file(copytron.file), _cryptic(copytron.cryptic())
	{
		setArgsText(copytron.argsText());
	}
	Command &operator =(const Command &copytron) throw(int);

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
//	static void execvp(const char *Filename, char * const ArgList[]) throw(int);
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
	void setCryptic(bool val) throw(int);

	/*! \brief \ref file */
	inline string fileText() const throw(int)
	{
		return file.text();
	}
	/*! \brief \ref file */
	void setFileText(const string &val) throw(int)
	{
		file.setText(val);
	}

	/*! \brief \ref args */
	vector<string> argsText() const throw(int);
	/*! \brief \ref args */
	void setArgsText(const vector<string> &val) throw(int);

	/*! \brief \ref mcr_Signal_compare */
	virtual int compare(const ISignalData &rhs) const throw(int) override
	{
		return compare(dynamic_cast<const Command &>(rhs));
	}
	/*! \brief \ref mcr_Signal_compare */
	int compare(const Command &rhs) const throw(int);
	/*! \brief \ref mcr_Signal_copy
	 * \param copytron \ref opt
	 */
	virtual void copy(const ISignalData *copytron) throw(int) override;
	/*! \brief \ref mcr_ISignal_set_name */
	virtual const char *name() override
	{
		return "Command";
	}
	/*! \brief \ref mcr_is_platform \ref mcr_send */
	virtual void send() throw(int) override;

	inline void clear()
	{
		file.clear();
		args.clear();
	}
private:
	static IKeyProvider *_keyProvider;
	bool _cryptic;
};

/*! \brief Modify \ref Command signals */
class MCR_API CommandRef : public SignalManager
{
public:
	CommandRef(Libmacro *context = NULL, mcr_Signal *sigPt = NULL) throw(int);

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
	inline void setCryptic(bool val) throw(int)
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
	inline void setFile(const string &val) throw(int)
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
	inline void setArgs(const vector<string> &val) throw(int)
	{
		mkdata();
		data()->setArgsText(val);
	}
};
}

#endif
