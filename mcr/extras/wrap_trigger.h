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
 *  \brief C++ wrapper for \ref itrigger.h and \ref trigger.h functions
 */

#ifndef MCR_EXTRAS_WRAP_TRIGGER_H_
#define MCR_EXTRAS_WRAP_TRIGGER_H_

#include "mcr/extras/model.h"

namespace mcr
{
class TriggerRef;
/*! \ref mcr_ITrigger reference editor
 *
 *  \ref itrigger.h
 *  Will not unregister ITrigger on destruction
 *  Virtual and concrete class
 */
class MCR_API ITriggerRef
{
public:
	/*! \param context If null the last created context will be used.
	 *  Throws EINVAL if no context exists
	 *  \param itrigPt ITrigger reference to edit
	 */
	ITriggerRef(Libmacro *context = nullptr, mcr_ITrigger *itrigPt = nullptr);
	ITriggerRef(const ITriggerRef &) = default;
	ITriggerRef(mcr_ITrigger *itrigPt);
	ITriggerRef(Libmacro *context, size_t id);
	ITriggerRef(size_t id);
	ITriggerRef(Libmacro *context, const char *name);
	ITriggerRef(const char *name);
	virtual ~ITriggerRef() {}
	inline ITriggerRef &operator =(const ITriggerRef &copytron) = default;
	inline ITriggerRef &operator =(mcr_ITrigger *itrigPt)
	{
		return build(itrigPt);
	}
	inline ITriggerRef &operator =(size_t id)
	{
		return build(id);
	}
	inline ITriggerRef &operator =(const char *name)
	{
		return build(name);
	}
	inline ITriggerRef &build(mcr_ITrigger *itrigPt)
	{
		setITrigger(itrigPt);
		return *this;
	}
	inline ITriggerRef &build(size_t id)
	{
		setId(id);
		return *this;
	}
	inline ITriggerRef &build(const char *name)
	{
		setName(name);
		return *this;
	}

	/*! Get ITrigger reference, non-virtual */
	inline mcr_ITrigger &operator *() const
	{
		return *_itrigPt;
	}
	/*! Get ITrigger reference, non-virtual */
	inline mcr_ITrigger *operator ->() const
	{
		return _itrigPt;
	}

	inline mcr_ITrigger *ptr() const
	{
		return _itrigPt;
	}
	inline Libmacro *context() const
	{
		return _context;
	}

	/*! \ref mcr_register this ITrigger
	 *
	 *  If the ITrigger id is not -1, then mcr_reg_set_name and
	 *  mcr_reg_add_name will be used instead.
	 */
	virtual void registerType(const char *name,
							  const char **addNames = nullptr, size_t addNamesCount = 0);
	/// \todo Create unregister function
	virtual void unregisterType() {}

	/*! Get ITrigger reference */
	virtual inline mcr_ITrigger *itrigger() const
	{
		return _itrigPt;
	}
	virtual inline void setITrigger(mcr_ITrigger *itrigPt)
	{
		_itrigPt = itrigPt;
	}

	/* Interface */
	/*! \ref mcr_Interface.id */
	virtual inline size_t id()
	{
		return mcr_ITrigger_id(_itrigPt);
	}
	virtual void setId(size_t val);
	/// \todo set/change id/name
	virtual const char *name();
	virtual void setName(const char *name);
	/*! \ref mcr_Interface.data_size */
	virtual inline size_t dataSize() const
	{
		return _itrigPt ? _itrigPt->interface.data_size : 0;
	}
	virtual inline void setDataSize(size_t newSize)
	{
		if (_itrigPt)
			_itrigPt->interface.data_size = newSize;
	}

	virtual inline mcr_data_fnc init() const
	{
		return _itrigPt ? _itrigPt->interface.init : nullptr;
	}
	virtual inline void setInit(mcr_data_fnc val)
	{
		if (_itrigPt)
			_itrigPt->interface.init = val;
	}

	virtual inline mcr_data_fnc deinit() const
	{
		return _itrigPt ? _itrigPt->interface.deinit : nullptr;
	}
	virtual inline void setDeinit(mcr_data_fnc val)
	{
		if (_itrigPt)
			_itrigPt->interface.deinit = val;
	}

