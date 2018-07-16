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
 *  \brief C++ wrappers for Libmacro structures
 *
 *  All POD's not intended to subclass.  Implement different types with
 *  reference classes and managers
 */

#ifndef __cplusplus
	#pragma message "C++ support is required for extras module"
	#include "mcr/err.h"
#endif

#ifndef MCR_EXTRAS_MODEL_H_
#define MCR_EXTRAS_MODEL_H_

#include "mcr/extras/def.h"

namespace mcr
{
/* Typedefs for things without deinit requirements */
/*! Construct with \ref mcr_Interface_new */
typedef mcr_Interface Interface;
/*! Construct with \ref mcr_Instance_new */
typedef mcr_Instance Instance;
/*! Construct with \ref mcr_Data_new */
typedef mcr_Data Data;
/*! Construct with \ref mcr_ISignal_new */
typedef mcr_ISignal CISignal;
/*! Construct with \ref mcr_ModFlags_new */
typedef mcr_ModFlags ModFlags;
/*! Construct with \ref mcr_Dispatcher_new */
typedef mcr_Dispatcher Dispatcher;
/*! Construct with \ref mcr_DispatchPair_new */
typedef mcr_DispatchPair DispatchPair;
/*! Construct with \ref mcr_ITrigger_new */
typedef mcr_ITrigger ITrigger;

/*! \ref ISignal with \ref Libmacro reference
 *
 *  \ref mcr_CtxISignal
 *  Not typedef because this is a C++ Libmacro context
 */
struct MCR_EXTRAS_API CtxISignal {
	mcr_ISignal isignal;

	/*! \param context If null the last created context will be used.
	 *  Throws EINVAL if no context exists
	 *  \param send \ref mcr_ISignal.send
	 *  \param dispatcher \ref opt mcr_ISignal.dispatcher
	 *  \param interface \ref opt mcr_ISignal.interface
	 *  exist.
	 */
	CtxISignal(Libmacro *context, mcr_signal_fnc send,
			   mcr_Dispatcher *dispatcher = NULL,
			   const Interface &interface = mcr_Interface_new(0, NULL, NULL, NULL,
											NULL));
	CtxISignal(mcr_signal_fnc send);
	virtual ~CtxISignal() {}
	CtxISignal(const CtxISignal &) = default;
	CtxISignal &operator =(const CtxISignal &) = default;

	inline const mcr_ISignal *ptr() const
	{
		return &isignal;
	}
	inline mcr_ISignal *ptr()
	{
		return &isignal;
	}

	inline Libmacro *context() const
	{
		return _context;
	}
private:
	Libmacro *_context;
};

/*! \ref Dispatcher with \ref Libmacro reference
 *
 *  \ref mcr_CtxDispatcher
 *  Not typedef because this is a C++ Libmacro context
 */
struct MCR_EXTRAS_API CtxDispatcher {
	mcr_Dispatcher dispatcher;

	/*! \param context If null the last created context will be used.
	 *  Throws EINVAL if no context exists
	 *  \param add \ref mcr_Dispatcher.add
	 *  \param clear \ref opt mcr_Dispatcher.clear
	 *  \param dispatch \ref opt mcr_Dispatcher.dispatch
	 *  \param modifier \ref opt mcr_Dispatcher.modifier
	 *  \param remove \ref opt mcr_Dispatcher.remove
	 *  \param trim \ref opt mcr_Dispatcher.trim
	 *  exist.
	 */
	CtxDispatcher(Libmacro *context = NULL, mcr_Dispatcher_add_fnc add = NULL,
				  mcr_Dispatcher_fnc clear = NULL,
				  mcr_Dispatcher_dispatch_fnc dispatch = NULL,
				  mcr_Dispatcher_modify_fnc modifier = NULL,
				  mcr_Dispatcher_remove_fnc remove = NULL,
				  mcr_Dispatcher_fnc trim = NULL);
	virtual ~CtxDispatcher() {}
	CtxDispatcher(const CtxDispatcher &) = default;
	CtxDispatcher &operator =(const CtxDispatcher &) = default;

	inline const mcr_Dispatcher *ptr() const
	{
		return &dispatcher;
	}
	inline mcr_Dispatcher *ptr()
	{
		return &dispatcher;
	}

	inline Libmacro *context() const
	{
		return _context;
	}
private:
	Libmacro *_context;
};

/*! \ref ITrigger with \ref Libmacro reference
 *
 *  \ref mcr_CtxITrigger
 *  Not typedef because this is a C++ Libmacro context
 */
struct MCR_EXTRAS_API CtxITrigger {
	mcr_ITrigger itrigger;

