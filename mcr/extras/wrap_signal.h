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

/*!
 * \file
 * \brief C++ wrapper for \ref isignal.h and \ref signal.h functions
 */

#ifndef __cplusplus
#pragma message "C++ support is required for extras module"
#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_WRAP_SIGNAL_H
#define MCR_EXTRAS_WRAP_SIGNAL_H

#include "mcr/extras/model.h"

namespace mcr
{
class SignalRef;
/*!
 * \brief \ref mcr_ISignal reference editor
 *
 * \ref isignal.h
 * Will not unregister ISignal on destruction
 * Virtual and concrete class
 */
class MCR_API ISignalRef
{
public:
	/*!
	 * \param context If null the last created context will be used.
	 * Throws EINVAL if no context exists
	 * \param isigPt ISignal reference to edit
	 */
	ISignalRef(Libmacro *context = NULL, mcr_ISignal *isigPt = NULL) throw(int);
	ISignalRef(mcr_ISignal *isigPt) throw(int);
	ISignalRef(Libmacro *context, size_t id) throw(int);
	ISignalRef(size_t id) throw(int);
	ISignalRef(Libmacro *context, const char *name) throw(int);
	ISignalRef(const char *name) throw(int);
	virtual ~ISignalRef() {}
	ISignalRef(const ISignalRef &) = default;
	inline ISignalRef &operator =(const ISignalRef &copytron)
	{
		if (&copytron != this)
			setISignal(copytron._isigPt);
		return *this;
	}
	inline ISignalRef &operator =(mcr_ISignal *isigPt)
	{
		setISignal(isigPt);
		return *this;
	}
	inline ISignalRef &operator =(size_t id)
	{
		setId(id);
		return *this;
	}
	inline ISignalRef &operator =(const char *name)
	{
		setName(name);
		return *this;
	}

	/*! \brief Get ISignal reference, non-virtual */
	inline mcr_ISignal &operator *()
	{
		return *_isigPt;
	}
	/*! \brief Get ISignal reference, non-virtual */
	inline const mcr_ISignal &operator *() const
	{
		return *_isigPt;
	}
	/*! \brief Get ISignal reference, non-virtual */
	inline mcr_ISignal *operator ->()
	{
		return _isigPt;
	}
	/*! \brief Get ISignal reference, non-virtual */
	inline const mcr_ISignal *operator ->() const
	{
		return _isigPt;
	}

	inline Libmacro *context() const
	{
		return _context;
	}

	/*!
	 * \brief \ref mcr_register this ISignal
	 *
	 * If the ISignal id is not -1, then mcr_reg_set_name and
	 * mcr_reg_add_name will be used instead.
	 */
	virtual void registerType(const char *name,
				  const vector<string> &addNames = vector<string>()) throw(int);
	/*! \brief TODO Create mcr_unregister function */
	virtual void unregisterType() {}

	/*! \brief Get ISignal reference */
	virtual inline mcr_ISignal *isignal()
	{
		return _isigPt;
	}
	virtual inline void setISignal(mcr_ISignal *isigPt)
	{
		_isigPt = isigPt;
	}

	/* Interface */
	/*! \brief \ref mcr_Interface.id */
	virtual inline size_t id()
	{
		return mcr_ISignal_id(_isigPt);
	}
	virtual void setId(size_t val);
	/* TODO set/change id/name */
	virtual const char *name();
	virtual void setName(const char *name);
	/*! \brief \ref mcr_Interface.data_size */
	virtual inline size_t dataSize() const
	{
		return _isigPt ? _isigPt->interface.data_size : 0;
	}
	virtual inline void setDataSize(size_t newSize)
	{
		if (_isigPt)
			_isigPt->interface.data_size = newSize;
	}

	virtual inline mcr_data_fnc init() const
	{
		return _isigPt ? _isigPt->interface.init : NULL;
	}
	virtual inline void setInit(mcr_data_fnc val)
	{
		if (_isigPt)
			_isigPt->interface.init = val;
	}

