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

#ifndef MCR_EXTRAS_SIGNAL_FUNCTIONS_H_
#define MCR_EXTRAS_SIGNAL_FUNCTIONS_H_

#include <QObject>
#include <QVariantMap>

#include "mcr/extras/wrap_signal.h"
#include "mcr/extras/iserializer.h"
#include "mcr/extras/command.h"
#include "mcr/extras/interrupt.h"
#include "mcr/extras/string_key.h"

namespace mcr
{
/*! QML: Acces signal information.  C++ serialize. */
class MCR_API SignalFunctions : public QObject
{
	Q_OBJECT
public:
	explicit SignalFunctions(QObject *parent = nullptr,
							 Libmacro *libmacroPt = nullptr);
	SignalFunctions(const SignalFunctions &) = delete;
	virtual ~SignalFunctions() override;
	SignalFunctions &operator =(const SignalFunctions &) = delete;

	Libmacro *context() const
	{
		return _libmacroPt;
	}
	/* ID of ISignal name */
	Q_INVOKABLE QVariant id(const QString &name) const;
	/* Name of ISignal ID */
	Q_INVOKABLE QString name(const QVariant &id) const;

	Q_INVOKABLE int modifierCount() const;
	Q_INVOKABLE unsigned int modifier(const QString &name) const;
	Q_INVOKABLE QString modifierName(unsigned int mod) const;
	Q_INVOKABLE QStringList modifierNames() const;

	Q_INVOKABLE int echoCount() const;
	Q_INVOKABLE int echo(const QString &name) const;
	Q_INVOKABLE QString echoName(int echo) const;
	Q_INVOKABLE QStringList echoNames() const;

	Q_INVOKABLE int keyCount() const;
	Q_INVOKABLE int key(const QString &name) const;
	Q_INVOKABLE QString keyName(int code) const;
	Q_INVOKABLE QStringList keyNames() const;
	Q_INVOKABLE unsigned int keyMod(int code) const;

	ISerializer *serializer(size_t id);
	ISerializer *serializer(const QVariant &id)
	{
		return serializer(id.value<size_t>());
	}
	void setSerializer(size_t id, ISerializer::get serFnc);
private:
	Libmacro *_libmacroPt;
	/* std::vector<ISerializer::get> */
	void *_serializers;
	inline std::vector<ISerializer::get> &serializers() const
	{
		return *static_cast<std::vector<ISerializer::get> *>(_serializers);
	}

	inline void ensureSerializer(size_t id)
	{
		dthrow(id == static_cast<size_t>(-1), EINVAL);
		auto &ser = serializers();
		if (id >= ser.size())
			ser.resize(id + 1, nullptr);
	}
};

class MCR_API SerSignal : public ISerializer, public SignalRef
{
public:
	/// \todo Change to SignalRef?
	typedef QVariant (*get)(const SerSignal &container);
	typedef void (*set)(SerSignal &container, const QVariant &value);

	SerSignal(size_t reserveKeys = 0, mcr_ISignal *valueInterface = nullptr);
	SerSignal(const SerSignal &) = default;
	virtual ~SerSignal() override;
	SerSignal &operator =(const SerSignal &) = default;

	virtual void *object() const override
	{
		return signal();
	}
	virtual void setObject(void *val) override
	{
		setSignal(static_cast<mcr_Signal *>(val));
	}
	virtual size_t keyCount(bool canonical) const override;
	virtual QString *keyData(bool canonical) const override;
	virtual QVariant value(const QString &name) const override;
	virtual void setValue(const QString &name, const QVariant &val) override;

	virtual inline void setValueInterface(mcr_ISignal *isigPt)
	{
		dthrow(!isigPt, EINVAL);
		_valueInterface = isigPt;
	}
	/* Will modify keys, getMap, and setMap */
	virtual void setMaps(const QString &key, get fnGet, set fnSet);
	/* Will modify keysCanonical, getMap, and setMap */
	virtual void setMapsCanonical(const QString &key, get fnGet, set fnSet);
	/* Will add to both keys and keysCanonical, and add to both getMap and setMap. */
	virtual void setMapsGeneric(const QString &key, get fnGet, set fnSet)
	{
		setMaps(key, fnGet, fnSet);
		setMapsCanonical(key, fnGet, fnSet);
	}
private:
	void *_keys;
	void *_keysCanonical;
	void *_getMap;
	void *_setMap;
	mcr_ISignal *_valueInterface;
	inline std::vector<QString> &keysRef() const
	{
		return *static_cast<std::vector<QString> *>(_keys);
	}
	inline std::vector<QString> &keysCanonicalRef() const
	{
		return *static_cast<std::vector<QString> *>(_keysCanonical);
	}
	inline std::map<QString, get> &getMapRef() const
	{
		return *static_cast<std::map<QString, get> *>(_getMap);
	}
	inline std::map<QString, set> &setMapRef() const
	{
		return *static_cast<std::map<QString, set> *>(_setMap);
	}
};

class MCR_API SerHidEcho : public SerSignal
{
public:
	SerHidEcho();

	static QVariant echo(const SerSignal &container)
	{
		return QVariant::fromValue<size_t>(container.isEmpty() ? 0 : container.data<mcr_HidEcho>()->echo);
	}
	static void setEcho(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_HidEcho>()->echo = val.value<size_t>();
	}
	static QVariant echoName(const SerSignal &container);
	static void setEchoName(SerSignal &container, const QVariant &val);
};

class MCR_API SerKey : public SerSignal
{
public:
	SerKey();

