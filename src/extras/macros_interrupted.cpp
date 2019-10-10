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

#include "mcr/libmacro.h"

#include <set>

namespace mcr
{
MacrosInterrupted::MacrosInterrupted() :
	_macroMap(new std::map<std::string, Macro *>())
{
}

MacrosInterrupted::MacrosInterrupted(const MacrosInterrupted &copytron) :
	IInterrupt(), _macroMap(new std::map<std::string, Macro *>())
{
	auto &myMap = macroMapRef();
	for (auto &iter: copytron.macroMapRef()) {
		myMap[iter.first] = iter.second;
	}
}

MacrosInterrupted::~MacrosInterrupted()
{
	delete &macroMapRef();
}

MacrosInterrupted &MacrosInterrupted::operator=(const MacrosInterrupted
		&copytron)
{
	if (&copytron != this)
		macroMapRef() = copytron.macroMapRef();
	return *this;
}

void MacrosInterrupted::interrupt(const char *target, int type)
{
	auto &myMap = macroMapRef();
	std::set<void *> set;
	auto iter = myMap.end();
	if (!target || !mcr_casecmp(target, "all")) {
		for (auto &iter: myMap) {
			if (set.find(iter.second) == set.end()) {
				set.insert(iter.second);
				iter.second->setInterruptor(static_cast<Macro::Interrupt>(type));
			}
		}
	} else {
		iter = myMap.find(target);
		if (iter != myMap.end())
			iter->second->setInterruptor(static_cast<Macro::Interrupt>(type));
	}
}

void MacrosInterrupted::map(const char *name, Macro *macroPt)
{
	macroMapRef()[name] = macroPt;
}

void MacrosInterrupted::unmap(const char *name)
{
	auto &myMap = macroMapRef();
	auto element = myMap.find(name);
	if (element != myMap.end())
		myMap.erase(element);
}

void MacrosInterrupted::clear()
{
	macroMapRef().clear();
}
}
