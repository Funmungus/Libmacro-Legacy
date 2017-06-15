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
vector<Libmacro *> Libmacro::_registry;

Libmacro::Libmacro(bool enabled) throw(int)
	: iAlarm(this), iCommand(this), iStringKey(this), _enabled(false)
{
	int err = mcr_initialize(ptr(), true, true);
	if (err)
		throw(err);
	/* Register */
	_registry.push_back(this);
	iAlarm.registerType();
	iCommand.registerType();
	iStringKey.registerType();
	initialize();
	SafeString::initialize();
	setEnabled(enabled);
}

Libmacro::~Libmacro() throw(int)
{
	int err;
	SafeString::deinitialize();
	deinitialize();
	/* Unregister context that is being removed */
	auto end = _registry.end();
	for (auto iter = _registry.begin(); iter != end; iter++) {
		/* Assume placed only once */
		if (*iter == this) {
			_registry.erase(iter);
			break;
		}
	}
	if (isEnabled()) {
		fprintf(stderr, "Error: Libmacro context was not disabled "
				"before destruction.  "
				"Threading errors may occur.\n");
		fprintf(stderr, "Warning: mcr_deinitialize errors are ignored\n");
		mcr_deinitialize(ptr());
	} else if ((err = mcr_deinitialize(ptr()))) {
		throw(err);
	}
}

Libmacro *Libmacro::instance() throw(int)
{
	if (_registry.size() == 0)
		throw EFAULT;
	return &*_registry.back();
}

void Libmacro::setEnabled(bool val) throw(int)
{
	int err;
	if (val != _enabled) {
		_enabled = val;
		mcr_Dispatcher_set_enabled_all(ptr(), val);
		/* Operation not permitted if not admin elevated */
		if ((err = mcr_intercept_set_enabled(ptr(), val)) &&
				err != EPERM) {
			throw err;
		}
	}
}

void Libmacro::setCharacter(int c, vector<Signal> &val) throw(int)
{
	size_type i;
	if (c < 0)
		throw EINVAL;
	if (c >= (signed)characters.size())
		characters.resize(c + 1);
	vector<Signal> &mem = characters[c];
	mem.resize(val.size());
	for (i = 0; i < val.size(); i++) {
		mem[i] = val[i];
	}
}

void Libmacro::setKeyChar(int c, int key, long msecDelay, bool shiftFlag) throw(int)
{
	Signal delaySig, keySig;
	NoOpRef nMan(this, delaySig.ptr());
	KeyRef keyMan(this, keySig.ptr());
	vector<Signal> localSet;
	size_type i;
	nMan.mkdata();
	mcr_NoOp_set_all(nMan.data<mcr_NoOp>(), msecDelay / 1000, msecDelay % 1000);
	keyMan.mkdata();
	mcr_Key_set_all(keyMan.data<mcr_Key>(), key, 0, MCR_DOWN);
	/* shift = 4 keys, 3 delays, non-shift = 2 keys, 1 delay */
	localSet.resize(shiftFlag ? 7 : 3);
	for (i = 0; i < localSet.size(); i++) {
		/* Odd numbers are delays */
		if (i % 2)
			localSet[i] = delaySig;
		else
			localSet[i] = keySig;
	}
	if (shiftFlag) {
		int shift = mcr_Key_mod_key(ptr(), MCR_SHIFT);
		mcr_Key_set_all(MCR_KEY_DATA(localSet[0].signal), shift, shift, MCR_DOWN);
		localSet[6] = localSet[0];
		keyMan.setSignal(localSet[4].ptr());
		keyMan.setUpType(MCR_UP);
		keyMan.setSignal(localSet[6].ptr());
		keyMan.setUpType(MCR_UP);
	} else {
		keyMan.setSignal(localSet[2].ptr());
		keyMan.setUpType(MCR_UP);
	}
	setCharacter(c, localSet);
}

void Libmacro::setCharacterDelays(mcr_NoOp delayValue) throw(int)
{
	size_type i, j;
	vector<Signal> *vecMem;
	SignalRef siggy;
	mcr_ISignal *isigPt = mcr_iNoOp(ptr());
	for (i = 0; i < characters.size(); i++) {
		vecMem = &characters[i];
		for (j = 0; j < vecMem->size(); j++) {
			siggy = vecMem->at(j).ptr();
			if (siggy.isignal() == isigPt) {
				siggy.mkdata();
				*siggy.data<mcr_NoOp>() = delayValue;
			}
		}
	}
}

void Libmacro::removeCharacter(int c) throw(int)
{
	if (c < 0)
		throw EINVAL;
	if (c < (signed)characters.size())
		characters[c].clear();
}

void Libmacro::trimCharacters()
{
	size_type i;
	if (!characters.size())
		return;
	/* Stop at last index with elements */
	for (i = characters.size() - 1; i > 0 && !characters[i].size(); i--);

	/* i is either 0(with or without elements), or index has elements */
	if (characters[i].size()) {
		/* Index found with elements */
		characters.resize(i + 1);
	} else {
		/* No index with any elements */
		characters.clear();
	}
}
}
