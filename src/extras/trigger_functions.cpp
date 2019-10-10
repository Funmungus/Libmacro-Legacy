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

#include "mcr/extras/trigger_functions.h"

#include "mcr/libmacro.h"

namespace mcr
{
TriggerFunctions::TriggerFunctions(QObject *parent, Libmacro *libmacroPt)
	: QObject(parent), _libmacroPt(libmacroPt ? libmacroPt : Libmacro::instance()),
	  _serializers(new std::vector<ISerializer::get>)
{
	serializers().resize(2, nullptr);
	setSerializer(context()->iAction()->interface.id, []() ->
				  ISerializer * { return new SerAction(); });
	setSerializer(context()->iStaged()->interface.id, []() ->
				  ISerializer * { return new SerStaged(); });
}

TriggerFunctions::~TriggerFunctions()
{
	delete &serializers();
}

QVariant TriggerFunctions::id(const QString &name) const
{
	auto itrigPt = static_cast<mcr_ITrigger *>(mcr_reg_from_name(mcr_ITrigger_reg(
					   context()->ptr()),
				   name.toUtf8().constData()));
	return QVariant::fromValue<size_t>(mcr_iid(itrigPt));
}

QString TriggerFunctions::name(const QVariant &id) const
{
	return mcr_reg_name(mcr_ITrigger_reg(context()->ptr()), id.value<size_t>());
}

ISerializer *TriggerFunctions::serializer(size_t id) const
{
	auto &ser = serializers();
	if (id < static_cast<size_t>(ser.size())) {
		ISerializer::get fncPt = ser[id];
		if (fncPt)
			return fncPt();
	}
	return nullptr;
}

void TriggerFunctions::setSerializer(size_t id, ISerializer::get serFnc)
{
	auto &ser = serializers();
	if (id == static_cast<size_t>(-1))
		return;
	ensureSerializer(id);
	/* Since we cannot notify statically, errors will crash */
	ser[id] = serFnc;
}

SerTrigger::SerTrigger(size_t reserveKeys, mcr_ITrigger *valueInterface) :
	_keys(new std::vector<QString>),
	_keysCanonical(new std::vector<QString>),
	_getMap(new std::map<QString, get>),
	_setMap(new std::map<QString, set>),
	_valueInterface(valueInterface)
{
	keysRef().reserve(reserveKeys);
	keysCanonicalRef().reserve(reserveKeys);
}

SerTrigger::~SerTrigger()
{
	delete &keysRef();
	delete &keysCanonicalRef();
	delete &getMapRef();
	delete &setMapRef();
}

size_t SerTrigger::keyCount(bool canonical) const
{
	auto &keys = canonical ? keysCanonicalRef() : keysRef();
	return keys.size();
}

QString *SerTrigger::keyData(bool canonical) const
{
	auto &keys = canonical ? keysCanonicalRef() : keysRef();
	if (keys.empty())
		return nullptr;
	return &keys.front();
}

QVariant SerTrigger::value(const QString &name) const
{
	auto &getMap = getMapRef();
	auto iter = getMap.find(name);
	if (iter == getMap.end() || !iter->second)
		return QVariant();
	return iter->second(*this);
}

void SerTrigger::setValue(const QString &name, const QVariant &val)
{
	auto &setMap = setMapRef();
	auto iter = setMap.find(name);
	if (iter == setMap.end() || !iter->second)
		return;
	dthrow(!_valueInterface, EFAULT);
	if (itrigger() != _valueInterface)
		setITrigger(_valueInterface);
	mkdata();
	iter->second(*this, val);
}

void SerTrigger::setMaps(const QString &key, get fnGet, set fnSet)
{
	keysRef().push_back(key);
	getMapRef()[key] = fnGet;
	setMapRef()[key] = fnSet;
}

void SerTrigger::setMapsCanonical(const QString &key, get fnGet, set fnSet)
{
	keysCanonicalRef().push_back(key);
	getMapRef()[key] = fnGet;
	setMapRef()[key] = fnSet;
}

SerAction::SerAction() : SerTrigger(2)
{
	setValueInterface(context()->iAction());
	setMapsGeneric("modifiers", &modifiers, &setModifiers);
	setMapsGeneric("triggerFlags", &triggerFlags, &setTriggerFlags);
}

SerStaged::SerStaged() : SerTrigger(1)
{
	setValueInterface(context()->iStaged());
	setMapsGeneric("stages", &stages, &setStages);
}

QVariant SerStaged::stages(const SerTrigger &)
{
	return QVariant();
}

void SerStaged::setStages(SerTrigger &, const QVariant &)
{
}
}
