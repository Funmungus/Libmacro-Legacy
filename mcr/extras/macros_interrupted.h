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
 */

#ifndef MCR_EXTRAS_MACROS_INTERRUPTED_H_
#define MCR_EXTRAS_MACROS_INTERRUPTED_H_

#include "mcr/extras/model.h"
#include "mcr/extras/iinterrupt.h"

namespace mcr
{
class MCR_API MacrosInterrupted : public IInterrupt {
public:
	MacrosInterrupted();
	MacrosInterrupted(const MacrosInterrupted &);
	virtual ~MacrosInterrupted() override;
	MacrosInterrupted &operator =(const MacrosInterrupted &copytron);

	virtual void interrupt(const char *target, int type) override;

	void map(const char *name, Macro *macroPt);
	inline void map(const std::string &name, Macro *macroPt)
	{
		map(name.c_str(), macroPt);
	}
	void unmap(const char *name);
	void clear();
private:
	void *_macroMap;
	inline std::map<std::string, Macro *> &macroMapRef() const
	{
		return *static_cast<std::map<std::string, Macro *> *>(_macroMap);
	}
};
}

#endif
