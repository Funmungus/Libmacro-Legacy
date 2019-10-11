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

#include "mcr/extras/extras.h"

namespace mcr
{
IKeyProvider *Command::_keyProvider = nullptr;

Command::Command(bool cryptic)
	: _file(_keyProvider, "", cryptic), _args(new std::vector<SafeString>),
	  _argCount(0), _cryptic(cryptic && _keyProvider)
{
}

Command::Command(const Command &copytron)
	: ISignalData(), _file(copytron._file), _args(new std::vector<SafeString>(copytron.argsRef())),
	  _argCount(copytron.argCount()), _cryptic(copytron.cryptic())
{
}

Command::~Command()
{
	delete &argsRef();
}

Command &Command::operator =(const Command &copytron)
{
	if (&copytron != this) {
		clear();
		setCryptic(copytron.cryptic());
		setFile(copytron.file());
		setArgs(copytron.argsRef());
	}
	return *this;
}

void Command::setCryptic(bool val)
{
	auto &args = argsRef();
	if (val != cryptic() && _keyProvider) {
		_cryptic = val;
		_file.setCryptic(val);
		for (auto &str: args) {
			str.setCryptic(val);
		}
	}
}

int Command::compare(const Command &rhs) const
{
	int cmp;
	auto &args = argsRef();
	auto &rArgs = rhs.argsRef();
	if (&rhs == this)
		return 0;
	if ((cmp = MCR_CMP(cryptic(), rhs.cryptic())))
		return cmp;
	if ((cmp = MCR_CMP(args.size(), rArgs.size())))
		return cmp;
	if ((cmp = _file.compare(rhs._file)))
		return cmp;
	for (size_t i = 0; i < args.size(); i++) {
		if ((cmp = args[i].compare(rArgs[i])))
			return cmp;
	}
	return 0;
}

void Command::copy(const IData *copytron)
{
	if (copytron == this)
		return;
	if (copytron) {
		const Command *mem = dynamic_cast<const Command *>(copytron);
		_cryptic = mem->cryptic();
		_file = mem->_file;
		setArgs(mem->argsRef());
	} else {
		clear();
		_cryptic = false;
	}
}

void Command::setArgCount(size_t count)
{
	argsRef().resize(count);
	_argCount = count;
}

SafeString *Command::argsData() const
{
	if (argsRef().empty())
		return nullptr;
	return &argsRef().front();
}

const char *Command::arg(size_t index) const
{
	auto &args = argsRef();
	if (index >= args.size())
		throw EINVAL;
	std::string s = args[index].text();
	return s.c_str();
}

void Command::setArg(size_t index, const char *value)
{
	auto &args = argsRef();
	if (index >= args.size())
		args.resize(index + 1);
	args[index].setText(value);
}
}
