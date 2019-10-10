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
ITriggerRef::ITriggerRef(Libmacro *context, mcr_ITrigger *itrigPt)
	: _context(context ? context : Libmacro::instance()), _itrigPt(itrigPt)
{
}

ITriggerRef::ITriggerRef(mcr_ITrigger *itrigPt)
	: _context(Libmacro::instance()), _itrigPt(itrigPt)
{
}

ITriggerRef::ITriggerRef(Libmacro *context, size_t id)
	: _context(context ? context : Libmacro::instance()),
	  _itrigPt(mcr_ITrigger_from_id(_context->ptr(), id))
{
}

ITriggerRef::ITriggerRef(size_t id)
	: _context(Libmacro::instance()), _itrigPt(mcr_ITrigger_from_id(_context->ptr(),
			id))
{
}

ITriggerRef::ITriggerRef(Libmacro *context, const char *name)
	: _context(context ? context : Libmacro::instance()),
	  _itrigPt(mcr_ITrigger_from_name(_context->ptr(), name))
{
}

ITriggerRef::ITriggerRef(const char *name)
	: _context(Libmacro::instance()),
	  _itrigPt(mcr_ITrigger_from_name(_context->ptr(), name))
{
}

void ITriggerRef::registerType(const char *name, const char **addNames,
							   size_t addNamesCount)
{
	mcr_IRegistry *regPt = mcr_ITrigger_reg(_context->ptr());
	dassert(regPt);
	if (_itrigPt) {
		if (_itrigPt->interface.id) {
			mcr_reg_set_names(regPt, _itrigPt, name, addNames, addNamesCount);
		} else {
			mcr_register(regPt, _itrigPt, name, nullptr, 0);
		}
		if (mcr_err)
			throw mcr_read_err();
	}
}

void ITriggerRef::setId(size_t val)
{
	_itrigPt = mcr_ITrigger_from_id(_context->ptr(), val);
}

const char *ITriggerRef::name()
{
	return _itrigPt ? mcr_ITrigger_name(_context->ptr(), _itrigPt) : nullptr;
}

void ITriggerRef::setName(const char *name)
{
	_itrigPt = mcr_ITrigger_from_name(_context->ptr(), name);
}

TriggerRef::TriggerRef(Libmacro *context, mcr_Trigger *trigPt)
	: _context(context ? context : Libmacro::instance()), _trigPt(trigPt)
{
}

TriggerRef::TriggerRef(mcr_Trigger *trigPt)
	: _context(Libmacro::instance()), _trigPt(trigPt)
{
}

void TriggerRef::setITrigger(mcr_ITrigger *itrigPt)
{
	if (_trigPt && _trigPt->itrigger != itrigPt) {
		if (mcr_Trigger_deinit(_trigPt))
			throw mcr_read_err();
		_trigPt->itrigger = itrigPt;
	}
}

void TriggerRef::setId(size_t val)
{
	mcr_ITrigger *itrigPt = mcr_ITrigger_from_id(_context->ptr(), val);
	if (_trigPt && _trigPt->itrigger != itrigPt) {
		if (mcr_Trigger_deinit(_trigPt))
			throw mcr_read_err();
		_trigPt->itrigger = itrigPt;
	}
}

void TriggerRef::setName(const char *val)
{
	mcr_ITrigger *itrigPt = mcr_ITrigger_from_name(_context->ptr(), val);
	if (_trigPt && _trigPt->itrigger != itrigPt) {
		if (mcr_Trigger_deinit(_trigPt))
			throw mcr_read_err();
		_trigPt->itrigger = itrigPt;
	}
}

void TriggerRef::copy(const mcr_Trigger *copytron)
{
	if (_trigPt) {
		if (mcr_Trigger_copy(_trigPt, copytron))
			throw mcr_read_err();
	}
}

void TriggerRef::copy(const TriggerRef &copytron)
{
	if (_trigPt) {
		if (mcr_Trigger_copy(_trigPt, copytron._trigPt))
			throw mcr_read_err();
	}
}
}
