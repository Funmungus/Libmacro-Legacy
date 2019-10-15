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

#include "mcr/extras/signal_functions.h"

#include "mcr/libmacro.h"

#include <set>

namespace mcr
{
SignalFunctions::SignalFunctions(QObject *parent, Libmacro *libmacroPt)
	: QObject(parent), _libmacroPt(libmacroPt ? libmacroPt : Libmacro::instance()),
	  _serializers(new std::vector<ISerializer::get>)
{
	size_t i;
	Libmacro *ctx = context();
	mcr_ISignal *isigs[] = {
		ctx->iHidEcho(), ctx->iKey(),
		ctx->iModifier(), ctx->iMoveCursor(),
		ctx->iNoOp(), ctx->iScroll(),
		ctx->iCommand().ptr(), ctx->iStringKey().ptr(),
		ctx->iInterrupt().ptr()
	};
	ISerializer::get fncs[] = {
		[]() -> ISerializer *{ return new SerHidEcho(); },
		[]() -> ISerializer *{ return new SerKey(); },
		[]() -> ISerializer *{ return new SerModifier(); },
		[]() -> ISerializer *{ return new SerMoveCursor(); },
		[]() -> ISerializer *{ return new SerNoOp(); },
		[]() -> ISerializer *{ return new SerScroll(); },
		[]() -> ISerializer *{ return new SerCommand(); },
		[]() -> ISerializer *{ return new SerStringKey(); },
		[]() -> ISerializer *{ return new SerInterrupt(); }
	};
	i = mcr_arrlen(fncs);
	serializers().resize(i, nullptr);
	while (i--) {
		setSerializer(isigs[i]->interface.id, fncs[i]);
	}
}

SignalFunctions::~SignalFunctions()
{
	delete &serializers();
}

ISerializer *SignalFunctions::serializer(size_t id)
{
	auto &ser = serializers();
	if (id < static_cast<size_t>(ser.size())) {
		ISerializer::get fncPt = ser[id];
		if (fncPt)
			return fncPt();
	}
	return nullptr;
}

void SignalFunctions::setSerializer(size_t id, ISerializer::get serFnc)
{
	auto &ser = serializers();
	if (id == static_cast<size_t>(-1))
		return;
	ensureSerializer(id);
	/* Since we cannot notify statically, errors will crash */
	ser[id] = serFnc;
}

QVariant SignalFunctions::id(const QString &name) const
{
	mcr_ISignal *isigPt = static_cast<mcr_ISignal *>(mcr_reg_from_name(
							  mcr_ISignal_reg(
								  context()->ptr()),
							  name.toUtf8().constData()));
	return QVariant::fromValue<size_t>(mcr_iid(isigPt));
}

QString SignalFunctions::name(const QVariant &id) const
{
	return mcr_reg_name(mcr_ISignal_reg(context()->ptr()), id.value<size_t>());
}

int SignalFunctions::modifierCount() const
{
	return static_cast<int>(mcr_ModFlags_count(context()->ptr()));
}

unsigned int SignalFunctions::modifier(const QString &name) const
{
	return mcr_ModFlags_modifier(context()->ptr(), name.toUtf8().constData());
}

QString SignalFunctions::modifierName(unsigned int mod) const
{
	if (mod == static_cast<unsigned int>(-1))
		return tr("All");
	return mcr_ModFlags_name(context()->ptr(), mod);
}

QStringList SignalFunctions::modifierNames() const
{
	QStringList ret;
	mcr_context *ctx = context()->ptr();
	size_t count = mcr_ModFlags_count(ctx);
	for (unsigned int i = 0, flag = 1; i < count; i++, flag <<= 1) {
		ret << mcr_ModFlags_name(ctx, flag);
	}
	return ret;
}

int SignalFunctions::echoCount() const
{
	return static_cast<int>(mcr_HidEcho_count(context()->ptr()));
}

int SignalFunctions::echo(const QString &name) const
{
	return static_cast<int>(mcr_HidEcho_name_echo(context()->ptr(),
							name.toUtf8().constData()));
}

QString SignalFunctions::echoName(int echo) const
{
	return mcr_HidEcho_name(context()->ptr(), static_cast<size_t>(echo));
}

QStringList SignalFunctions::echoNames() const
{
	QStringList ret;
	mcr_context *ctx = context()->ptr();
	size_t count = mcr_HidEcho_count(ctx);
	for (unsigned int i = 0; i < count; i++) {
		ret << mcr_HidEcho_name(ctx, i);
	}
	return ret;
}

int SignalFunctions::keyCount() const
{
	return static_cast<int>(mcr_Key_count(context()->ptr()));
}

int SignalFunctions::key(const QString &name) const
{
	return static_cast<int>(mcr_Key_name_key(context()->ptr(),
							name.toUtf8().constData()));
}

QString SignalFunctions::keyName(int code) const
{
	return mcr_Key_name(context()->ptr(), code);
}

QStringList SignalFunctions::keyNames() const
{
	QStringList ret;
	mcr_context *ctx = context()->ptr();
	size_t count = mcr_Key_count(ctx);
	for (int i = 0; i < static_cast<int>(count); i++) {
		ret << mcr_Key_name(ctx, i);
	}
	return ret;
}

unsigned int SignalFunctions::keyMod(int code) const
{
	return mcr_Key_modifier(context()->ptr(), code);
}

SerSignal::SerSignal(size_t reserveKeys, mcr_ISignal *valueInterface) :
	_keys(new std::vector<QString>),
	_keysCanonical(new std::vector<QString>),
	_getMap(new std::map<QString, get>),
	_setMap(new std::map<QString, set>),
	_valueInterface(valueInterface)
{
	keysRef().reserve(reserveKeys);
	keysCanonicalRef().reserve(reserveKeys);
}

SerSignal::~SerSignal()
{
	delete &keysRef();
	delete &keysCanonicalRef();
	delete &getMapRef();
	delete &setMapRef();
}

size_t SerSignal::keyCount(bool canonical) const
{
	auto &keys = canonical ? keysCanonicalRef() : keysRef();
	return keys.size();
}

QString *SerSignal::keyData(bool canonical) const
{
	auto &keys = canonical ? keysCanonicalRef() : keysRef();
	if (keys.empty())
		return nullptr;
	return &keys.front();
}

QVariant SerSignal::value(const QString &name) const
{
	auto &getMap = getMapRef();
	auto iter = getMap.find(name);
	if (iter == getMap.end() || !iter->second)
		return QVariant();
	return iter->second(*this);
}

void SerSignal::setValue(const QString &name, const QVariant &val)
{
	auto &setMap = setMapRef();
	auto iter = setMap.find(name);
	if (iter == setMap.end() || !iter->second)
		return;
	dthrow(!_valueInterface, EFAULT);
	if (isignal() != _valueInterface)
		setISignal(_valueInterface);
	mkdata();
	iter->second(*this, val);
}

void SerSignal::setMaps(const QString &key, get fnGet, set fnSet)
{
	keysRef().push_back(key);
	getMapRef()[key] = fnGet;
	setMapRef()[key] = fnSet;
}

void SerSignal::setMapsCanonical(const QString &key, get fnGet, set fnSet)
{
	keysCanonicalRef().push_back(key);
	getMapRef()[key] = fnGet;
	setMapRef()[key] = fnSet;
}

SerHidEcho::SerHidEcho() : SerSignal(1)
{
	setValueInterface(context()->iHidEcho());
	setMaps("echo", &echo, &setEcho);
	setMapsCanonical("echoName", &echoName, &setEchoName);
}

QVariant SerHidEcho::echoName(const SerSignal &container)
{
	size_t e = container.isEmpty() ? 0 : container.data<mcr_HidEcho>()->echo;
	return mcr_HidEcho_name(container.context()->ptr(), e);
}

void SerHidEcho::setEchoName(SerSignal &container, const QVariant &val)
{
	QString n = val.toString();
	container.mkdata().data<mcr_HidEcho>()->echo = mcr_HidEcho_name_echo(
				container.context()->ptr(), n.toUtf8().constData());
}

SerKey::SerKey() : SerSignal(2)
{
	setValueInterface(context()->iKey());
	setMaps("key", &key, &setKey);
	setMapsCanonical("keyName", &keyName, &setKeyName);
	setMapsGeneric("applyType", &applyType, &setApplyType);
}

QVariant SerKey::keyName(const SerSignal &container)
{
	int e = container.isEmpty() ? 0 : container.data<mcr_Key>()->key;
	return mcr_Key_name(container.context()->ptr(), e);
}

void SerKey::setKeyName(SerSignal &container, const QVariant &val)
{
	QString n = val.toString();
	container.mkdata().data<mcr_Key>()->key = mcr_Key_name_key(
				container.context()->ptr(), n.toUtf8().constData());
}

SerModifier::SerModifier() : SerSignal(2)
{
	setValueInterface(context()->iModifier());
	setMapsGeneric("modifiers", &modifiers, &setModifiers);
	setMapsGeneric("applyType", &applyType, &setApplyType);
}

SerMoveCursor::SerMoveCursor() : SerSignal(4)
{
	setValueInterface(context()->iMoveCursor());
	setMapsGeneric("justify", &justify, &setJustify);
	setMapsGeneric("x", &x, &setX);
	setMapsGeneric("y", &y, &setY);
	setMapsGeneric("z", &z, &setZ);
}

SerNoOp::SerNoOp() : SerSignal(2)
{
	setValueInterface(context()->iNoOp());
	setMapsGeneric("sec", &sec, &setSec);
	setMapsGeneric("msec", &msec, &setMsec);
}

SerScroll::SerScroll() : SerSignal(3)
{
	setValueInterface(context()->iScroll());
	setMapsGeneric("x", &x, &setX);
	setMapsGeneric("y", &y, &setY);
	setMapsGeneric("z", &z, &setZ);
}

SerCommand::SerCommand() : SerSignal(3)
{
	setValueInterface(context()->iCommand().ptr());
	setMapsGeneric("cryptic", &cryptic, &setCryptic);
	setMapsGeneric("file", &file, &setFile);
	setMapsGeneric("args", &args, &setArgs);
}

QVariant SerCommand::args(const SerSignal &container)
{
	QStringList ret;
	std::vector<std::string> list;
	if (!container.isEmpty()) {
		list = container.data<Command>()->args();
		for (auto &iter: list) {
			ret << QString::fromStdString(iter);
		}
	}
	return ret;
}

void SerCommand::setArgs(SerSignal &container, const QVariant &val)
{
	std::vector<std::string> argsMem;
	QStringList list = val.toStringList();
	auto pt = container.mkdata().data<Command>();
	argsMem.reserve(static_cast<size_t>(list.size()));
	for (auto &s: list) {
		argsMem.push_back(s.toStdString());
	}
	pt->setArgs(argsMem);
}

SerInterrupt::SerInterrupt() : SerSignal(2)
{
	setValueInterface(context()->iInterrupt().ptr());
	setMapsGeneric("type", &type, &setType);
	setMapsGeneric("target", &target, &setTarget);
}

SerStringKey::SerStringKey() : SerSignal(4)
{
	setValueInterface(context()->iStringKey().ptr());
	setMapsGeneric("cryptic", &cryptic, &setCryptic);
	setMapsGeneric("sec", &sec, &setSec);
	setMapsGeneric("msec", &msec, &setMsec);
	setMapsGeneric("text", &text, &setText);
}
}
