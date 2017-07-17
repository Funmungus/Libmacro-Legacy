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
#include "mcr/extras/isignal.h"

namespace mcr
{
/* Reduce bloating with private static void reference */
static vector<void *> _registry;

Libmacro::Libmacro(bool enabled) MCR_THROWS
	: _iAlarm(new ISignal<Alarm>(this)),
	  _iCommand(new ISignal<Command>(this)),
	  _iStringKey(new ISignal<StringKey>(this)), _characters(NULL, sizeof(Array)),
	  _enabled(false)
{
	int err = mcr_initialize(ptr(), true, true);
	if (err)
		throw(err);
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
	for (auto iter = _characters.first<Array>(), endPt = _characters.end<Array>();
	     iter != endPt; iter++) {
		iter->clear<Signal>();
	}
	_characters.clear<Array>();
}

Libmacro *Libmacro::instance() MCR_THROWS
{
	if (_registry.size() == 0)
		throw EFAULT;
	return (Libmacro *)_registry[_registry.size() - 1];
}

void Libmacro::setEnabled(bool val) MCR_THROWS
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

void Libmacro::setCharacter(int c, const Signal *valBuffer, size_t bufferLength) MCR_THROWS
{
	if (c < 0)
		throw EINVAL;
	if (c >= (signed)_characters.used())
		_characters.minUsed<Array>(c + 1);
	auto mem = _characters.element<Array>(c);
	mem->clear<Signal>();
	if (bufferLength) {
		mem->minUsed<Signal>(bufferLength);
		for (size_t i = 0; i < bufferLength; i++) {
			*mem->element<Signal>(i) = valBuffer[i];
		}
	}
}

void Libmacro::setCharacterKey(int c, int key, long msecDelay, bool shiftFlag) MCR_THROWS
{
	Signal delaySig, keySig;
	NoOpRef nMan(this, delaySig.ptr());
	KeyRef keyMan(this, keySig.ptr());
	/* Signal pattern is always key + delay + key, which means an odd number */
	size_t i, signalCount = shiftFlag ? 7 : 3;
	nMan.mkdata();
	mcr_NoOp_set_all(nMan.data<mcr_NoOp>(), msecDelay / 1000, msecDelay % 1000);
	keyMan.mkdata();
	mcr_Key_set_all(keyMan.data<mcr_Key>(), key, 0, MCR_DOWN);
	Signal *localSet = new Signal[signalCount];
	try {
		for (i = 0; i < signalCount; i++) {
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
		setCharacter(c, localSet, signalCount);
	} catch (int err) {
		delete []localSet;
		localSet = NULL;
		throw err;
	}
	if (localSet)
		delete []localSet;
}

void Libmacro::setCharacterDelays(mcr_NoOp delayValue) MCR_THROWS
{
	SignalRef siggy;
	mcr_ISignal *isigPt = mcr_iNoOp(ptr());
	for (size_t i = 0; i < _characters.used(); i++) {
		auto sigArr = _characters.element<Array>(i);
		for (auto iter = sigArr->first<Signal>(), endPt = sigArr->end<Signal>();
		     iter != endPt; iter++ ) {
			siggy = iter->ptr();
			if (siggy.isignal() == isigPt) {
				siggy.mkdata();
				*siggy.data<mcr_NoOp>() = delayValue;
			}
		}
	}
}

void Libmacro::removeCharacter(int c) MCR_THROWS
{
	if (c < 0)
		throw EINVAL;
	if (c < (signed)_characters.used())
		_characters.element<Array>(c)->clear<Signal>();
}

void Libmacro::trimCharacters()
{
	size_t i;
	if (!_characters.used())
		return;
	/* Stop at last index with elements */
	for (i = _characters.used() - 1; !_characters.element<Array>(i)->used(); i--);

	/* i is either 0(with or without elements), or index has elements */
	if (_characters.element<Array>(i)->used()) {
		/* Index found with elements, all past this are empty */
		/* For debugging make sure either this is already last element,
		 * or next element is empty */
		dassert(_characters.used() == i + 1 ||
			!_characters.element<Array>(i + 1)->used());
		_characters.resize<Array>(i + 1);
	} else {
		/* No index with any elements */
		for (auto iter = _characters.first<Array>(), endPt = _characters.end<Array>();
		     iter < endPt; iter++) {
			iter->clear<Signal>();
		}
		_characters.clear<Array>();
	}
}
}