	/*! \param context If null the last created context will be used.
	 *  Throws EINVAL if no context exists
	 *  \param receive \ref opt mcr_ITrigger.receive
	 *  \param interface \ref opt mcr_ITrigger.interface
	 */
	CtxITrigger(Libmacro *context = NULL, mcr_Dispatcher_receive_fnc receive = NULL,
				const Interface &interface = mcr_Interface_new(0, NULL, NULL, NULL,
						NULL));
	virtual ~CtxITrigger() {}
	CtxITrigger(const CtxITrigger &) = default;
	CtxITrigger &operator =(const CtxITrigger &) = default;

	inline const mcr_ITrigger *ptr() const
	{
		return &itrigger;
	}
	inline mcr_ITrigger *ptr()
	{
		return &itrigger;
	}

	inline Libmacro *context() const
	{
		return _context;
	}
private:
	Libmacro *_context;
};

/*! \ref mcr_Signal
 *
 *  Edit with \ref SignalRef
 */
struct MCR_EXTRAS_API Signal {
	mcr_Signal signal;

	/*! \param isignal \ref mcr_Signal.isignal
	 *  \param isDispatch \ref mcr_Signal.is_dispatch
	 */
	Signal(mcr_ISignal *isignal = NULL, bool isDispatch = false)
	{
		mcr_Signal_init(ptr());
		signal.isignal = isignal;
		signal.is_dispatch = isDispatch;
	}
	virtual ~Signal()
	{
		mcr_Signal_deinit(ptr());
	}
	/*! \ref mcr_Signal_copy */
	Signal(const Signal &copytron)
	{
		mcr_Signal_init(ptr());
		copy(copytron.ptr());
	}
	/*! \ref mcr_Signal_copy */
	Signal(const mcr_Signal &copytron)
	{
		mcr_Signal_init(ptr());
		copy(&copytron);
	}
	/*! \ref mcr_Signal_copy */
	Signal(const mcr_Signal *copytron)
	{
		mcr_Signal_init(ptr());
		copy(copytron);
	}
	/*! \ref mcr_Signal_copy */
	inline Signal &operator =(const Signal &copytron)
	{
		return operator =(copytron.ptr());
	}
	/*! \ref mcr_Signal_copy */
	inline Signal &operator =(const mcr_Signal &copytron)
	{
		return operator =(&copytron);
	}
	inline Signal &operator =(const mcr_Signal *copytron)
	{
		copy(copytron);
		return *this;
	}
	inline Signal &operator =(mcr_ISignal *isignal)
	{
		setISignal(isignal);
		return *this;
	}

	inline const mcr_Signal *ptr() const
	{
		return &signal;
	}
	inline mcr_Signal *ptr()
	{
		return &signal;
	}

	/*! Alias \ref mcr_Signal.is_dispatch */
	inline bool &isDispatch()
	{
		return signal.is_dispatch;
	}

	/*! \ref mcr_Signal_copy */
	void copy(const mcr_Signal *copytron)
	{
		int err;
		if (copytron != ptr()) {
			if ((err = mcr_Signal_copy(ptr(), copytron)))
				throw err;
		}
	}
	/*! \ref mcr_Signal_deinit + set \ref mcr_Signal.isignal */
	void setISignal(mcr_ISignal *isignal);
};

/*! \ref mcr_Trigger
 *
 *  Edit with \ref TriggerRef
 */
struct MCR_EXTRAS_API Trigger {
	mcr_Trigger trigger;

	/*! \param itrigger \ref mcr_Trigger.itrigger
	 *  \param trigger \ref mcr_Trigger.trigger
	 *  \param actor \ref mcr_Trigger.actor
	 */
	Trigger(mcr_ITrigger *itrigger = NULL,
			mcr_Dispatcher_receive_fnc trigger = NULL, void *actor = NULL)
	{
		mcr_Trigger_init(ptr());
		this->trigger.itrigger = itrigger;
		this->trigger.trigger = trigger;
		this->trigger.actor = actor;
	}
	virtual ~Trigger()
	{
		mcr_Trigger_deinit(ptr());
	}
	/*! \ref mcr_Trigger_copy */
	Trigger(const Trigger &copytron)
	{
		mcr_Trigger_init(ptr());
		copy(copytron.ptr());
	}
	/*! \ref mcr_Trigger_copy */
	Trigger(const mcr_Trigger &copytron)
	{
		mcr_Trigger_init(ptr());
		copy(&copytron);
	}
	/*! \ref mcr_Trigger_copy */
	Trigger(const mcr_Trigger *copytron)
	{
		mcr_Trigger_init(ptr());
		copy(copytron);
	}
	/*! \ref mcr_Trigger_copy */
	inline Trigger &operator =(const Trigger &copytron)
	{
		return operator =(copytron.ptr());
	}
	/*! \ref mcr_Trigger_copy */
	inline Trigger &operator =(const mcr_Trigger &copytron)
	{
		return operator =(&copytron);
	}
	/*! \ref mcr_Trigger_copy */
	inline Trigger &operator =(const mcr_Trigger *copytron)
	{
		copy(copytron);
		return *this;
	}
	/*! \ref mcr_Trigger_deinit + set \ref mcr_Trigger.itrigger */
	inline Trigger &operator =(mcr_ITrigger *itrigger)
	{
		setITrigger(itrigger);
		return *this;
	}

