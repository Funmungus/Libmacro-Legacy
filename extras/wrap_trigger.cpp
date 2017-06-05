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

#include "mcr/extras/mod_extras.h"

namespace mcr
{
ITriggerRef::ITriggerRef(Libmacro *context, mcr_ITrigger *itrigPt) throw(int)
	: _context(context ? context : Libmacro::instance()), _itrigPt(itrigPt)
{
}

ITriggerRef::ITriggerRef(mcr_ITrigger *itrigPt) throw(int)
	: _context(Libmacro::instance()), _itrigPt(itrigPt)
{
}

ITriggerRef::ITriggerRef(Libmacro *context, size_t id) throw(int)
	: _context(context ? context : Libmacro::instance()),
	  _itrigPt(mcr_ITrigger_from_id(_context->ptr(), id))
{
}

ITriggerRef::ITriggerRef(size_t id) throw(int)
	: _context(Libmacro::instance()), _itrigPt(mcr_ITrigger_from_id(_context->ptr(), id))
{
}

ITriggerRef::ITriggerRef(Libmacro *context, const char *name) throw(int)
	: _context(context ? context : Libmacro::instance()),
	  _itrigPt(mcr_ITrigger_from_name(_context->ptr(), name))
{
}

ITriggerRef::ITriggerRef(const char *name) throw(int)
	: _context(Libmacro::instance()), _itrigPt(mcr_ITrigger_from_name(_context->ptr(), name))
{
}

void ITriggerRef::registerType(const char *name, const vector<string> &addNames) throw(int)
{
	int err = 0;
	mcr_IRegistry *regPt = mcr_ITrigger_reg(_context->ptr());
	dassert(regPt);
	if (_itrigPt) {
		auto end = addNames.end();
		if (_itrigPt->interface.id) {
			if (name)
				err = mcr_reg_set_name(regPt, _itrigPt, name);
		} else {
			err = mcr_register(regPt, _itrigPt, name, NULL, 0);
		}
		for (auto it = addNames.begin(); it != end; it++) {
			if (err)
				break;
			err = mcr_reg_add_name(regPt, _itrigPt, bytes(*it));
		}
		if (err)
			throw err;
	}
}

void ITriggerRef::setId(size_t val)
{
	_itrigPt = mcr_ITrigger_from_id(_context->ptr(), val);
}

const char *ITriggerRef::name()
{
	return _itrigPt ? mcr_ITrigger_name(_context->ptr(), _itrigPt) : NULL;
}

void ITriggerRef::setName(const char *name)
{
	_itrigPt = mcr_ITrigger_from_name(_context->ptr(), name);
}

TriggerRef::TriggerRef(Libmacro *context, mcr_Trigger *trigPt) throw(int)
	: _context(context ? context : Libmacro::instance()), _trigPt(trigPt)
{
}

TriggerRef::TriggerRef(mcr_Trigger *trigPt) throw(int)
	: _context(Libmacro::instance()), _trigPt(trigPt)
{
}

void TriggerRef::setITrigger(mcr_ITrigger *itrigPt) throw(int)
{
	int err;
	if (_trigPt && _trigPt->itrigger != itrigPt) {
		if ((err = mcr_Trigger_deinit(_trigPt)))
			throw err;
		_trigPt->itrigger = itrigPt;
	}
}

void TriggerRef::setId(size_t val) throw(int)
{
	int err;
	mcr_ITrigger *itrigPt = mcr_ITrigger_from_id(_context->ptr(), val);
	if (_trigPt && _trigPt->itrigger != itrigPt) {
		if ((err = mcr_Trigger_deinit(_trigPt)))
			throw err;
		_trigPt->itrigger = itrigPt;
	}
}

void TriggerRef::setName(const char *val) throw(int)
{
	int err;
	mcr_ITrigger *itrigPt = mcr_ITrigger_from_name(_context->ptr(), val);
	if (_trigPt && _trigPt->itrigger != itrigPt) {
		if ((err = mcr_Trigger_deinit(_trigPt)))
			throw err;
		_trigPt->itrigger = itrigPt;
	}
}

void TriggerRef::copy(const mcr_Trigger *copytron) throw(int)
{
	int err;
	if (_trigPt) {
		if ((err = mcr_Trigger_copy(_trigPt, copytron)))
			throw err;
	}
}

void TriggerRef::copy(const mcr::TriggerRef &copytron) throw(int)
{
	int err;
	if (_trigPt) {
		if ((err = mcr_Trigger_copy(_trigPt, copytron._trigPt)))
			throw err;
	}
}

void TriggerManager::copy(const mcr_Trigger *copytron) throw(int)
{
	int err = 0;
	if (copytron && copytron->itrigger == itrigger()) {
		err = mcr_Trigger_copy(triggerRef(), copytron);
	} else {
		err = mcr_Trigger_deinit(triggerRef());
	}
	if (err)
		throw err;
}

void TriggerManager::copy(const mcr::TriggerRef &copytron) throw(int)
{
	copy(copytron.triggerRef());
}

ActionRef::ActionRef(Libmacro *context, mcr_Trigger *trigPt) throw(int)
	: TriggerManager(context, trigPt)
{
	init(mcr_iAction(this->context()->ptr()));
}

StagedRef::StagedRef(Libmacro *context, mcr_Trigger *trigPt) throw(int)
	: TriggerManager(context, trigPt)
{
	init(mcr_iStaged(this->context()->ptr()));
}

vector<Stage> StagedRef::stages() const
{
	vector<Stage> ret;
	const mcr_Staged *pt = data<mcr_Staged>();
	Stage mem(context());
	if (pt) {
#define localPush(itPt) mem = (const mcr_Stage *)itPt; ret.push_back(mem);
		MCR_ARR_FOR_EACH(pt->stages, localPush);
#undef localPush
	}
	return ret;
}

void StagedRef::setStages(const vector<Stage> &val) throw(int)
{
	int err;
	size_type i;
	mcr_Staged *pt = data<mcr_Staged>();
	mcr_Stage mem;
	mcr_Stage_init(&mem);
	if (pt) {
		for (i = 0; i < val.size(); i++) {
			err = mcr_Stage_copy(&mem, &val[i]);
			if (err)
				throw(err);
			err = mcr_Array_push(&pt->stages, &mem);
			if (err) {
				mcr_Stage_deinit(&mem);
				throw(err);
			}
			mcr_Stage_init(&mem);
		}
	}
	mcr_Stage_deinit(&mem);
}
}
