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

/*! \file
 *  \brief Extra and C++ things not needed for Libmacro to function
 */

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_EXTRAS_H_
#define MCR_EXTRAS_EXTRAS_H_

#include "mcr/extras/command.h"
#include "mcr/extras/interrupt.h"
#include "mcr/extras/string_key.h"
#include "mcr/extras/wrap_trigger.h"
#include "mcr/extras/macros_interrupted.h"

namespace mcr
{
struct MCR_API Libmacro final {
	MacrosInterrupted macrosInterrupted;

	/*! ctor
	 *  \param enabled Initial enabled state, must be disabled before
	 *  destruction
	 */
	Libmacro(bool enabled = true);
	Libmacro(const Libmacro &);
	/*! dtor If this is enabled during destruction threading errors
	 *  are likely to happen
	 */
	~Libmacro();
	Libmacro &operator =(const Libmacro &);
	/*! Get the last created \ref Libmacro module
	 *
	 *  Will throw EFAULT if no \ref Libmacro exists, and will not create a
	 *  singleton object
	 */
	static Libmacro *instance();

	inline const mcr_context *ptr() const
	{
		return _context;
	}
	inline mcr_context *ptr()
	{
		return _context;
	}

	inline mcr_ISignal *iHidEcho()
	{
		return mcr_iHidEcho(ptr());
	}
	inline mcr_ISignal *iKey()
	{
		return mcr_iKey(ptr());
	}
	inline mcr_ISignal *iModifier()
	{
		return mcr_iModifier(ptr());
	}
	inline mcr_ISignal *iMoveCursor()
	{
		return mcr_iMoveCursor(ptr());
	}
	inline mcr_ISignal *iNoOp()
	{
		return mcr_iNoOp(ptr());
	}
	inline mcr_ISignal *iScroll()
	{
		return mcr_iScroll(ptr());
	}
	inline CtxISignal &iCommand() const
	{
		return *_iCommand;
	}
	inline CtxISignal &iInterrupt() const
	{
		return *_iInterrupt;
	}
	inline CtxISignal &iStringKey() const
	{
		return *_iStringKey;
	}

	inline mcr_ITrigger *iAction()
	{
		return mcr_iAction(ptr());
	}
	inline mcr_ITrigger *iStaged()
	{
		return mcr_iStaged(ptr());
	}
	/*! Generic enabler for mcr_context functions, such as
	 *  hardware hooks and any sort of threading.
	 */
	inline bool isEnabled() const
	{
		return _enabled;
	}
	/*! Set current enabled state.  Please disable before the
	 *  destructor to avoid threading errors.
	 */
	void setEnabled(bool val);

	/// \todo Find a place for Key registry characters
	size_t characterCount() const;
	size_t characterCount(int c) const;
	Signal *characterData(int c) const;

	/*! Set signals for a character as a list of signals. */
	inline void setCharacter(int c, const std::vector<Signal> &val)
	{
		if (c < 0)
			throw EINVAL;
		setCharacter(c, &val.front(), val.size());
	}
	/*! Set signals for a character that will press, pause, and release a
	 *  key.
	 *
	 *  \param c Character to set signals for
	 *  \param key Keyboard key to press and release
	 *  \param msecDelay Milliseconds to pause between key press and release
	 *  \param shiftFlag If true a shift key will press before the key, and
	 *  release after the key.
	 *  \return \ref reterr
	 */
	void setCharacterKey(int c, int key, long msecDelay, bool shiftFlag);
	/*! Set all \ref mcr_NoOp delay values for \ref StringKey
	 *
	 *  \param delayValue Apply this pause time
	 */
	void setCharacterDelays(mcr_NoOp delayValue);
	void removeCharacter(int c);
	void trimCharacters();
	void clearCharacters();
private:
	/*! Base Libmacro context */
	mcr_context *_context;
	CtxISignal *_iCommand;
	CtxISignal *_iInterrupt;
	CtxISignal *_iStringKey;
	/*! Map index to set of signals for \ref StringKey
	 *  vector<vector<Signal>> */
	void *_characters;
	bool _enabled;

	void construct(bool enabled);
	/*! \ref mcr_is_platform Platform initializer
	 */
	void initialize();
	/*! \ref mcr_is_platform Platform deinitializer
	 */
	void deinitialize();
	inline SignalSetSet &charactersRef() const
	{
		return *static_cast<SignalSetSet *>(_characters);
	}
	/* Get reference to Signal set for character, resize _characters if needed */
	inline SignalSet &characterRef(int c) const
	{
		auto &chara = charactersRef();
		size_t sizeC = static_cast<size_t>(c);
		if (c < 0)
			throw EINVAL;
		if (sizeC >= chara.size())
			chara.resize(sizeC + 1);
		return chara[sizeC];
	}

	void setCharacter(int c, const Signal *valueArray, size_t count);
};
}

#endif