	inline const mcr_Trigger *ptr() const
	{
		return &trigger;
	}
	inline mcr_Trigger *ptr()
	{
		return &trigger;
	}

	/*! \ref mcr_Trigger_copy */
	void copy(const mcr_Trigger *copytron)
	{
		int err;
		if (copytron != ptr()) {
			if ((err = mcr_Trigger_copy(ptr(), copytron)))
				throw err;
		}
	}
	/*! \ref mcr_Trigger_deinit + set \ref mcr_Trigger.itrigger */
	void setITrigger(mcr_ITrigger *itrigger);
};

/*! \ref mcr_Macro
 *
 *  Edit with \ref MacroRef
 */
struct MCR_EXTRAS_API Macro {
	typedef mcr_Interrupt Interrupt;

	Macro(Libmacro *context = NULL, bool blocking = false,
		  bool sticky = false, unsigned int threadMax = 1,
		  bool enable = false);
	virtual ~Macro();
	Macro(const Macro &copytron);
	Macro(const mcr_Macro &copytron);
	Macro(const mcr_Macro *copytron);
	inline Macro &operator =(const Macro &copytron)
	{
		copy(copytron);
		return *this;
	}

	inline Macro &operator =(const mcr_Macro &copytron)
	{
		copy(&copytron);
		return *this;
	}
	inline Macro &operator =(const mcr_Macro *copytron)
	{
		copy(copytron);
		return *this;
	}

	inline const mcr_Macro *ptr() const
	{
		return &_macro;
	}
	inline mcr_Macro *ptr()
	{
		return &_macro;
	}

	inline bool blocking() const
	{
		return _macro.blocking;
	}
	void setBlocking(bool val)
	{
		_macro.blocking = val;
	}

	inline bool sticky() const
	{
		return _macro.sticky;
	}
	void setSticky(bool val)
	{
		_macro.sticky = val;
	}

	inline size_t signalCount()
	{
		return _macro.signal_set.used;
	}
	inline void clearSignals()
	{
		setSignals(NULL, 0);
	}
	inline mcr_Signal *signal(size_t index)
	{
		return mcr_Macro_signal(&_macro, index);
	}
	void setSignal(const mcr_Signal *sigPt, size_t index);
	void setSignals(const mcr_Signal *sigArr, size_t count);
	void resizeSignals(size_t count);

	inline unsigned int threadMax() const
	{
		return _macro.thread_max;
	}
	void setThreadMax(unsigned int val);
	inline Interrupt interruptor() const
	{
		return _macro.interruptor;
	}
	void setInterruptor(Interrupt val);
	inline bool isEnabled() const
	{
		return mcr_Macro_is_enabled(ptr());
	}
	void setEnabled(bool val);
	/* Read-only */
	inline mcr_context *ctx() const
	{
		return _macro.ctx;
	}
	inline unsigned int threadCount() const
	{
		return _macro.thread_count;
	}
	inline unsigned int queued() const
	{
		return _macro.queued;
	}

	void copy(const Macro &copytron)
	{
		copy(copytron.ptr());
	}
	void copy(const mcr_Macro *copytron)
	{
		int err;
		if (copytron != &_macro) {
			if ((err = mcr_Macro_copy(&_macro, copytron)))
				throw err;
		}
	}

	inline Libmacro *context() const
	{
		return _context;
	}
private:
	mcr_Macro _macro;
	Libmacro *_context;
};

struct MCR_EXTRAS_API Stage {
	mcr_Stage stage;

	Stage(Libmacro *ctx = NULL,
		  bool blocking = false,
		  mcr_Signal *interceptPt = NULL, unsigned int measurementError = 0,
		  unsigned int mods = 0, int trigFlags = MCR_TF_ALL);
	~Stage();
	Stage(const Stage &copytron);
	Stage(const mcr_Stage *copytron);
	Stage &operator =(const Stage &copytron)
	{
		_context = copytron._context;
		copy(copytron.ptr());
		return *this;
	}
	Stage &operator =(const mcr_Stage *copytron)
	{
		copy(copytron);
		return *this;
	}

	inline const mcr_Stage *ptr() const
	{
		return &stage;
	}
	inline mcr_Stage *ptr()
	{
		return &stage;
	}

	void setIntercept(mcr_Signal *val);
	void setGenericIntercept(mcr_Signal *val);

	int compare(const mcr_Stage *rhsPt)
	{
		return mcr_Stage_compare(ptr(), rhsPt);
	}
	void copy(const mcr_Stage *copytron)
	{
		int err;
		if (copytron != ptr()) {
			err = mcr_Stage_copy(ptr(), copytron);
			if (err)
				throw(err);
		}
	}

	inline Libmacro *context() const
	{
		return _context;
	}
private:
	Libmacro *_context;
};
}

#endif
