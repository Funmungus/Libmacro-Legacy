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

namespace mcr
{
IInterrupt *Interrupt::defaultInterrupt = nullptr;

//QString InterruptRef::typeName(int val)
//{
//	switch (val) {
//	case MCR_CONTINUE:
//		return tr("Enable/Continue");
//	case MCR_PAUSE:
//		return tr("Pause");
//	case MCR_INTERRUPT:
//		return tr("Interrupt One");
//	case MCR_INTERRUPT_ALL:
//		return tr("Interrupt All");
//	case MCR_DISABLE:
//		return tr("Disable");
//	}
//	return tr("Unknown");
//}

Interrupt::Interrupt(IInterrupt *iintercept,
					 const char *target,
					 int type) : iInterrupt(iintercept), type(type),
	_target(new std::string)
{
	if (target)
		targetRef() = target;
}

Interrupt::Interrupt(const Interrupt &copytron) : ISignalData(), iInterrupt(
		copytron.iInterrupt), type(copytron.type),
	_target(new std::string(copytron.targetRef()))
{
}

Interrupt &Interrupt::operator =(const Interrupt &copytron)
{
	if (&copytron != this) {
		iInterrupt = copytron.iInterrupt;
		type = copytron.type;
		targetRef() = copytron.targetRef();
	}
	return *this;
}

Interrupt::~Interrupt()
{
	delete &targetRef();
}

int Interrupt::compare(const IData &rhs) const
{
	const Interrupt &rig = dynamic_cast<const Interrupt &>(rhs);
	if (type != rig.type)
		return type < rig.type ? -1 : 1;
	return targetRef().compare(rig.targetRef());
}

void Interrupt::copy(const IData *copytron)
{
	const Interrupt *rPt = dynamic_cast<const Interrupt *>(copytron);
	if (rPt) {
		if (rPt != this)
			*this = *rPt;
	} else {
		*this = Interrupt();
	}
}

void Interrupt::send()
{
	if (iInterrupt) {
		iInterrupt->interrupt(target(), type);
	} else if(defaultInterrupt) {
		defaultInterrupt->interrupt(target(), type);
	}
}

const char *Interrupt::target() const
{
	return targetRef().c_str();
}

void Interrupt::setTarget(const char *val)
{
	targetRef() = val;
}
}