	virtual inline mcr_data_fnc deinit() const
	{
		return _isigPt ? _isigPt->interface.deinit : NULL;
	}
	virtual inline void setDeinit(mcr_data_fnc val)
	{
		if (_isigPt)
			_isigPt->interface.deinit = val;
	}

	virtual inline mcr_compare_fnc compare() const
	{
		return _isigPt ? _isigPt->interface.compare : NULL;
	}
	virtual inline void setCompare(mcr_compare_fnc val)
	{
		if (_isigPt)
			_isigPt->interface.compare = val;
	}

	virtual inline mcr_copy_fnc copy() const
	{
		return _isigPt ? _isigPt->interface.copy : NULL;
	}
	virtual inline void setCopy(mcr_copy_fnc val)
	{
		if (_isigPt)
			_isigPt->interface.copy = val;
	}

	/* ISignal */
	virtual inline mcr_Dispatcher *dispatcher() const
	{
		return _isigPt ? _isigPt->dispatcher : NULL;
	}
	virtual inline void setDispatcher(mcr_Dispatcher *val)
	{
		if (_isigPt)
			_isigPt->dispatcher = val;
	}

	virtual inline mcr_signal_fnc send() const
	{
		return _isigPt ? _isigPt->send : NULL;
	}
	virtual inline void setSend(mcr_signal_fnc val)
	{
		if (_isigPt)
			_isigPt->send = val;
	}
private:
	Libmacro *_context;
	mcr_ISignal *_isigPt;
};

/*!
 * \brief \ref mcr_Signal reference editor
 *
 * \ref signal.h
 * Will not deinit Signal on destruction
 * Virtual and concrete class
 */
class MCR_API SignalRef
{
public:
	/*!
	 * \param context If null the last created context will be used.
	 * Throws EINVAL if no context exists
	 * \param sigPt Signal reference to edit
	 */
	SignalRef(Libmacro *context = NULL, mcr_Signal *sigPt = NULL) throw(int);
	SignalRef(mcr_Signal *sigPt) throw(int);
	virtual ~SignalRef() {}
	SignalRef(const SignalRef &) = default;
	inline SignalRef &operator =(const SignalRef &copytron)
	{
		if (&copytron != this)
			setSignal(copytron.signal());
		return *this;
	}
	/*! \brief Change ISignal of Signal reference */
	inline SignalRef &operator =(ISignalRef &isignal) throw(int)
	{
		setISignal(isignal.isignal());
		return *this;
	}
	/*! \brief Change Signal to reference */
	inline SignalRef &operator =(mcr_Signal *sigPt)
	{
		setSignal(sigPt);
		return *this;
	}
	/*! \brief Change ISignal of Signal reference */
	inline SignalRef &operator =(mcr_ISignal *isigPt) throw(int)
	{
		setISignal(isigPt);
		return *this;
	}
	/*! \brief Change ISignal of Signal reference */
	inline SignalRef &operator =(size_t id) throw(int)
	{
		setId(id);
		return *this;
	}
	/*! \brief Change ISignal of Signal reference */
	inline SignalRef &operator =(const char *name) throw(int)
	{
		setName(name);
		return *this;
	}

	/* Virtual comparison? */
#define localComparison(operation, comparisonPrefix, comparisonPostfix) \
	inline bool operator operation (const SignalRef &rhs) const \
	{ \
		if (&_context != &rhs._context) \
			return false; \
		return comparisonPrefix compare(rhs) comparisonPostfix; \
	} \
	inline bool operator operation (const mcr_Signal *rhs) const \
	{ return comparisonPrefix compare(rhs) comparisonPostfix; }
	/* compare == 0 */
	localComparison(==, !, )
	/* compare != 0 */
	localComparison(!=,, )
	/* compare < 0 */
	localComparison(<,,< 0)
	/* compare <= 0 */
	localComparison(<=,,<= 0)
	/* compare > 0 */
	localComparison(>,,> 0)
	/* compare >= 0 */
	localComparison(>=,,>= 0)
#undef localComparison

