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

namespace mcr
{
ISignalRef::ISignalRef(Libmacro *context, mcr_ISignal *isigPt)
	: _context(context ? context : Libmacro::instance()), _isigPt(isigPt)
{
}

ISignalRef::ISignalRef(mcr_ISignal *isigPt)
	: _context(Libmacro::instance()), _isigPt(isigPt)
{
}

ISignalRef::ISignalRef(Libmacro *context, size_t id)
	: _context(context ? context : Libmacro::instance()),
	  _isigPt(mcr_ISignal_from_id(_context->ptr(), id))
{
}

ISignalRef::ISignalRef(size_t id)
	: _context(Libmacro::instance()), _isigPt(mcr_ISignal_from_id(_context->ptr(),
			id))
{
}

ISignalRef::ISignalRef(Libmacro *context, const char *name)
	: _context(context ? context : Libmacro::instance()),
	  _isigPt(mcr_ISignal_from_name(_context->ptr(), name))
{
}

ISignalRef::ISignalRef(const char *name)
	: _context(Libmacro::instance()), _isigPt(mcr_ISignal_from_name(_context->ptr(),
			name))
{
}

void ISignalRef::registerType(const char *name, const char **addNames,
							  size_t addNamesCount)
{
	int err = 0;
	mcr_IRegistry *regPt = mcr_ISignal_reg(_context->ptr());
	dassert(regPt);
	if (_isigPt) {
		if (_isigPt->interface.id) {
			err = mcr_reg_set_names(regPt, _isigPt, name, addNames, addNamesCount);
		} else {
			err = mcr_register(regPt, _isigPt, name, addNames, addNamesCount);
		}
		if (err)
			throw err;
	}
}

void ISignalRef::setId(size_t val)
{
	_isigPt = mcr_ISignal_from_id(_context->ptr(), val);
}

const char *ISignalRef::name()
{
	return _isigPt ? mcr_ISignal_name(_context->ptr(), _isigPt) : NULL;
}

void ISignalRef::setName(const char *name)
{
	_isigPt = mcr_ISignal_from_name(_context->ptr(), name);
}

SignalRef::SignalRef(Libmacro *context, mcr_Signal *sigPt)
	: _context(context ? context : Libmacro::instance()), _sigPt(sigPt)
{
}

SignalRef::SignalRef(mcr_Signal *sigPt)
	: _context(Libmacro::instance()), _sigPt(sigPt)
{
}

void SignalRef::setISignal(mcr_ISignal *isigPt)
{
	int err;
	if (_sigPt && _sigPt->isignal != isigPt) {
		if ((err = mcr_Signal_deinit(_sigPt)))
			throw err;
		_sigPt->isignal = isigPt;
	}
}

void SignalRef::setId(size_t val)
{
	int err;
	mcr_ISignal *isigPt = mcr_ISignal_from_id(_context->ptr(), val);
	if (_sigPt && _sigPt->isignal != isigPt) {
		if ((err = mcr_Signal_deinit(_sigPt)))
			throw err;
		_sigPt->isignal = isigPt;
	}
}

void SignalRef::setName(const char *val)
{
	int err;
	mcr_ISignal *isigPt = mcr_ISignal_from_name(_context->ptr(), val);
	if (_sigPt && _sigPt->isignal != isigPt) {
		if ((err = mcr_Signal_deinit(_sigPt)))
			throw err;
		_sigPt->isignal = isigPt;
	}
}

bool SignalRef::dispatch()
{
	if (_sigPt)
		return mcr_dispatch(_context->ptr(), _sigPt);
	return false;
}

void SignalRef::send()
{
	int err;
	if (_sigPt) {
		if ((err = mcr_send(_context->ptr(), _sigPt)))
			throw err;
	}
}

void SignalRef::copy(const mcr_Signal *copytron)
{
	int err;
	if (_sigPt) {
		if ((err = mcr_Signal_copy(_sigPt, copytron)))
			throw err;
	}
}

void SignalRef::copy(const mcr::SignalRef &copytron)
{
	int err;
	if (_sigPt) {
		if ((err = mcr_Signal_copy(_sigPt, copytron._sigPt)))
			throw err;
	}
}

void SignalManager::copy(const mcr_Signal *copytron)
{
	int err = 0;
	if (copytron && copytron->isignal == isignal()) {
		err = mcr_Signal_copy(signal(), copytron);
	} else {
		err = mcr_Signal_deinit(signal());
	}
	if (err)
		throw err;
}

void SignalManager::copy(const mcr::SignalRef &copytron)
{
	copy(copytron.signal());
}

HidEchoRef::HidEchoRef(Libmacro *context, mcr_Signal *sigPt)
	: SignalManager(context, sigPt)
{
	init(mcr_iHidEcho(this->context()->ptr()));
}

size_t HidEchoRef::count() const
{
	return mcr_HidEcho_count(context()->ptr());
}

const char *HidEchoRef::name(size_t id) const
{
	return mcr_HidEcho_name(context()->ptr(), id);
}

size_t HidEchoRef::echo(const char *name) const
{
	return mcr_HidEcho_name_echo(context()->ptr(), name);
}

KeyRef::KeyRef(Libmacro *context, mcr_Signal *sigPt)
	: SignalManager(context, sigPt)
{
	init(mcr_iKey(this->context()->ptr()));
}

size_t KeyRef::count() const
{
	return mcr_Key_count(context()->ptr());
}

const char *KeyRef::name(int keyCode) const
{
	return mcr_Key_name(context()->ptr(), keyCode);
}

int KeyRef::key(const char *name) const
{
	return mcr_Key_name_key(context()->ptr(), name);
}

ModsRef::ModsRef(Libmacro *context, mcr_Signal *sigPt)
	: SignalManager(context, sigPt)
{
	init(mcr_iMods(this->context()->ptr()));
}

size_t ModsRef::count() const
{
	return mcr_ModFlags_count(context()->ptr());
}

const char *ModsRef::name(int modifier) const
{
	return mcr_ModFlags_name(context()->ptr(), modifier);
}

int ModsRef::modifier(const char *name) const
{
	return mcr_ModFlags_modifier(context()->ptr(), name);
}

MoveCursorRef::MoveCursorRef(Libmacro *context, mcr_Signal *sigPt)
	: SignalManager(context, sigPt)
{
	init(mcr_iMoveCursor(this->context()->ptr()));
}

NoOpRef::NoOpRef(Libmacro *context, mcr_Signal *sigPt)
	: SignalManager(context, sigPt)
{
	init(mcr_iNoOp(this->context()->ptr()));
}

ScrollRef::ScrollRef(Libmacro *context, mcr_Signal *sigPt)
	: SignalManager(context, sigPt)
{
	init(mcr_iScroll(this->context()->ptr()));
}
}
