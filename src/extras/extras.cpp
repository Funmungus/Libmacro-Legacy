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

#include "mcr/extras/isignal.h"
#include "mcr/libmacro.h"

namespace mcr
{
/* Reduce bloating with private static void reference */
static std::vector<void *> _registry;

Libmacro::Libmacro(bool enabled)
	: _context(new mcr_context),
	  _iCommand(new ISignal<Command>(this)),
	  _iInterrupt(new ISignal<Interrupt>(this)),
	  _iStringKey(new ISignal<StringKey>(this)),
	  _characters(new SignalSetSet),
	  _enabled(false)
{
	construct(enabled);
}

Libmacro::Libmacro(const Libmacro &copytron)
	: _context(new mcr_context),
	  _iCommand(new ISignal<Command>(this)),
	  _iInterrupt(new ISignal<Interrupt>(this)),
	  _iStringKey(new ISignal<StringKey>(this)),
	  _characters(new SignalSetSet),
	  _enabled(false)
{
	construct(copytron.isEnabled());
	/* Cannot copy macros from other context */
}

Libmacro::~Libmacro()
{
	SafeString::deinitialize();
	deinitialize();
	if (!_registry.empty()) {
		/* Unregister context that is being removed */
		for (auto iter = _registry.begin(), endPt = _registry.end(); iter != endPt;
			 iter++) {
			/* Assume placed only once */
			if (*iter == this) {
				_registry.erase(iter);
				break;
			}
		}
	}
	if (isEnabled()) {
		fprintf(stderr, "Error: Libmacro context was not disabled "
				"before destruction.  "
				"Threading errors may occur.\n");
		fprintf(stderr, "Warning: mcr_deinitialize errors are ignored\n");
		mcr_deinitialize(ptr());
	} else if (mcr_deinitialize(ptr())) {
		dmsg;
	}
	delete _iCommand;
	_iCommand = nullptr;
	delete _iInterrupt;
	_iInterrupt = nullptr;
	delete _iStringKey;
	_iStringKey = nullptr;
	delete &charactersRef();
	_characters = nullptr;
	delete _context;
	_context = nullptr;
}

Libmacro &Libmacro::operator=(const Libmacro &copytron)
{
	if (&copytron != this)
		setEnabled(copytron.isEnabled());
	/* Cannot copy macros from other context */
	return *this;
}

Libmacro *Libmacro::instance()
{
	if (_registry.size() == 0)
		throw EFAULT;
	return static_cast<Libmacro *>(_registry.back());
}

void Libmacro::setEnabled(bool val)
{
	if (val != _enabled) {
		_enabled = val;
		mcr_Dispatcher_set_enabled_all(ptr(), val);
		/* Operation not permitted if not elevated permissions */
		if (mcr_intercept_set_enabled(ptr(), val) &&
			mcr_err != EPERM) {
			throw mcr_err;
		}
	}
}

size_t Libmacro::characterCount() const
{
	return charactersRef().size();
}

size_t Libmacro::characterCount(int c) const
{
	return characterRef(c).size();
}

Signal *Libmacro::characterData(int c) const
{
	auto &chara = characterRef(c);
	if (chara.empty())
		return nullptr;
	return &characterRef(c).front();
}

void Libmacro::setCharacterKey(int c, int key, long msecDelay,
							   bool shiftFlag)
{
	Signal delaySig(iNoOp()), keySig(iKey());
	SignalRef ref;
	size_t i;
	/* Shift: 2 keys to press */
	int gearShift = shiftFlag ? 2 : 1;
	auto &chara = characterRef(c);
	mcr_Key *kPt;
	/* With shift will be: down, pause, down, pause, up, pause, up */
	chara.resize(shiftFlag ? 7 : 3);
	for (i = 0; i < chara.size(); i++) {
		ref = chara[i];
		/* First set ISignal and instance data. Odd numbers are delays */
		if (i % 2) {
			ref.build(iNoOp()).mkdata();
			mcr_NoOp_set_all(ref.data<mcr_NoOp>(), msecDelay / 1000, msecDelay % 1000);
		} else {
			kPt = ref.build(iKey()).mkdata().data<mcr_Key>();
			kPt->key = key;
			if (gearShift) {
				--gearShift;
				kPt->apply = MCR_SET;
			} else {
				kPt->apply = MCR_UNSET;
			}
		}
	}
	if (shiftFlag) {
		gearShift = mcr_Key_modifier_key(ptr(), MCR_SHIFT);
		chara[0].data<mcr_Key>()->key = gearShift;
		chara[6].data<mcr_Key>()->key = gearShift;
	}
}

void Libmacro::setCharacterDelays(mcr_NoOp delayValue)
{
	auto &chara = charactersRef();
	SignalRef siggy;
	mcr_ISignal *isigPt = iNoOp();
	for (auto &i: chara) {
		for (auto &iter: i) {
			siggy = iter.ptr();
			if (siggy.isignal() == isigPt)
				*siggy.mkdata().data<mcr_NoOp>() = delayValue;
		}
	}
}

void Libmacro::removeCharacter(int c)
{
	auto &chara = charactersRef();
	size_t sizeC = static_cast<size_t>(c);
	if (c < 0)
		throw EINVAL;
	if (sizeC < chara.size())
		chara[sizeC].clear();
}

void Libmacro::trimCharacters()
{
	auto &chara = charactersRef();
	size_t i;
	if (!chara.size())
		return;
	/* Stop at last index with elements */
	for (i = chara.size() - 1; i && !chara[i].size(); i--);

	/* i is either 0(with or without elements), or index has elements */
	if (chara[i].size()) {
		/* Index found with elements, all past this are empty */
		/* For debugging make sure either this is already last element,
		 * or next element is empty */
		dassert(chara.size() == i + 1 ||
				!chara[i + 1].size());
		chara.resize(i + 1);
	} else {
		/* At index 0, and has no elements. */
		chara.clear();
	}
}

void Libmacro::clearCharacters()
{
	charactersRef().clear();
}

void Libmacro::construct(bool enabled)
{
	if (mcr_initialize(ptr()))
		throw mcr_err;
	if (mcr_load_contracts(ptr())) {
		mcr_deinitialize(ptr());
		throw mcr_err;
	}
	mcr_trim(ptr());
	/* Register */
	_registry.push_back(this);
	ISignal<Command>::cast(_iCommand)->registerType();
	ISignal<Interrupt>::cast(_iInterrupt)->registerType();
	ISignal<StringKey>::cast(_iStringKey)->registerType();
	initialize();
	SafeString::initialize();
	setEnabled(enabled);
	if (!Interrupt::defaultInterrupt)
		Interrupt::defaultInterrupt = &macrosInterrupted;
}

void Libmacro::setCharacter(int c, const Signal *valueArray, size_t count)
{
	auto &chara = charactersRef();
	size_t sizeC = static_cast<size_t>(c), i;
	/* Count without array is a logical error */
	if (c < 0 || (count && !valueArray))
		throw EINVAL;
	if (sizeC >= chara.size())
		chara.resize(sizeC + 1);
	/* If no count, shorthand array to empty */
	if (!count)
		valueArray = nullptr;
	chara[sizeC] = SignalSet();
	for (i = 0; i < count; i++) {
		chara[sizeC].push_back(valueArray[i]);
	}
}
}
