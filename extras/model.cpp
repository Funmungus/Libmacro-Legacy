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

#include "mcr/extras/mod_extras.h"

namespace mcr
{
CtxISignal::CtxISignal(mcr::Libmacro *context, mcr_signal_fnc send,
		       mcr_Dispatcher *dispatcher, const Interface &interface) MCR_THROWS
	: _context(context ? context : Libmacro::instance())
{
	if (!send)
		throw EFAULT;
	mcr_ISignal_init(ptr());
	isignal.send = send;
	isignal.dispatcher = dispatcher;
	isignal.interface = interface;
}

CtxISignal::CtxISignal(mcr_signal_fnc send) MCR_THROWS
	: _context(Libmacro::instance())
{
	if (!send)
		throw EFAULT;
	mcr_ISignal_init(ptr());
	isignal.send = send;
}

CtxDispatcher::CtxDispatcher(mcr::Libmacro *context, mcr_Dispatcher_add_fnc add,
			     mcr_Dispatcher_fnc clear,
			     mcr_Dispatcher_dispatch_fnc dispatch,
			     mcr_Dispatcher_modify_fnc modifier,
			     mcr_Dispatcher_remove_fnc remove,
			     mcr_Dispatcher_fnc trim) MCR_THROWS
	: _context(context ? context : Libmacro::instance())
{
	mcr_Dispatcher_init(ptr());
	dispatcher.add = add;
	dispatcher.clear = clear;
	dispatcher.dispatch = dispatch;
	dispatcher.modifier = modifier;
	dispatcher.remove = remove;
	dispatcher.trim = trim;
}

CtxITrigger::CtxITrigger(Libmacro *context, mcr_Dispatcher_receive_fnc receive,
			 const Interface &interface) MCR_THROWS
	: _context(context ? context : Libmacro::instance())
{
	mcr_ITrigger_init(ptr());
	itrigger.interface = interface;
	itrigger.receive = receive;
}

void Signal::setISignal(mcr_ISignal *isignal) MCR_THROWS
{
	int err;
	if (isignal != signal.isignal) {
		if ((err = mcr_Signal_deinit(ptr())))
			throw err;
		signal.isignal = isignal;
	}
}

void Trigger::setITrigger(mcr_ITrigger *itrigger) MCR_THROWS
{
	int err;
	if (itrigger != trigger.itrigger) {
		if ((err = mcr_Trigger_deinit(ptr())))
			throw err;
		trigger.itrigger = itrigger;
	}
}

Macro::Macro(Libmacro *context, bool block, bool sticky, unsigned int threadMax,
	     bool enable) MCR_THROWS
	: _context(context ? context : Libmacro::instance())
{
	int err;
	if ((err = mcr_Macro_init(ptr())))
		throw err;
	if ((err = mcr_Macro_set_all(ptr(), block, sticky, threadMax, enable,
				     _context->ptr())))
		throw err;
}

Macro::~Macro() MCR_THROWS
{
	int err = mcr_Macro_deinit(ptr());
	if (err)
		throw err;
}

Macro::Macro(const Macro &copytron) MCR_THROWS
{
	int err = mcr_Macro_init(ptr());
	if (err)
		throw err;
	if ((err = mcr_Macro_copy(ptr(), copytron.ptr())))
		throw err;
}

Macro::Macro(const mcr_Macro &copytron) MCR_THROWS
{
	int err = mcr_Macro_init(ptr());
	if (err)
		throw err;
	if ((err = mcr_Macro_copy(ptr(), &copytron)))
		throw err;
}

Macro::Macro(const mcr_Macro *copytron) MCR_THROWS
{
	int err = mcr_Macro_init(ptr());
	if (err)
		throw err;
	if ((err = mcr_Macro_copy(ptr(), copytron)))
		throw err;
}

void Macro::setSignal(const mcr_Signal *sigPt, size_t index) MCR_THROWS
{
	int err = mcr_Macro_set_signal(&_macro, sigPt, index);
	if (err)
		throw err;
}

void Macro::setSignals(const mcr_Signal *sigArr, size_t count) MCR_THROWS
{
	int err = mcr_Macro_set_signals(&_macro, sigArr, count);
	if (err)
		throw err;
}

void Macro::resizeSignals(size_t count) MCR_THROWS
{
	mcr_Macro_set_signals(ptr(), NULL, 0);
	int err = mcr_Array_resize(&ptr()->signal_set, count);
	if (err)
		throw err;
	mcr_Signal initial;
	mcr_Signal_init(&initial);
	for (size_t n = 0; n < count; n++) {
		mcr_Array_push(&ptr()->signal_set, &initial);
	}
}

void Macro::setThreadMax(unsigned int val)
{
	if (val < MCR_THREAD_MAX)
		_macro.thread_max = val;
}

void Macro::setInterruptor(Interrupt val) MCR_THROWS
{
	int err = mcr_Macro_interrupt(ptr(), val);
	if (err)
		throw err;
}

void Macro::setEnabled(bool val) MCR_THROWS
{
	int err = mcr_Macro_set_enabled(ptr(), val);
	if (err)
		throw err;
}

Stage::Stage(Libmacro *ctx, bool blocking, mcr_Signal *interceptPt,
	     unsigned int measurementError, unsigned int mods, int trigFlags) MCR_THROWS
	: _context(ctx ? ctx : Libmacro::instance())
{
	int err;
	mcr_Stage_init(ptr());
	err = mcr_Stage_set_all(_context->ptr(), ptr(), blocking, interceptPt,
				measurementError, mods, trigFlags);
	if (err)
		throw(err);
}

Stage::~Stage() MCR_THROWS
{
	int err = mcr_Stage_deinit(ptr());
	if (err)
		throw(err);
}

Stage::Stage(const Stage &copytron) MCR_THROWS
	: _context(copytron._context)
{
	int err;
	mcr_Stage_init(ptr());
	err = mcr_Stage_copy(ptr(), copytron.ptr());
	if (err)
		throw(err);
}

Stage::Stage(const mcr_Stage *copytron) MCR_THROWS
	: _context(Libmacro::instance())
{
	int err;
	mcr_Stage_init(ptr());
	err = mcr_Stage_copy(ptr(), copytron);
	if (err)
		throw(err);
}

void Stage::setIntercept(mcr_Signal *val) MCR_THROWS
{
	int err = mcr_Stage_set_intercept(_context->ptr(), ptr(), val);
	if (err)
		throw(err);
}

void Stage::setGenericIntercept(mcr_Signal *val) MCR_THROWS
{
	int err = mcr_Stage_set_intercept_generic(_context->ptr(), ptr(), val);
	if (err)
		throw(err);
}
}
