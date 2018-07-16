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

/*! \file
 *  \brief Extra and C++ things not needed for Libmacro to function
 */

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_EXTRAS_H_
#define MCR_EXTRAS_EXTRAS_H_

#include "mcr/extras/alarm.h"
#include "mcr/extras/command.h"
#include "mcr/extras/string_key.h"
#include "mcr/extras/wrap_trigger.h"

#include "mcr/libmacro.h"

namespace mcr
{
struct MCR_EXTRAS_API Libmacro {
	/*! Base Libmacro context */
	mcr_context context;

	/*! ctor
	 *  \param enabled Initial enabled state, must be disabled before
	 *  destruction
	 */
	Libmacro(bool enabled = true);
	/*! dtor If this is enabled during destruction threading errors
	 *  are likely to happen
	 */
	~Libmacro();
	/*! Get the last created \ref Libmacro module
	 *
	 *  Will throw EFAULT if no \ref Libmacro exists, and will not create a
	 *  singleton object
	 */
	static Libmacro *instance();

	inline const mcr_context *ptr() const
	{
		return &context;
	}
	inline mcr_context *ptr()
	{
		return &context;
	}

	inline CtxISignal &iAlarm() const
	{
		return *_iAlarm;
	}
	inline CtxISignal &iCommand() const
	{
		return *_iCommand;
	}
	inline CtxISignal &iStringKey() const
	{
		return *_iStringKey;
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

	size_type characterCount() const;
	size_type characterCount(int c) const;
	Signal *character(int c);
	const Signal *character(int c) const;
	inline vector<Signal> characterSet(int c)
	{
		vector<Signal> ret;
		if (c < 0 || c >= (signed)characterCount())
			throw EINVAL;
		auto mem = character(c);
		for (auto iter = mem, endPt = mem ? mem + characterCount(c) : NULL;
			 iter != endPt; iter++) {
			ret.push_back(*iter);
		}
		return ret;
	}
	void setCharacter(int c, const Signal *valBuffer, size_t bufferLength);
	inline void setCharacterSet(int c, const vector<Signal> &val)
	{
		if (c < 0)
			throw EINVAL;
		setCharacter(c, val.data(), val.size());
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
	CtxISignal *_iAlarm;
	CtxISignal *_iCommand;
	CtxISignal *_iStringKey;
	/*! Map index to set of signals for \ref StringKey */
	void *_characters;  // vector<vector<Signal>>
	bool _enabled;

	/*! \ref mcr_is_platform Platform initializer
	 */
	void initialize();
	/*! \ref mcr_is_platform Platform deinitializer
	 */
	void deinitialize();
	inline vector<vector<Signal>> &characters() const
	{
		dassert(_characters);
		return *static_cast<vector<vector<Signal>> *>(_characters);
	}
};
}

#endif
