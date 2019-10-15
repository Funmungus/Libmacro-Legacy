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
	mcr_IRegistry *regPt = mcr_ISignal_reg(_context->ptr());
	dassert(regPt);
	if (_isigPt) {
		if (_isigPt->interface.id) {
			mcr_reg_set_names(regPt, _isigPt, name, addNames, addNamesCount);
		} else {
			mcr_register(regPt, _isigPt, name, addNames, addNamesCount);
		}
		if (mcr_err)
			throw mcr_read_err();
	}
}

void ISignalRef::setId(size_t val)
{
	_isigPt = mcr_ISignal_from_id(_context->ptr(), val);
}

const char *ISignalRef::name()
{
	return _isigPt ? mcr_ISignal_name(_context->ptr(), _isigPt) : nullptr;
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
	if (_sigPt && _sigPt->isignal != isigPt) {
		if (mcr_Signal_deinit(_sigPt))
			throw mcr_read_err();
		_sigPt->isignal = isigPt;
	}
}

void SignalRef::setId(size_t val)
{
	mcr_ISignal *isigPt = mcr_ISignal_from_id(_context->ptr(), val);
	if (_sigPt && _sigPt->isignal != isigPt) {
		if (mcr_Signal_deinit(_sigPt))
			throw mcr_read_err();
		_sigPt->isignal = isigPt;
	}
}

void SignalRef::setName(const char *val)
{
	mcr_ISignal *isigPt = mcr_ISignal_from_name(_context->ptr(), val);
	if (_sigPt && _sigPt->isignal != isigPt) {
		if (mcr_Signal_deinit(_sigPt))
			throw mcr_read_err();
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
	if (_sigPt) {
		if (mcr_send(_context->ptr(), _sigPt))
			throw mcr_read_err();
	}
}

void SignalRef::copy(const mcr_Signal *copytron)
{
	if (_sigPt) {
		if (mcr_Signal_copy(_sigPt, copytron))
			throw mcr_read_err();
	}
}

void SignalRef::copy(const SignalRef &copytron)
{
	if (_sigPt) {
		if (mcr_Signal_copy(_sigPt, copytron._sigPt))
			throw mcr_read_err();
	}
}
}