	/*! \brief Get Signal reference, non-virtual */
	inline mcr_Signal &operator *()
	{
		return *_sigPt;
	}
	/*! \brief Get Signal reference, non-virtual */
	inline const mcr_Signal &operator *() const
	{
		return *_sigPt;
	}
	/*! \brief Get Signal reference, non-virtual */
	inline mcr_Signal *operator ->()
	{
		return _sigPt;
	}
	/*! \brief Get Signal reference, non-virtual */
	inline const mcr_Signal *operator ->() const
	{
		return _sigPt;
	}

	/*! \brief \ref send, non-virtual */
	inline void operator ()() throw(int)
	{
		send();
	}

	inline Libmacro *context() const
	{
		return _context;
	}

	/* ISignal manipulators */
	virtual inline mcr_ISignal *isignal() const
	{
		return _sigPt ? _sigPt->isignal : NULL;
	}
	virtual inline ISignalRef isignalRef() const
	{
		return ISignalRef(_context, isignal());
	}
	virtual void setISignal(mcr_ISignal *isigPt) throw(int);

	virtual inline size_t id() const
	{
		return mcr_iid(isignal());
	}
	virtual void setId(size_t val) throw(int);
	virtual inline const char *name() const
	{
		return isignalRef().name();
	}
	virtual void setName(const char *val) throw(int);

	/* Signal properties */
	virtual inline mcr_Signal *signal() const
	{
		return _sigPt;
	}
	virtual inline void setSignal(mcr_Signal *sigPt)
	{
		_sigPt = sigPt;
	}

	virtual inline bool isDispatch() const
	{
		return _sigPt ? _sigPt->is_dispatch : false;
	}
	virtual inline void setDispatch(bool val)
	{
		if (_sigPt)
			_sigPt->is_dispatch = val;
	}

	/* Signal functions */
	template <typename T>
	inline const T *data() const
	{
		return (const T *)mcr_Instance_data(_sigPt);
	}
	template <typename T>
	inline T *data()
	{
		return (T *)mcr_Instance_data(_sigPt);
	}
	virtual inline void mkdata() throw(int)
	{
		int err;
		/* Only create new data if it does not already exist */
		if (_sigPt && _sigPt->isignal && _sigPt->isignal->interface.data_size
		    && !data<void *>()) {
			if ((err = mcr_Instance_reset(_sigPt)))
				throw err;
			if (!data<void *>())
				throw mcr_error();
		}
	}

	virtual bool dispatch();
	virtual void send() throw(int);
	virtual void copy(const mcr_Signal *copytron) throw(int);
	virtual void copy(const SignalRef &copytron) throw(int);
	virtual inline int compare(const mcr_Signal *rhs) const
	{
		return mcr_Signal_compare(_sigPt, rhs);
	}
	virtual inline int compare(const SignalRef &rhs) const
	{
		return mcr_Signal_compare(_sigPt, rhs._sigPt);
	}
private:
	Libmacro *_context;
	mcr_Signal *_sigPt;
};

/*!
 * \brief Reference to Signal of specific type.
 *
 * This class is intended to always have the same ISignal. Setting
 * a signal reference of a different type should reset it to the correct
 * type.
 */
class MCR_API SignalManager : public SignalRef
{
public:
	virtual inline mcr_ISignal *isignal() const override
	{
		return _isigPt;
	}
	virtual inline void setISignal(mcr_ISignal *isigPt) throw(int) override
	{
		if (isigPt != _isigPt)
			throw EPERM;
		SignalRef::setISignal(_isigPt);
	}
	virtual inline void setId(size_t val) throw(int) override
	{
		if (val != id())
			throw EPERM;
		SignalRef::setISignal(_isigPt);
	}
	virtual inline void setName(const char *val) throw(int) override
	{
		UNUSED(val);
		SignalRef::setISignal(_isigPt);
	}
	virtual inline void setSignal(mcr_Signal *sigPt) override
	{
		if (sigPt != signal()) {
			SignalRef::setSignal(sigPt);
			SignalRef::setISignal(_isigPt);
		}
	}
	virtual void copy(const mcr_Signal *copytron) throw(int) override;
	virtual void copy(const SignalRef &copytron) throw(int) override;
protected:
	/*!
	 * \brief Please call \ref init after construction
	 * \param context If null the last created context will be used.
	 * Throws EINVAL if no context exists
	 * \param sigPt Signal reference to manage
	 */
	SignalManager(Libmacro *context = NULL, mcr_Signal *sigPt = NULL,
		      mcr_ISignal *isigPt = NULL) throw(int)
		: SignalRef(context, sigPt), _isigPt(isigPt)
	{
		if (_isigPt)
			SignalRef::setISignal(_isigPt);
	}
//	/*!
//	 * \brief Please call \ref init after construction
//	 * \param sigPt Signal reference to manage
//	 */
//	SignalManager(mcr_Signal *sigPt) throw(int)
//		: SignalRef(sigPt), _isigPt(sigPt ? sigPt->isignal : NULL)
//	{
//		if (_isigPt)
//			SignalRef::setISignal(_isigPt);
//	}
	/*! \param isigPt ISignal used to manage Signal reference */
	inline void init(mcr_ISignal *isigPt) throw(int)
	{
		if (!isigPt)
			throw EFAULT;
		_isigPt = isigPt;
		SignalRef::setISignal(_isigPt);
	}
private:
	mcr_ISignal *_isigPt;
};

class MCR_API HidEchoRef : public SignalManager
{
public:
	HidEchoRef(Libmacro *context = NULL, mcr_Signal *sigPt = NULL) throw(int);

