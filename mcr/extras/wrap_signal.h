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
 *  \brief C++ wrapper for \ref mcr/signal/isignal.h and \ref mcr/signal/signal.h functions
 */

#ifndef MCR_EXTRAS_WRAP_SIGNAL_H_
#define MCR_EXTRAS_WRAP_SIGNAL_H_

#include "mcr/extras/model.h"

namespace mcr
{
class SignalRef;
/*! \ref mcr_ISignal reference editor
 *
 *  Will not unregister ISignal on destruction
 *  Virtual and concrete class
 */
class MCR_API ISignalRef
{
public:
	/*! \param context If null the last created context will be used.
	 *  Throws EINVAL if no context exists
	 *  \param isigPt ISignal reference to edit
	 */
	ISignalRef(Libmacro *context = nullptr, mcr_ISignal *isigPt = nullptr);
	ISignalRef(const ISignalRef &) = default;
	ISignalRef(mcr_ISignal *isigPt);
	ISignalRef(Libmacro *context, size_t id);
	ISignalRef(size_t id);
	ISignalRef(Libmacro *context, const char *name);
	ISignalRef(const char *name);
	virtual ~ISignalRef() {}
	ISignalRef &operator =(const ISignalRef &) = default;
	inline ISignalRef &operator =(mcr_ISignal *isigPt)
	{
		return build(isigPt);
	}
	inline ISignalRef &operator =(size_t id)
	{
		return build(id);
	}
	inline ISignalRef &operator =(const char *name)
	{
		return build(name);
	}
	inline ISignalRef &build(mcr_ISignal *isigPt)
	{
		setISignal(isigPt);
		return *this;
	}
	inline ISignalRef &build(size_t id)
	{
		setId(id);
		return *this;
	}
	inline ISignalRef &build(const char *name)
	{
		setName(name);
		return *this;
	}

	/*! Get ISignal reference, non-virtual */
	inline mcr_ISignal &operator *() const
	{
		return *_isigPt;
	}
	/*! Get ISignal reference, non-virtual */
	inline mcr_ISignal *operator ->() const
	{
		return _isigPt;
	}

	/*! Get ISignal reference, non-virtual */
	inline mcr_ISignal *ptr() const
	{
		return _isigPt;
	}
	inline Libmacro *context() const
	{
		return _context;
	}

	/*! \ref mcr_register this ISignal
	 *
	 *  If the ISignal id is not -1, then mcr_reg_set_name and
	 *  mcr_reg_add_name will be used instead.
	 */
	virtual void registerType(const char *name,
							  const char **addNames = nullptr, size_t addNamesCount = 0);
	/// \todo Create unregister function
	virtual void unregisterType() {}

	/*! Get ISignal reference */
	virtual inline mcr_ISignal *isignal()
	{
		return _isigPt;
	}
	virtual inline void setISignal(mcr_ISignal *isigPt)
	{
		_isigPt = isigPt;
	}

	/* Interface */
	/*! \ref mcr_Interface.id */
	virtual inline size_t id()
	{
		return mcr_ISignal_id(_isigPt);
	}
	virtual void setId(size_t val);
	/// \todo set/change id/name
	virtual const char *name();
	virtual void setName(const char *name);
	/*! \ref mcr_Interface.data_size */
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
		return _isigPt ? _isigPt->interface.init : nullptr;
	}
	virtual inline void setInit(mcr_data_fnc val)
	{
		if (_isigPt)
			_isigPt->interface.init = val;
	}

	virtual inline mcr_data_fnc deinit() const
	{
		return _isigPt ? _isigPt->interface.deinit : nullptr;
	}
	virtual inline void setDeinit(mcr_data_fnc val)
	{
		if (_isigPt)
			_isigPt->interface.deinit = val;
	}

	virtual inline mcr_compare_fnc compare() const
	{
		return _isigPt ? _isigPt->interface.compare : nullptr;
	}
	virtual inline void setCompare(mcr_compare_fnc val)
	{
		if (_isigPt)
			_isigPt->interface.compare = val;
	}

	virtual inline mcr_copy_fnc copy() const
	{
		return _isigPt ? _isigPt->interface.copy : nullptr;
	}
	virtual inline void setCopy(mcr_copy_fnc val)
	{
		if (_isigPt)
			_isigPt->interface.copy = val;
	}

	/* ISignal */
	virtual inline mcr_Dispatcher *dispatcher() const
	{
		return _isigPt ? _isigPt->dispatcher : nullptr;
	}
	virtual inline void setDispatcher(mcr_Dispatcher *val)
	{
		if (_isigPt)
			_isigPt->dispatcher = val;
	}

