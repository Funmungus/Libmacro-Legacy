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

#include <cstring>
#include "mcr/extras/mod_extras.h"

namespace mcr
{
IKeyProvider *Command::_keyProvider = NULL;

Command &Command::operator =(const Command &copytron) throw(int)
{
	if (&copytron != this) {
		clear();
		setCryptic(copytron.cryptic());
		setFileText(copytron.fileText());
		setArgsText(copytron.argsText());
	}
	return *this;
}

void Command::setCryptic(bool val) throw(int)
{
	size_type i;
	if (val != cryptic() && _keyProvider) {
		_cryptic = val;
		file.setCryptic(val);
		for (i = 0; i < args.size(); i++) {
			args[i].setCryptic(val);
		}
	}
}

vector<string> Command::argsText() const throw(int)
{
	vector<string> ret;
	size_type i;
	for (i = 0; i < args.size(); i++) {
		ret.push_back(args[i].text());
	}
	return ret;
}

void Command::setArgsText(const vector<string> &val) throw(int)
{
	size_type i;
	args.clear();
	for (i = 0; i < val.size(); i++) {
		args.push_back(SafeString(_keyProvider, val[i], _cryptic));
	}
}

int Command::compare(const Command &rhs) const throw(int)
{
	int cmp;
	size_type i;
	if (&rhs == this)
		return 0;
	if ((cmp = MCR_CMP_INTEGRAL(cryptic(), rhs.cryptic())))
		return cmp;
	if ((cmp = MCR_CMP_INTEGRAL(args.size(), rhs.args.size())))
		return cmp;
	if ((cmp = file.compare(rhs.file)))
		return cmp;
	for (i = 0; i < args.size(); i++) {
		if ((cmp = args[i].compare(rhs.args[i])))
			return cmp;
	}
	return 0;
}

void Command::copy(const mcr::ISignalData *copytron) throw(int)
{
	if (copytron == this)
		return;
	if (copytron) {
		const Command *mem = dynamic_cast<const Command *>(copytron);
		clear();
		_cryptic = mem->cryptic();
		setFileText(mem->fileText());
		setArgsText(mem->argsText());
	} else {
		clear();
		_cryptic = false;
	}
}

CommandRef::CommandRef(Libmacro *context, mcr_Signal *sigPt) throw(int)
	: SignalManager(context, sigPt)
{
	init(&this->context()->iCommand.isignal);
}
}
