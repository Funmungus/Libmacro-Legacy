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

#ifndef MCR_EXTRAS_TRIGGER_FUNCTIONS_H_
#define MCR_EXTRAS_TRIGGER_FUNCTIONS_H_

#include <QObject>
#include <QVariantMap>

#include "mcr/extras/wrap_trigger.h"
#include "mcr/extras/iserializer.h"

namespace mcr
{
/*! QML: Acces trigger information.  C++ serialize. */
class MCR_API TriggerFunctions : public QObject
{
	Q_OBJECT
public:
	explicit TriggerFunctions(QObject *parent = nullptr,
							  Libmacro *libmacroPt = nullptr);
	TriggerFunctions(const TriggerFunctions &) = delete;
	virtual ~TriggerFunctions() override;
	TriggerFunctions &operator =(const TriggerFunctions &) = delete;

	Q_INVOKABLE QVariant id(const QString &name) const;
	Q_INVOKABLE QString name(const QVariant &id) const;

	Libmacro *context() const
	{
		return _libmacroPt;
	}

	ISerializer *serializer(size_t id) const;
	ISerializer *serializer(const QVariant &id) const
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

class MCR_API SerTrigger : public ISerializer, public TriggerRef
{
public:
	/// \todo Change to TriggerRef?
	typedef QVariant (*get)(const SerTrigger &container);
	typedef void (*set)(SerTrigger &container, const QVariant &value);

	SerTrigger(size_t reserveKeys = 0, mcr_ITrigger *valueInterface = nullptr);
	SerTrigger(const SerTrigger &) = default;
	virtual ~SerTrigger() override;
	SerTrigger &operator =(const SerTrigger &) = default;

	void *object() const override
	{
		return trigger();
	}
	void setObject(void *val) override
	{
		setTrigger(static_cast<mcr_Trigger *>(val));
	}
	virtual size_t keyCount(bool canonical) const override;
	virtual QString *keyData(bool canonical) const override;
	virtual QVariant value(const QString &name) const override;
	virtual void setValue(const QString &name, const QVariant &val) override;

	virtual inline void setValueInterface(mcr_ITrigger *itrigPt)
	{
		dthrow(!itrigPt, EINVAL);
		_valueInterface = itrigPt;
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
	mcr_ITrigger *_valueInterface;
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

class MCR_API SerAction : public SerTrigger
{
public:
	SerAction();

	static QVariant modifiers(const SerTrigger &container)
	{
		return container.isEmpty() ? 0 : container.data<mcr_Action>()->modifiers;
	}
	static void setModifiers(SerTrigger &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Action>()->modifiers = val.toUInt();
	}
	static QVariant triggerFlags(const SerTrigger &container)
	{
		return container.isEmpty() ? 0 : container.data<mcr_Action>()->trigger_flags;
	}
	static void setTriggerFlags(SerTrigger &container, const QVariant &val)
	{
		container.mkdata().data<mcr_Action>()->trigger_flags = val.toUInt();
	}
};

class MCR_API SerStaged : public SerTrigger
{
public:
	SerStaged();

	static QVariant stages(const SerTrigger &container);
	static void setStages(SerTrigger &container, const QVariant &val);
};
}

#endif