	virtual inline mcr_signal_fnc send() const
	{
		return _isigPt ? _isigPt->send : nullptr;
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

/*! \ref mcr_Signal reference editor
 *
 *  \ref signal.h
 *  Will not deinit Signal on destruction
 *  Virtual and concrete class
 */
class MCR_API SignalRef
{
public:
	/*! \param context If null the last created context will be used.
	 *  Throws EINVAL if no context exists
	 *  \param sigPt Signal reference to edit
	 */
	SignalRef(Libmacro *context = nullptr, mcr_Signal *sigPt = nullptr);
	SignalRef(const SignalRef &) = default;
	SignalRef(mcr_Signal *sigPt);
	virtual ~SignalRef() {}
	inline SignalRef &operator =(const SignalRef &) = default;
	/*! Change Signal to reference */
	inline SignalRef &operator =(Signal &signal)
	{
		return build(signal);
	}
	/*! Change Signal to reference */
	inline SignalRef &operator =(Signal *sigPt)
	{
		return build(sigPt);
	}
	/*! Change Signal to reference */
	inline SignalRef &operator =(mcr_Signal *sigPt)
	{
		return build(sigPt);
	}
	/*! Change Signal to reference */
	inline SignalRef &build(Signal &signal)
	{
		setSignal(signal);
		return *this;
	}
	/*! Change Signal to reference */
	inline SignalRef &build(Signal *sigPt)
	{
		setSignal(sigPt);
		return *this;
	}
	/*! Change Signal to reference */
	inline SignalRef &build(mcr_Signal *sigPt)
	{
		setSignal(sigPt);
		return *this;
	}

	/*! Change ISignal of Signal reference */
	inline SignalRef &operator =(ISignalRef &isignal)
	{
		return build(isignal);
	}
	/*! Change ISignal of Signal reference */
	inline SignalRef &operator =(mcr_ISignal *isigPt)
	{
		return build(isigPt);
	}
	/*! Change ISignal of Signal reference */
	inline SignalRef &operator =(size_t id)
	{
		return build(id);
	}
	/*! Change ISignal of Signal reference */
	inline SignalRef &operator =(const char *name)
	{
		return build(name);
	}
	inline SignalRef &build(ISignalRef &isignal)
	{
		setISignal(isignal);
		return *this;
	}
	inline SignalRef &build(mcr_ISignal *isigPt)
	{
		setISignal(isigPt);
		return *this;
	}
	inline SignalRef &build(size_t id)
	{
		setId(id);
		return *this;
	}
	inline SignalRef &build(const char *name)
	{
		setName(name);
		return *this;
	}

	/* Virtual comparison? */
#define localComparison(operation, comparisonPrefix, comparisonPostfix) \
	inline bool operator operation (const SignalRef &rhs) const \
	{ \
		if (this == &rhs) \
			return (this operation &rhs); \
		return _context == rhs._context && (*this operation rhs.ptr()); \
	} \
	inline bool operator operation (const mcr_Signal *rhs) const \
	{ return comparisonPrefix compare(rhs) comparisonPostfix; }
	/* compare == 0 */
	localComparison(==, !, )
	/* compare < 0 */
	localComparison(<,,< 0)
	/* compare > 0 */
	localComparison(>,,> 0)
#undef localComparison

	/*! \ref send, non-virtual */
	inline void operator ()()
	{
		send();
	}

	inline mcr_Signal *ptr() const
	{
		return _sigPt;
	}
	inline Libmacro *context() const
	{
		return _context;
	}

	/* ISignal manipulators */
	virtual inline mcr_ISignal *isignal() const
	{
		return _sigPt ? _sigPt->isignal : nullptr;
	}
	virtual inline ISignalRef isignalRef() const
	{
		return ISignalRef(_context, isignal());
	}
	virtual void setISignal(mcr_ISignal *isigPt);
	inline void setISignal(ISignalRef &isignal)
	{
		setISignal(isignal.isignal());
	}

	virtual inline size_t id() const
	{
		return mcr_iid(isignal());
	}
	virtual void setId(size_t val);
	virtual inline const char *name() const
	{
		return isignalRef().name();
	}
	virtual void setName(const char *val);

	/* Signal properties */
	virtual inline mcr_Signal *signal() const
	{
		return _sigPt;
	}
	virtual inline void setSignal(mcr_Signal *sigPt)
	{
		_sigPt = sigPt;
	}
	inline void setSignal(const SignalRef &copytron)
	{
		if (&copytron != this)
			setSignal(copytron.signal());
	}
	inline void setSignal(Signal &sigPt)
	{
		return setSignal(sigPt.ptr());
	}
	inline void setSignal(Signal *sigPt)
	{
		return setSignal(sigPt ? sigPt->ptr() : nullptr);
	}

	inline bool isEmpty() const
	{
		return !data<void *>();
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
		return SignalRef::data<T>(_sigPt);
	}
	template <typename T>
	inline T *data()
	{
		return SignalRef::data<T>(_sigPt);
	}
	template <typename T>
	static inline const T *data(const mcr_Signal &signal)
	{
		return static_cast<const T *>(signal.instance.data.data);
	}
	template <typename T>
	static inline T *data(mcr_Signal &signal)
	{
		return static_cast<T *>(signal.instance.data.data);
	}
	template <typename T>
	static inline const T *data(const mcr_Signal *sigPt)
	{
		return sigPt ? SignalRef::data<T>(*sigPt) : nullptr;
	}
	template <typename T>
	static inline T *data(mcr_Signal *sigPt)
	{
		return sigPt ? SignalRef::data<T>(*sigPt) : nullptr;
	}
	virtual inline SignalRef &mkdata()
	{
		/* Only create new data if it does not already exist */
		if (_sigPt && _sigPt->isignal && _sigPt->isignal->interface.data_size
			&& isEmpty()) {
			if (mcr_Instance_reset(_sigPt))
				throw mcr_err;
			if (isEmpty())
				throw mcr_err;
		}
		return *this;
	}

	virtual bool dispatch();
	virtual void send();
	virtual void copy(const mcr_Signal *copytron);
	virtual void copy(const SignalRef &copytron);
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
}

#endif
