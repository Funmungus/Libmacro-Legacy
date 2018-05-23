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

/*! \file
 * \brief C++ wrapper for \ref itrigger.h and \ref trigger.h functions
 */

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_WRAP_TRIGGER_H_
#define MCR_EXTRAS_WRAP_TRIGGER_H_

#include "mcr/extras/model.h"

namespace mcr
{
class TriggerRef;
/*! \ref mcr_ITrigger reference editor
 *
 * \ref itrigger.h
 * Will not unregister ITrigger on destruction
 * Virtual and concrete class
 */
class MCR_EXTRAS_API ITriggerRef
{
public:
	/*! \param context If null the last created context will be used.
	 * Throws EINVAL if no context exists
	 * \param itrigPt ITrigger reference to edit
	 */
	ITriggerRef(Libmacro *context = NULL, mcr_ITrigger *itrigPt = NULL);
	ITriggerRef(mcr_ITrigger *itrigPt);
	ITriggerRef(Libmacro *context, size_t id);
	ITriggerRef(size_t id);
	ITriggerRef(Libmacro *context, const char *name);
	ITriggerRef(const char *name);
	virtual ~ITriggerRef() {}
	ITriggerRef(const ITriggerRef &) = default;
	inline ITriggerRef &operator =(const ITriggerRef &copytron)
	{
		if (&copytron != this)
			setITrigger(copytron.itrigger());
		return *this;
	}
	inline ITriggerRef &operator =(mcr_ITrigger *itrigPt)
	{
		setITrigger(itrigPt);
		return *this;
	}
	inline ITriggerRef &operator =(size_t id)
	{
		setId(id);
		return *this;
	}
	inline ITriggerRef &operator =(const char *name)
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
	 * If the ITrigger id is not -1, then mcr_reg_set_name and
	 * mcr_reg_add_name will be used instead.
	 */
	virtual void registerType(const char *name,
							  const char **addNames = NULL, size_t addNamesCount = 0);
	/*! TODO Create mcr_unregister function */
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
	/* TODO set/change id/name */
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
		return _itrigPt ? _itrigPt->interface.init : NULL;
	}
	virtual inline void setInit(mcr_data_fnc val)
	{
		if (_itrigPt)
			_itrigPt->interface.init = val;
	}

	virtual inline mcr_data_fnc deinit() const
	{
		return _itrigPt ? _itrigPt->interface.deinit : NULL;
	}
	virtual inline void setDeinit(mcr_data_fnc val)
	{
		if (_itrigPt)
			_itrigPt->interface.deinit = val;
	}

	virtual inline mcr_compare_fnc compare() const
	{
		return _itrigPt ? _itrigPt->interface.compare : NULL;
	}
	virtual inline void setCompare(mcr_compare_fnc val)
	{
		if (_itrigPt)
			_itrigPt->interface.compare = val;
	}

	virtual inline mcr_copy_fnc copy() const
	{
		return _itrigPt ? _itrigPt->interface.copy : NULL;
	}
	virtual inline void setCopy(mcr_copy_fnc val)
	{
		if (_itrigPt)
			_itrigPt->interface.copy = val;
	}