	size_t count() const;
	const char *name(size_t id) const;
	size_t echo(const char *name) const;

	inline size_t echo() const
	{
		if (data<mcr_HidEcho>())
			return data<mcr_HidEcho>()->echo;
		return MCR_ECHO_ANY;
	}
	inline void setEcho(size_t val) throw(int)
	{
		mkdata();
		data<mcr_HidEcho>()->echo = val;
	}

	inline const char *echoName() const
	{
		return name(echo());
	}
	inline void setEchoName(const char *name) throw(int)
	{
		setEcho(echo(name));
	}
};
typedef HidEchoRef EchoRef;

class MCR_API KeyRef : public SignalManager
{
public:
	KeyRef(Libmacro *context = NULL, mcr_Signal *sigPt = NULL) throw(int);

	size_t count() const;
	const char *name(int keyCode) const;
	int key(const char *name) const;

	inline int key() const
	{
		if (data<mcr_Key>())
			return data<mcr_Key>()->key;
		return MCR_KEY_ANY;
	}
	inline void setKey(int val) throw(int)
	{
		mkdata();
		data<mcr_Key>()->key = val;
	}

	inline int scan() const
	{
		if (data<mcr_Key>())
			return data<mcr_Key>()->scan;
		return MCR_KEY_ANY;
	}
	inline void setScan(int val) throw(int)
	{
		mkdata();
		data<mcr_Key>()->scan = val;
	}

	inline mcr_KeyUpType upType() const
	{
		if (data<mcr_Key>())
			return data<mcr_Key>()->up_type;
		return (mcr_KeyUpType)0;
	}
	inline void setUpType(mcr_KeyUpType val) throw(int)
	{
		mkdata();
		data<mcr_Key>()->up_type = val;
	}

	inline const char *keyName() const
	{
		return name(key());
	}
	inline void setKeyName(const char *name) throw(int)
	{
		setKey(key(name));
	}
};

class MCR_API ModsRef : public SignalManager
{
public:
	ModsRef(Libmacro *context = NULL, mcr_Signal *sigPt = NULL) throw(int);

	size_t count() const;
	const char *name(int modifier) const;
	int modifier(const char *name) const;

	inline mcr_KeyUpType upType() const
	{
		if (data<mcr_Mods>())
			return data<mcr_Mods>()->up_type;
		return (mcr_KeyUpType)0;
	}
	inline void setUpType(mcr_KeyUpType val) throw(int)
	{
		mkdata();
		data<mcr_Mods>()->up_type = val;
	}

