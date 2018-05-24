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

#include "mcr/extras/isignal.h"

namespace mcr
{
/* Reduce bloating with private static void reference */
static vector<void *> _registry;

Libmacro::Libmacro(bool enabled)
	: _iAlarm(new ISignal<Alarm>(this)),
	  _iCommand(new ISignal<Command>(this)),
	  _iStringKey(new ISignal<StringKey>(this)),
	  _characters(new vector<vector<Signal>>()),
	  _enabled(false)
{
	int err = mcr_initialize(ptr());
	if (err)
		throw(err);
	if ((err = mcr_load_contracts(ptr()))) {
		mcr_deinitialize(ptr());
		throw(err);
	}
	mcr_trim(ptr());
	/* Register */
	_registry.push_back(this);
	((ISignal<Alarm> *)_iAlarm)->registerType();
	((ISignal<Command> *)_iCommand)->registerType();
	((ISignal<StringKey> *)_iStringKey)->registerType();
	initialize();
	SafeString::initialize();
	setEnabled(enabled);
}

Libmacro::~Libmacro()
{
	int err;
	SafeString::deinitialize();
	deinitialize();
	/* Unregister context that is being removed */
	for (auto iter = _registry.begin(), endPt = _registry.end(); iter != endPt;
		 iter++) {
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
		dmsg;
	}
	delete _iAlarm;
	delete _iCommand;
	delete _iStringKey;
	delete &characters();
	_characters = NULL;
}

Libmacro *Libmacro::instance()
{
	if (_registry.size() == 0)
		throw EFAULT;
	return (Libmacro *)_registry[_registry.size() - 1];
}

void Libmacro::setEnabled(bool val)
{
	int err;
	if (val != _enabled) {
		_enabled = val;
		mcr_Dispatcher_set_enabled_all(ptr(), val);
		/* Operation not permitted if not elevated permissions */
		if ((err = mcr_intercept_set_enabled(ptr(), val)) &&
			err != EPERM) {
			throw err;
		}
	}
}

size_type Libmacro::characterCount() const
{
	return characters().size();
}

size_type Libmacro::characterCount(int c) const
{
	auto mem = characters();
	if (c < 0 || c >= (signed)mem.size())
		return 0;
	return mem[c].size();
}

Signal *Libmacro::character(int c)
{
	auto mem = characters();
	if (c < 0 || c >= (signed)mem.size())
		throw EINVAL;
	return mem[c].data();
}

const Signal *Libmacro::character(int c) const
{
	auto mem = characters();
	if (c < 0 || c >= (signed)mem.size())
		throw EINVAL;
	return mem[c].data();
}

void Libmacro::setCharacter(int c, const Signal *valBuffer,
							size_t bufferLength)
{
	auto mem = characters();
	if (c < 0)
		throw EINVAL;
	if (c >= (signed)mem.size())
		mem.resize(c + 1);
	auto arr = mem[c];
	arr.resize(bufferLength);
	for (size_t i = 0; i < bufferLength; i++) {
		arr[i] = valBuffer[i];
	}
}

void Libmacro::setCharacterKey(int c, int key, long msecDelay,
							   bool shiftFlag)
{
	/* Sanity early to avoid object instances */
	if (c < 0)
		throw EINVAL;
	auto mem = characters();
	Signal delaySig, keySig;
	NoOpRef nMan(this, delaySig.ptr());
	KeyRef keyMan(this, keySig.ptr());
	/* Signal pattern is always key + delay + key, which means an odd number */
	size_type i;
	vector<Signal> localSet(shiftFlag ? 7 : 3);
	nMan.mkdata();
	mcr_NoOp_set_all(nMan.data<mcr_NoOp>(), msecDelay / 1000, msecDelay % 1000);
	keyMan.mkdata();
	mcr_Key_set_all(keyMan.data<mcr_Key>(), key, MCR_DOWN);
	for (i = 0; i < localSet.size(); i++) {
		/* Odd numbers are delays */
		if (i % 2)
			localSet[i] = delaySig;
		else
			localSet[i] = keySig;
	}
	if (shiftFlag) {
		int shift = mcr_Key_mod_key(ptr(), MCR_SHIFT);
		mcr_Key_set_all(MCR_KEY_DATA(localSet[0].signal), shift, MCR_DOWN);
		localSet[6] = localSet[0];
		keyMan.setSignal(localSet[4].ptr());
		keyMan.setUpType(MCR_UP);
		keyMan.setSignal(localSet[6].ptr());
		keyMan.setUpType(MCR_UP);
	} else {
		keyMan.setSignal(localSet[2].ptr());
		keyMan.setUpType(MCR_UP);
	}
	if (c >= (signed)mem.size())
		mem.resize(c + 1);
	mem[c] = localSet;
}

void Libmacro::setCharacterDelays(mcr_NoOp delayValue)
{
	auto mem = characters();
	SignalRef siggy;
	mcr_ISignal *isigPt = mcr_iNoOp(ptr());
	for (size_type i = 0; i < mem.size(); i++) {
		auto sigArr = mem[i];
		for (auto iter = sigArr.begin(), endPt = sigArr.end();
			 iter != endPt; iter++) {
			siggy = iter->ptr();
			if (siggy.isignal() == isigPt) {
				siggy.mkdata();
				*siggy.data<mcr_NoOp>() = delayValue;
			}
		}
	}
}

void Libmacro::removeCharacter(int c)
{
	auto mem = characters();
	if (c < 0)
		throw EINVAL;
	if (c < (signed)mem.size())
		mem[c].clear();
}

void Libmacro::trimCharacters()
{
	auto mem = characters();
	size_t i;
	if (!mem.size())
		return;
	/* Stop at last index with elements */
	for (i = mem.size() - 1; !mem[i].size(); i--);

	/* i is either 0(with or without elements), or index has elements */
	if (mem[i].size()) {
		/* Index found with elements, all past this are empty */
		/* For debugging make sure either this is already last element,
		 * or next element is empty */
		dassert(mem.size() == i + 1 ||
				!mem[i + 1].size());
		mem.resize(i + 1);
	} else {
		mem.clear();
	}
}

void Libmacro::clearCharacters()
{
	characters().clear();
}
}
