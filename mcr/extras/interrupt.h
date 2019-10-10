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
 *  \brief
 */

#ifndef MCR_EXTRAS_INTERRUPT_H_
#define MCR_EXTRAS_INTERRUPT_H_

#include "mcr/extras/wrap_signal.h"
#include "mcr/extras/iinterrupt.h"
#include "mcr/extras/isignal_data.h"

namespace mcr
{
struct MCR_API Interrupt : public ISignalData {
	/*! Set this to iintercept if none is provided */
	static IInterrupt *defaultInterrupt;
	/*! Set of Macros to do interrupt actions on */
	IInterrupt *iInterrupt;
	/*! How interrupt is processed
	 *
	 * \ref mcr_Interrupt */
	int type;

	Interrupt(IInterrupt *iintercept = nullptr,
			  const char *target = "All",
			  int type = MCR_INTERRUPT_ALL);
	Interrupt(const Interrupt &);
	~Interrupt();
	Interrupt &operator =(const Interrupt &);

	virtual int compare(const IData &rhs) const;
	virtual void copy(const IData *copytron);
	virtual const char *name() const
	{
		return "Interrupt";
	}
	virtual void send();

	/*! Null, empty, or name of macro.
	 *
	 *  All, case insensitive, is reserved to interrupt all macros. */
	const char *target() const;
	void setTarget(const char *val);
private:
	/* std::string */
	void *_target;
	inline std::string &targetRef() const
	{
		return *static_cast<std::string *>(_target);
	}
};
}

#endif