	virtual inline mcr_compare_fnc compare() const
	{
		return _itrigPt ? _itrigPt->interface.compare : nullptr;
	}
	virtual inline void setCompare(mcr_compare_fnc val)
	{
		if (_itrigPt)
			_itrigPt->interface.compare = val;
	}

	virtual inline mcr_copy_fnc copy() const
	{
		return _itrigPt ? _itrigPt->interface.copy : nullptr;
	}
	virtual inline void setCopy(mcr_copy_fnc val)
	{
		if (_itrigPt)
			_itrigPt->interface.copy = val;
	}

	/* ITrigger */
	virtual inline mcr_Dispatcher_receive_fnc receive() const
	{
		return _itrigPt ? _itrigPt->receive : nullptr;
	}
	virtual inline void setReceive(mcr_Dispatcher_receive_fnc fnc)
	{
		if (_itrigPt)
			_itrigPt->receive = fnc;
	}
private:
	Libmacro *_context;
	mcr_ITrigger *_itrigPt;
};

/*! \ref mcr_Trigger reference editor
 *
 *  \ref trigger.h
 *  Will not deinit Trigger on destruction
 *  Virtual and concrete class
 */
class MCR_API TriggerRef
{
public:
	/*! \param context If null the last created context will be used.
	 *  Throws EINVAL if no context exists
	 *  \param trigPt Trigger reference to edit
	 */
	TriggerRef(Libmacro *context = nullptr, mcr_Trigger *trigPt = nullptr);
	TriggerRef(const TriggerRef &) = default;
	TriggerRef(mcr_Trigger *trigPt);
	virtual ~TriggerRef() {}
	inline TriggerRef &operator =(const TriggerRef &copytron) = default;
	/*! Change Trigger to reference */
	inline TriggerRef &operator =(Trigger &trigger)
	{
		return build(trigger);
	}
	/*! Change Trigger to reference */
	inline TriggerRef &operator =(Trigger *trigPt)
	{
		return build(trigPt);
	}
	/*! Change Trigger to reference */
	inline TriggerRef &operator =(mcr_Trigger *trigPt)
	{
		return build(trigPt);
	}
	/*! Change Trigger to reference */
	inline TriggerRef &build(Trigger &trigger)
	{
		setTrigger(trigger);
		return *this;
	}
	/*! Change Trigger to reference */
	inline TriggerRef &build(Trigger *trigPt)
	{
		setTrigger(trigPt);
		return *this;
	}
	/*! Change Trigger to reference */
	inline TriggerRef &build(mcr_Trigger *trigPt)
	{
		setTrigger(trigPt);
		return *this;
	}

	/*! Change ITrigger of Trigger reference */
	inline TriggerRef &operator =(ITriggerRef &itrigger)
	{
		return build(itrigger);
	}
	/*! Change ITrigger of Trigger reference */
	inline TriggerRef &operator =(mcr_ITrigger *itrigPt)
	{
		return build(itrigPt);
	}
	/*! Change ITrigger of Trigger reference */
	inline TriggerRef &operator =(size_t id)
	{
		return build(id);
	}
	/*! Change ITrigger of Trigger reference */
	inline TriggerRef &operator =(const char *name)
	{
		return build(name);
	}
	inline TriggerRef &build(ITriggerRef &itrigger)
	{
		setITrigger(itrigger);
		return *this;
	}
	inline TriggerRef &build(mcr_ITrigger *itrigPt)
	{
		setITrigger(itrigPt);
		return *this;
	}
	inline TriggerRef &build(size_t id)
	{
		setId(id);
		return *this;
	}
	inline TriggerRef &build(const char *name)
	{
		setName(name);
		return *this;
	}

	/* Virtual comparison? */
#define localComparison(operation, comparisonPrefix, comparisonPostfix) \
	inline bool operator operation (const TriggerRef &rhs) const \
	{ \
		if (this == &rhs) \
			return (this operation &rhs); \
		return _context == rhs._context && (*this operation rhs.ptr()); \
	} \
	inline bool operator operation (const mcr_Trigger *rhs) const \
	{ return comparisonPrefix compare(rhs) comparisonPostfix; }
	/* compare == 0 */
	localComparison(==, !, )
	/* compare < 0 */
	localComparison(<,,< 0)
	/* compare > 0 */
	localComparison(>,,> 0)
#undef localComparison