	/* ITrigger */
	virtual inline mcr_Dispatcher_receive_fnc receive() const
	{
		return _itrigPt ? _itrigPt->receive : NULL;
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
 * \ref trigger.h
 * Will not deinit Trigger on destruction
 * Virtual and concrete class
 */
class MCR_EXTRAS_API TriggerRef
{
public:
	/*! \param context If null the last created context will be used.
	 * Throws EINVAL if no context exists
	 * \param trigPt Trigger reference to edit
	 */
	TriggerRef(Libmacro *context = NULL, mcr_Trigger *trigPt = NULL);
	TriggerRef(mcr_Trigger *trigPt);
	virtual ~TriggerRef() {}
	TriggerRef(const TriggerRef &) = default;
	inline TriggerRef &operator =(const TriggerRef &copytron)
	{
		if (&copytron != this)
			setTriggerRef(copytron.triggerRef());
		return *this;
	}
	/*! Change ITrigger of Trigger reference */
	inline TriggerRef &operator =(ITriggerRef &itrigger)
	{
		setITrigger(itrigger.itrigger());
		return *this;
	}
	/*! Change Trigger to reference */
	inline TriggerRef &operator =(mcr_Trigger *trigPt)
	{
		setTriggerRef(trigPt);
		return *this;
	}
	/*! Change ITrigger of Trigger reference */
	inline TriggerRef &operator =(mcr_ITrigger *itrigPt)
	{
		setITrigger(itrigPt);
		return *this;
	}
	/*! Change ITrigger of Trigger reference */
	inline TriggerRef &operator =(size_t id)
	{
		setId(id);
		return *this;
	}
	/*! Change ITrigger of Trigger reference */
	inline TriggerRef &operator =(const char *name)
	{
		setName(name);
		return *this;
	}

	/* Virtual comparison? */
#define localComparison(operation, comparisonPrefix, comparisonPostfix) \
	inline bool operator operation (const TriggerRef &rhs) const \
	{ \
		if (&_context != &rhs._context) \
			return false; \
		return comparisonPrefix compare(rhs) comparisonPostfix; \
	} \
	inline bool operator operation (const mcr_Trigger *rhs) const \
	{ return comparisonPrefix compare(rhs) comparisonPostfix; }
	/* compare == 0 */
	localComparison(==, !, )
//	/* compare != 0 */
//	localComparison(!=, !!, )
	/* compare < 0 */
	localComparison(<,,< 0)
//	/* compare <= 0 */
//	localComparison(<=,,<= 0)
	/* compare > 0 */
	localComparison(>,,> 0)
//	/* compare >= 0 */
//	localComparison(>=,,>= 0)
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
		return _trigPt ? _trigPt->itrigger : NULL;
	}
	virtual inline ITriggerRef itriggerRef() const
	{
		return ITriggerRef(_context, itrigger());
	}
	virtual void setITrigger(mcr_ITrigger *itrigPt);

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
	virtual inline mcr_Trigger *triggerRef() const
	{
		return _trigPt;
	}
	virtual inline void setTriggerRef(mcr_Trigger *trigPt)
	{
		_trigPt = trigPt;
	}

	virtual inline mcr_Dispatcher_receive_fnc trigger() const
	{
		return _trigPt ? _trigPt->trigger : NULL;
	}
	virtual inline void setTrigger(mcr_Dispatcher_receive_fnc val)
	{
		if (_trigPt)
			_trigPt->trigger = val;
	}
	virtual inline void *actor() const
	{
		return _trigPt ? _trigPt->actor : NULL;
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
		return (const T *)mcr_Instance_data(_trigPt);
	}
	template <typename T>
	inline T *data()
	{
		return (T *)mcr_Instance_data(_trigPt);
	}
	virtual inline void mkdata()
	{
		int err;
		if (_trigPt && _trigPt->itrigger && _trigPt->itrigger->interface.data_size
			&& !data<void *>()) {
			if ((err = mcr_Instance_reset(_trigPt)))
				throw err;
			if (!data<void *>())
				throw mcr_err;
		}
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

/*! Reference to Trigger of specific type.
 *
 * This class is intended to always have the same ITrigger. Setting
 * a trigger reference of a different type should reset it to the correct
 * type.
 */
class MCR_EXTRAS_API TriggerManager : public TriggerRef
{
public:
	virtual inline mcr_ITrigger *itrigger() const override
	{
		return _itrigPt;
	}
	virtual inline void setITrigger(mcr_ITrigger *itrigPt) override
	{
		if (itrigPt != _itrigPt)
			throw EPERM;
		TriggerRef::setITrigger(_itrigPt);
	}
	virtual inline void setId(size_t val) override
	{
		if (val != id())
			throw EPERM;
		TriggerRef::setITrigger(_itrigPt);
	}
	virtual inline void setName(const char *val) override
	{
		UNUSED(val);
		TriggerRef::setITrigger(_itrigPt);
	}
	virtual inline void setTriggerRef(mcr_Trigger *trigPt) override
	{
		if (trigPt != triggerRef()) {
			TriggerRef::setTriggerRef(trigPt);
			TriggerRef::setITrigger(_itrigPt);
		}
	}
	virtual void copy(const mcr_Trigger *copytron) override;
	virtual void copy(const TriggerRef &copytron) override;
protected:
	/*! Please call \ref init after construction
	 * \param context If null the last created context will be used.
	 * Throws EINVAL if no context exists
	 * \param trigPt Trigger reference to manage
	 */
	TriggerManager(Libmacro *context = NULL, mcr_Trigger *trigPt = NULL,
				   mcr_ITrigger *itrigPt = NULL)
		: TriggerRef(context, trigPt), _itrigPt(itrigPt)
	{
		if (_itrigPt)
			TriggerRef::setITrigger(_itrigPt);
	}
//	/*!//	 * \brief Please call \ref init after construction
//	 * \param trigPt Trigger reference to manage
//	 */
//	TriggerManager(mcr_Trigger *trigPt)
//		: TriggerRef(trigPt), _itrigPt(trigPt ? trigPt->itrigger : NULL)
//	{
//		if (_itrigPt)
//			TriggerRef::setITrigger(_itrigPt);
//	}
	/*! \param itrigPt ITrigger used to manage Trigger reference */
	inline void init(mcr_ITrigger *itrigPt)
	{
		if (!itrigPt)
			throw EFAULT;
		_itrigPt = itrigPt;
		TriggerRef::setITrigger(_itrigPt);
	}
private:
	mcr_ITrigger *_itrigPt;
};

class MCR_EXTRAS_API ActionRef : public TriggerManager
{
public:
	ActionRef(Libmacro *context = NULL, mcr_Trigger *trigPt = NULL);

	inline unsigned int modifiers() const
	{
		if (data<mcr_Action>())
			return data<mcr_Action>()->modifiers;
		return 0;
	}
	inline void setModifiers(unsigned int val)
	{
		mkdata();
		data<mcr_Action>()->modifiers = val;
	}

	inline unsigned int triggerFlags() const
	{
		if (data<mcr_Action>())
			return data<mcr_Action>()->trigger_flags;
		return 0;
	}
	inline void setTriggerFlags(int val)
	{
		mkdata();
		data<mcr_Action>()->trigger_flags = val;
	}
};

class MCR_EXTRAS_API StagedRef : public TriggerManager
{
public:
	StagedRef(Libmacro *context = NULL, mcr_Trigger *trigPt = NULL);

	inline bool block () const
	{
		if (data<mcr_Staged>())
			return data<mcr_Staged>()->block;
		return 0;
	}
	inline void setBlock(bool val)
	{
		mkdata();
		data<mcr_Staged>()->block = val;
	}

	inline mcr_Array *stagesRef()
	{
		if (data<mcr_Staged>())
			return &data<mcr_Staged>()->stages;
		return NULL;
	}
	inline mcr_Stage *stagesArr()
	{
		if (data<mcr_Staged>())
			return (mcr_Stage *)data<mcr_Staged>()->stages.array;
		return NULL;
	}
	inline size_t stageCount() const
	{
		if (data<mcr_Staged>())
			return data<mcr_Staged>()->stages.used;
		return 0;
	}
	vector<Stage> stages() const;
	void setStages(const vector<Stage> &val);

	inline mcr_BlockStyle style () const
	{
		if (data<mcr_Staged>())
			return data<mcr_Staged>()->style;
		return (mcr_BlockStyle)0;
	}
	inline void setStyle(mcr_BlockStyle val)
	{
		mkdata();
		data<mcr_Staged>()->style = val;
	}
};
}

#endif
