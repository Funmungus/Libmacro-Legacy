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
struct MCR_API Libmacro {
	/*! \brief Base Libmacro context */
	mcr_context context;
	/*! \brief \ref Alarm */
	ISignal<Alarm> iAlarm;
	/*! \brief \ref Command */
	ISignal<Command> iCommand;
	/*! \brief \ref StringKey */
	ISignal<StringKey> iStringKey;

	/*! \brief Map index to set of signals for \ref StringKey */
	vector<vector<Signal>> characters;

	/*! \brief ctor
	 * \param enabled Initial enabled state, must be disabled before
	 * destruction
	 */
	Libmacro(bool enabled = true) throw(int);
	/*! \brief dtor If this is enabled during destruction threading errors
	 * are likely to happen
	 */
	~Libmacro() throw(int);
	/*!
	 * \brief Get the last created \ref Libmacro module
	 *
	 * Will throw EFAULT if no \ref Libmacro exists, and will not create a
	 * singleton object
	 */
	static Libmacro *instance() throw(int);

	inline const mcr_context *ptr() const
	{
		return &context;
	}
	inline mcr_context *ptr()
	{
		return &context;
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
	void setEnabled(bool val) throw(int);

	const vector<Signal> &character(int c) const throw(int)
	{
		if (c < 0 || c >= (signed)characters.size())
			throw EINVAL;
		return characters[c];
	}
	vector<Signal> &character(int c) throw(int)
	{
		if (c < 0 || c >= (signed)characters.size())
			throw EINVAL;
		return characters[c];
	}
	void setCharacter(int c, vector<Signal> &val) throw(int);
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
	void setKeyChar(int c, int key, long msecDelay, bool shiftFlag) throw(int);
	/*!
	 * \brief Set all \ref mcr_NoOp delay values for \ref StringKey
	 *
	 * \param delayValue Apply this pause time
	 */
	void setCharacterDelays(mcr_NoOp delayValue) throw(int);
	void removeCharacter(int c) throw(int);
	void trimCharacters();
private:
	static vector<Libmacro *> _registry;
	bool _enabled;

	/*!
	 * \brief \ref mcr_is_platform Platform initializer
	 */
	void initialize() throw(int);
	/*!
	 * \brief \ref mcr_is_platform Platform deinitializer
	 */
	void deinitialize();
};
}

#endif
