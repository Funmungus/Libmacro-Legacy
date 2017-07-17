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

/*!
 * \file
 * \brief \ref mcr_mod_extras - Extras module
 *
 * In case of extreme complexity, break glass
 */

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_MOD_EXTRAS_H
#define MCR_EXTRAS_MOD_EXTRAS_H

#include "mcr/modules.h"
#include "mcr/extras/extras.h"

namespace mcr
{
struct MCR_EXTRAS_API Libmacro {
	/*! \brief Base Libmacro context */
	mcr_context context;

	/*! \brief ctor
	 * \param enabled Initial enabled state, must be disabled before
	 * destruction
	 */
	Libmacro(bool enabled = true) MCR_THROWS;
	/*! \brief dtor If this is enabled during destruction threading errors
	 * are likely to happen
	 */
	~Libmacro();
	/*!
	 * \brief Get the last created \ref Libmacro module
	 *
	 * Will throw EFAULT if no \ref Libmacro exists, and will not create a
	 * singleton object
	 */
	static Libmacro *instance() MCR_THROWS;

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
	/*! \brief Generic enabler for mcr_context functions, such as
	 * hardware hooks and any sort of threading.
	 */
	inline bool isEnabled() const
	{
		return _enabled;
	}
	/*! \brief Set current enabled state.  Please disable before the
	 * destructor to avoid threading errors.
	 */
	void setEnabled(bool val) MCR_THROWS;

	inline size_t characterCount() const
	{
		return _characters.used();
	}
	inline size_t characterCount(int c) const
	{
		auto mem = _characters.element<Array>(c);
		return mem ? mem->used() : 0;
	}
	inline Signal *character(int c) MCR_THROWS
	{
		if (c < 0 || c >= (signed)characterCount())
			throw EINVAL;
		return _characters.element<Array>(c)->first<Signal>();
	}
	inline const Signal *character(int c) const MCR_THROWS
	{
		if (c < 0 || c >= (signed)characterCount())
			throw EINVAL;
		return _characters.element<Array>(c)->first<Signal>();
	}
	inline vector<Signal> characterSet(int c) MCR_THROWS
	{
		vector<Signal> ret;
		if (c < 0 || c >= (signed)_characters.used())
			throw EINVAL;
		auto mem = _characters.element<Array>(c);
		for (auto iter = mem->first<Signal>(), endPt = mem->end<Signal>();
		     iter != endPt; iter++) {
			ret.push_back(*iter);
		}
		return ret;
	}
	void setCharacter(int c, const Signal *valBuffer, size_t bufferLength) MCR_THROWS;
	inline void setCharacterSet(int c, const vector<Signal> &val) MCR_THROWS
	{
		if (c < 0)
			throw EINVAL;
		setCharacter(c, val.data(), val.size());
	}
	/*!
	 * \brief Set signals for a character that will press, pause, and release a
	 * key.
	 *
	 * \param c Character to set signals for
	 * \param key Keyboard key to press and release
	 * \param msecDelay Milliseconds to pause between key press and release
	 * \param shiftFlag If true a shift key will press before the key, and
	 * release after the key.
	 * \return \ref reterr
	 */
	void setCharacterKey(int c, int key, long msecDelay, bool shiftFlag) MCR_THROWS;
	/*!
	 * \brief Set all \ref mcr_NoOp delay values for \ref StringKey
	 *
	 * \param delayValue Apply this pause time
	 */
	void setCharacterDelays(mcr_NoOp delayValue) MCR_THROWS;
	void removeCharacter(int c) MCR_THROWS;
	void trimCharacters();
private:
	CtxISignal *_iAlarm;
	CtxISignal *_iCommand;
	CtxISignal *_iStringKey;
	/*! \brief Map index to set of signals for \ref StringKey */
	Array _characters;
	bool _enabled;

	/*!
	 * \brief \ref mcr_is_platform Platform initializer
	 */
	void initialize() MCR_THROWS;
	/*!
	 * \brief \ref mcr_is_platform Platform deinitializer
	 */
	void deinitialize();
};
}

#endif