	/*! Get Trigger reference, non-virtual */
	inline mcr_Trigger &operator *() const
	{
		return *_trigPt;
	}
	/*! Get Trigger reference, non-virtual */
	inline mcr_Trigger *operator ->() const
	{
		return _trigPt;
	}

	/*! Get Trigger reference, non-virtual */
	inline mcr_Trigger *ptr() const
	{
		return _trigPt;
	}
	inline Libmacro *context() const
	{
		return _context;
	}

	/* ITrigger manipulators */
	virtual inline mcr_ITrigger *itrigger() const
	{
		return _trigPt ? _trigPt->itrigger : nullptr;
	}
	virtual inline ITriggerRef itriggerRef() const
	{
		return ITriggerRef(_context, itrigger());
	}
	virtual void setITrigger(mcr_ITrigger *itrigPt);
	inline void setITrigger(ITriggerRef &itrigger)
	{
		setITrigger(itrigger.itrigger());
	}

	virtual inline size_t id() const
	{
		return mcr_iid(itrigger());
	}
	virtual void setId(size_t val);
	virtual inline const char *name() const
	{
		return itriggerRef().name();
	}
	virtual void setName(const char *val);

	/* Trigger properties */
	virtual inline mcr_Trigger *trigger() const
	{
		return _trigPt;
	}
	virtual inline void setTrigger(mcr_Trigger *trigPt)
	{
		_trigPt = trigPt;
	}
	inline void setTrigger(const TriggerRef &copytron)
	{
		if (&copytron != this)
			setTrigger(copytron.trigger());
	}
	inline void setTrigger(Trigger &trigPt)
	{
		return setTrigger(trigPt.ptr());
	}
	inline void setTrigger(Trigger *trigPt)
	{
		return setTrigger(trigPt ? trigPt->ptr() : nullptr);
	}

	inline bool isEmpty() const
	{
		return !data<void *>();
	}

	virtual inline mcr_Dispatcher_receive_fnc triggerDispatch() const
	{
		return _trigPt ? _trigPt->trigger : nullptr;
	}
	virtual inline void setTriggerDispatch(mcr_Dispatcher_receive_fnc val)
	{
		if (_trigPt)
			_trigPt->trigger = val;
	}
	virtual inline void *actor() const
	{
		return _trigPt ? _trigPt->actor : nullptr;
	}
	virtual inline void setActor(void *val)
	{
		if (_trigPt)
			_trigPt->actor = val;
	}

	/* Trigger functions */
	template <typename T>
	inline const T *data() const
	{
		return TriggerRef::data<T>(_trigPt);
	}
	template <typename T>
	inline T *data()
	{
		return TriggerRef::data<T>(_trigPt);
	}
	template <typename T>
	static inline const T *data(const mcr_Trigger &signal)
	{
		return static_cast<const T *>(signal.instance.data.data);
	}
	template <typename T>
	static inline T *data(mcr_Trigger &signal)
	{
		return static_cast<T *>(signal.instance.data.data);
	}
	template <typename T>
	static inline const T *data(const mcr_Trigger *trigPt)
	{
		return trigPt ? TriggerRef::data<T>(*trigPt) : nullptr;
	}
	template <typename T>
	static inline T *data(mcr_Trigger *trigPt)
	{
		return trigPt ? TriggerRef::data<T>(*trigPt) : nullptr;
	}
	virtual inline TriggerRef &mkdata()
	{
		if (_trigPt && _trigPt->itrigger && _trigPt->itrigger->interface.data_size
			&& isEmpty()) {
			if (mcr_Instance_reset(_trigPt))
				throw mcr_err;
			if (isEmpty())
				throw mcr_err;
		}
		return *this;
	}

	virtual void copy(const mcr_Trigger *copytron);
	virtual void copy(const TriggerRef &copytron);
	virtual inline int compare(const mcr_Trigger *rhs) const
	{
		return mcr_Trigger_compare(_trigPt, rhs);
	}
	virtual inline int compare(const TriggerRef &rhs) const
	{
		return mcr_Trigger_compare(_trigPt, rhs._trigPt);
	}
private:
	Libmacro *_context;
	mcr_Trigger *_trigPt;
};
}

#endif
