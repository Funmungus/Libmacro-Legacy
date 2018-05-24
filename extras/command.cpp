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

#include "mcr/extras/extras.h"

#include <cstring>

namespace mcr
{
IKeyProvider *Command::_keyProvider = NULL;

Command &Command::operator =(const Command &copytron)
{
	if (&copytron != this) {
		clear();
		setCryptic(copytron.cryptic());
		setFileText(copytron.fileText());
		setArgsText(copytron.argsText());
	}
	return *this;
}

void Command::setCryptic(bool val)
{
	if (val != cryptic() && _keyProvider) {
		_cryptic = val;
		file.setCryptic(val);
		for (size_t i = 0; i < _argsCount; i++) {
			_args[i].setCryptic(val);
		}
	}
}

void Command::args(SafeString *bufferOut, size_t bufferLen) const
{
	if (!bufferOut || !bufferLen)
		return;
	if (bufferLen > _argsCount)
		bufferLen = _argsCount;
	for (size_t i = 0; i < bufferLen; i++) {
		bufferOut[i] = _args[i];
	}
}

void Command::setArgs(SafeString *bufferIn, size_t bufferLen)
{
	if (!bufferIn || !bufferLen) {
		setArgsCount(0);
		return;
	}
	setArgsCount(bufferLen);
	for (size_t i = 0; i < bufferLen; i++) {
		_args[i] = bufferIn[i];
	}
}

int Command::compare(const Command &rhs) const
{
	int cmp;
	if (&rhs == this)
		return 0;
	if ((cmp = MCR_CMP(cryptic(), rhs.cryptic())))
		return cmp;
	if ((cmp = MCR_CMP(_argsCount, rhs._argsCount)))
		return cmp;
	if ((cmp = file.compare(rhs.file)))
		return cmp;
	for (size_t i = 0; i < _argsCount; i++) {
		if ((cmp = _args[i].compare(rhs._args[i])))
			return cmp;
	}
	return 0;
}

void Command::copy(const mcr::ISignalData *copytron)
{
	if (copytron == this)
		return;
	if (copytron) {
		const Command *mem = dynamic_cast<const Command *>(copytron);
		setArgsCount(mem->argsCount());
		_cryptic = mem->cryptic();
		file = mem->file;
		setArgs(mem->_args, mem->argsCount());
	} else {
		clear();
		_cryptic = false;
	}
}

void Command::setArgsCount(size_t count)
{
	if (_argsBufferLen < count) {
		delete []_args;
		_argsBufferLen = _argsCount = count;
		_args = new SafeString[_argsBufferLen];
		for (size_t i = 0; i < _argsBufferLen; i++) {
			_args[i].setKeyProvider(_keyProvider);
			_args[i].setCryptic(cryptic());
		}
	} else {
		_argsCount = count;
		for (size_t i = 0; i < _argsCount; i++) {
			_args[i].clear();
		}
	}
}

CommandRef::CommandRef(Libmacro *context, mcr_Signal *sigPt)
	: SignalManager(context, sigPt)
{
	init(&this->context()->iCommand().isignal);
}
}