	static QVariant key(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<mcr_Key>()->key;
	}
	static void setKey(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Key>()->key = val.toInt();
	}
	static QVariant applyType(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<mcr_Key>()->apply;
	}
	static void setApplyType(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Key>()->apply = static_cast<mcr_ApplyType>
				(val.toInt());
	}
	static QVariant keyName(const SerSignal &container);
	static void setKeyName(SerSignal &container, const QVariant &val);
};

class MCR_API SerModifier : public SerSignal
{
public:
	SerModifier();

	static QVariant modifiers(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<mcr_Modifier>()->modifiers;
	}
	static void setModifiers(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Modifier>()->modifiers = val.toUInt();
	}
	static QVariant applyType(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<mcr_Modifier>()->apply;
	}
	static void setApplyType(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Modifier>()->apply = static_cast<mcr_ApplyType>
				(val.toInt());
	}
};

class MCR_API SerMoveCursor : public SerSignal
{
public:
	SerMoveCursor();

	static QVariant justify(const SerSignal &container)
	{
		return container.isEmpty() ? false :
			   container.data<mcr_MoveCursor>()->is_justify;
	}
	static void setJustify(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_MoveCursor>()->is_justify = val.toBool();
	}
	static QVariant x(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<mcr_MoveCursor>()->pos[MCR_X];
	}
	static void setX(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_MoveCursor>()->pos[MCR_X] = val.toLongLong();
	}
	static QVariant y(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<mcr_MoveCursor>()->pos[MCR_Y];
	}
	static void setY(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_MoveCursor>()->pos[MCR_Y] = val.toLongLong();
	}
	static QVariant z(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<mcr_MoveCursor>()->pos[MCR_Z];
	}
	static void setZ(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_MoveCursor>()->pos[MCR_Z] = val.toLongLong();
	}
};

class MCR_API SerNoOp : public SerSignal
{
public:
	SerNoOp();

	static QVariant sec(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<mcr_NoOp>()->sec;
	}
	static void setSec(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_NoOp>()->sec = val.toInt();
	}
	static QVariant msec(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<mcr_NoOp>()->msec;
	}
	static void setMsec(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_NoOp>()->msec = val.toInt();
	}
};

class MCR_API SerScroll : public SerSignal
{
public:
	SerScroll();

	static QVariant x(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<mcr_Scroll>()->dm[MCR_X];
	}
	static void setX(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Scroll>()->dm[MCR_X] = val.toLongLong();
	}
	static QVariant y(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<mcr_Scroll>()->dm[MCR_Y];
	}
	static void setY(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Scroll>()->dm[MCR_Y] = val.toLongLong();
	}
	static QVariant z(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<mcr_Scroll>()->dm[MCR_Z];
	}
	static void setZ(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Scroll>()->dm[MCR_Z] = val.toLongLong();
	}
};

class MCR_API SerCommand : public SerSignal
{
public:
	SerCommand();

	static QVariant cryptic(const SerSignal &container)
	{
		return container.isEmpty() ? false : container.data<Command>()->cryptic();
	}
	static void setCryptic(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<Command>()->setCryptic(val.toBool());
	}
	static QVariant file(const SerSignal &container)
	{
		return QString::fromStdString(container.isEmpty() ? "" :
									  container.data<Command>()->file());
	}
	static void setFile(SerSignal &container, const QVariant &val)
	{
		QString f = val.toString();
		container.mkdata().data<Command>()->setFile(f.toUtf8().constData());
	}
	static QVariant args(const SerSignal &container);
	static void setArgs(SerSignal &container, const QVariant &val);
};

class MCR_API SerInterrupt : public SerSignal
{
public:
	SerInterrupt();

	static QVariant type(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<Interrupt>()->type;
	}
	static void setType(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<Interrupt>()->type = val.toInt();
	}
	static QVariant target(const SerSignal &container)
	{
		return container.isEmpty() ? "" : container.data<Interrupt>()->target();
	}
	static void setTarget(SerSignal &container, const QVariant &val)
	{
		QString t = val.toString();
		container.mkdata().data<Interrupt>()->setTarget(t.toUtf8().constData());
	}
};

class MCR_API SerStringKey : public SerSignal
{
public:
	SerStringKey();

	static QVariant cryptic(const SerSignal &container)
	{
		return container.isEmpty() ? false : container.data<StringKey>()->cryptic();
	}
	static void setCryptic(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<StringKey>()->setCryptic(val.toBool());
	}
	static QVariant sec(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<StringKey>()->interval.sec;
	}
	static void setSec(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<StringKey>()->interval.sec = val.toInt();
	}
	static QVariant msec(const SerSignal &container)
	{
		return container.isEmpty() ? 0 : container.data<StringKey>()->interval.msec;
	}
	static void setMsec(SerSignal &container, const QVariant &val)
	{
		container.mkdata().data<StringKey>()->interval.msec = val.toInt();
	}
	static QVariant text(const SerSignal &container)
	{
		return QString::fromStdString(container.isEmpty() ? "" :
									  container.data<StringKey>()->text());
	}
	static void setText(SerSignal &container, const QVariant &val)
	{
		QString f = val.toString();
		container.mkdata().data<StringKey>()->setText(f.toUtf8().constData());
	}
};
}

#endif