	inline unsigned int modifiers() const
	{
		if (data<mcr_Mods>())
			return data<mcr_Mods>()->modifiers;
		return 0;
	}
	inline void setModifiers(unsigned int val) throw(int)
	{
		mkdata();
		data<mcr_Mods>()->modifiers = val;
	}
	inline void addModifiers(unsigned int val) throw(int)
	{
		mkdata();
		data<mcr_Mods>()->modifiers |= val;
	}
	inline void removeModifiers(unsigned int val) throw(int)
	{
		mkdata();
		data<mcr_Mods>()->modifiers &= (~val);
	}

	inline const char *modifierName() const
	{
		return name(modifiers());
	}
	inline void setModifierName(const char *name) throw(int)
	{
		setModifiers(modifier(name));
	}
	inline void addModifierName(const char *name) throw(int)
	{
		addModifiers(modifier(name));
	}
	inline void removeModifierName(const char *name) throw(int)
	{
		removeModifiers(modifier(name));
	}
};

class MCR_API MoveCursorRef : public SignalManager
{
public:
	MoveCursorRef(Libmacro *context = NULL, mcr_Signal *sigPt = NULL) throw(int);

	inline bool isJustify() const
	{
		if (data<mcr_MoveCursor>())
			return data<mcr_MoveCursor>()->is_justify;
		return false;
	}
	inline void setJustify(bool val) throw(int)
	{
		mkdata();
		data<mcr_MoveCursor>()->is_justify = val;
	}

	inline long long x() const
	{
		if (data<mcr_MoveCursor>())
			return data<mcr_MoveCursor>()->pos[MCR_X];
		return 0;
	}
	inline void setX(long long val) throw(int)
	{
		mkdata();
		data<mcr_MoveCursor>()->pos[MCR_X] = val;
	}

	inline long long y() const
	{
		if (data<mcr_MoveCursor>())
			return data<mcr_MoveCursor>()->pos[MCR_Y];
		return 0;
	}
	inline void setY(long long val) throw(int)
	{
		mkdata();
		data<mcr_MoveCursor>()->pos[MCR_Y] = val;
	}

	inline long long z() const
	{
		if (data<mcr_MoveCursor>())
			return data<mcr_MoveCursor>()->pos[MCR_Z];
		return 0;
	}
	inline void setZ(long long val) throw(int)
	{
		mkdata();
		data<mcr_MoveCursor>()->pos[MCR_Z] = val;
	}
};
typedef MoveCursorRef MCRef;

class MCR_API NoOpRef : public SignalManager
{
public:
	NoOpRef(Libmacro *context = NULL, mcr_Signal *sigPt = NULL) throw(int);

	inline int sec() const
	{
		if (data<mcr_NoOp>())
			return data<mcr_NoOp>()->sec;
		return 0;
	}
	inline void setSec(int val) throw(int)
	{
		mkdata();
		data<mcr_NoOp>()->sec = val;
	}

	inline int msec() const
	{
		if (data<mcr_NoOp>())
			return data<mcr_NoOp>()->msec;
		return 0;
	}
	inline void setMsec(int val) throw(int)
	{
		mkdata();
		data<mcr_NoOp>()->msec = val;
	}
};

class MCR_API ScrollRef : public SignalManager
{
public:
	ScrollRef(Libmacro *context = NULL, mcr_Signal *sigPt = NULL) throw(int);

	inline long long x() const
	{
		if (data<mcr_Scroll>())
			return data<mcr_Scroll>()->dm[MCR_X];
		return 0;
	}
	inline void setX(long long val) throw(int)
	{
		mkdata();
		data<mcr_Scroll>()->dm[MCR_X] = val;
	}

	inline long long y() const
	{
		if (data<mcr_Scroll>())
			return data<mcr_Scroll>()->dm[MCR_Y];
		return 0;
	}
	inline void setY(long long val) throw(int)
	{
		mkdata();
		data<mcr_Scroll>()->dm[MCR_Y] = val;
	}

	inline long long z() const
	{
		if (data<mcr_Scroll>())
			return data<mcr_Scroll>()->dm[MCR_Z];
		return 0;
	}
	inline void setZ(long long val) throw(int)
	{
		mkdata();
		data<mcr_Scroll>()->dm[MCR_Z] = val;
	}
};
}

#endif
