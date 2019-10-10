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

#include "mcr/extras/extras.h"

namespace mcr
{
CtxISignal::CtxISignal(Libmacro *context, mcr_signal_fnc send,
					   mcr_Dispatcher *dispatcher, const Interface &interface)
	: _context(context ? context : Libmacro::instance())
{
	if (!send)
		throw EFAULT;
	mcr_ISignal_init(ptr());
	isignal.send = send;
	isignal.dispatcher = dispatcher;
	isignal.interface = interface;
}

CtxISignal::CtxISignal(mcr_signal_fnc send)
	: _context(Libmacro::instance())
{
	if (!send)
		throw EFAULT;
	mcr_ISignal_init(ptr());
	isignal.send = send;
}

CtxDispatcher::CtxDispatcher(Libmacro *context, mcr_Dispatcher_add_fnc add,
							 mcr_Dispatcher_fnc clear,
							 mcr_Dispatcher_dispatch_fnc dispatch,
							 mcr_Dispatcher_modify_fnc modifier,
							 mcr_Dispatcher_remove_fnc remove,
							 mcr_Dispatcher_fnc trim)
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
						 const Interface &interface)
	: _context(context ? context : Libmacro::instance())
{
	mcr_ITrigger_init(ptr());
	itrigger.interface = interface;
	itrigger.receive = receive;
}

void Signal::setISignal(mcr_ISignal *isignal)
{
	if (isignal != signal.isignal) {
		if (mcr_Signal_deinit(ptr()))
			throw mcr_err;
		signal.isignal = isignal;
	}
}

void Trigger::setITrigger(mcr_ITrigger *itrigger)
{
	if (itrigger != trigger.itrigger) {
		if (mcr_Trigger_deinit(ptr()))
			throw mcr_err;
		trigger.itrigger = itrigger;
	}
}

Macro::Macro(Libmacro *context, bool blocking, bool sticky,
			 unsigned int threadMax,
			 bool enable)
	: _context(context ? context : Libmacro::instance()),
	  _activators(new std::vector<Signal>), _name(new std::string),
	  _triggers(new std::vector<Trigger>)
{
	if (mcr_Macro_init(ptr()))
		throw mcr_err;
	if (mcr_Macro_set_all(ptr(), blocking, sticky, threadMax, enable,
						  _context->ptr())) {
		throw mcr_err;
	}
	/* Nothing to dispatch yet */
}

Macro::Macro(const Macro &copytron)
	: _context(copytron.context()),
	  _activators(new std::vector<Signal>(copytron.activatorsRef())),
	  _name(new std::string(copytron.nameRef())),
	  _triggers(new std::vector<Trigger>(copytron.triggersRef()))
{
	if (!context())
		_context = Libmacro::instance();
	if (mcr_Macro_init(ptr()))
		throw mcr_err;
	if (mcr_Macro_copy(ptr(), copytron.ptr()))
		throw mcr_err;
	enableDispatch();
	triggerMe();
}

Macro::Macro(const mcr_Macro &copytron)
	: _context(Libmacro::instance()), _activators(new std::vector<Signal>),
	  _name(new std::string), _triggers(new std::vector<Trigger>)
{
	if (mcr_Macro_init(ptr()))
		throw mcr_err;
	if (mcr_Macro_copy(ptr(), &copytron))
		throw mcr_err;
	/* Nothing to dispatch yet */
}

Macro::Macro(const mcr_Macro *copytron)
	: _context(Libmacro::instance()), _activators(new std::vector<Signal>),
	  _name(new std::string), _triggers(new std::vector<Trigger>)
{
	if (mcr_Macro_init(ptr()))
		throw mcr_err;
	if (mcr_Macro_copy(ptr(), copytron))
		throw mcr_err;
	/* Nothing to dispatch yet */
}

Macro::~Macro()
{
	removeDispatch();
	mcr_Macro_deinit(ptr());
	delete &activatorsRef();
	delete &nameRef();
	delete &triggersRef();
}

size_t Macro::activatorCount() const
{
	return activatorsRef().size();
}

Signal *Macro::activatorData() const
{
	if (activatorsRef().empty())
		return nullptr;
	return &activatorsRef().front();
}

const char *Macro::c_name() const
{
	return nameRef().c_str();
}

void Macro::setName(const char *val)
{
	nameRef() = val;
}

size_t Macro::triggerCount() const
{
	return triggersRef().size();
}

Trigger *Macro::triggerData() const
{
	if (triggersRef().empty())
		return nullptr;
	return &triggersRef().front();
}

void Macro::setSignal(size_t index, const mcr_Signal *sigPt)
{
	if (mcr_Macro_set_signal(&_macro, sigPt, index))
		throw mcr_err;
}

void Macro::setSignals(const mcr_Signal *sigArr, size_t count)
{
	if (mcr_Macro_set_signals(&_macro, sigArr, count))
		throw mcr_err;
}

void Macro::resizeSignals(size_t count)
{
	if (count != signalCount()) {
		if (count < signalCount()) {
			while (count < signalCount()) {
				if (mcr_Macro_pop_signal(ptr()))
					throw mcr_err;
			}
		} else {
			mcr_Signal initial;
			mcr_Signal_init(&initial);
			if (mcr_Array_minfill(&ptr()->signal_set, count,
								  &initial)) {
				throw mcr_err;
			}
		}
	}
}

void Macro::setThreadMax(unsigned int val)
{
	if (val <= MCR_THREAD_MAX)
		_macro.thread_max = val;
}

void Macro::setInterruptor(Interrupt val)
{
	if (mcr_Macro_interrupt(ptr(), val))
		throw mcr_err;
}

void Macro::setEnabled(bool val)
{
	if (val != isEnabled()) {
		if (mcr_Macro_set_enabled(ptr(), val))
			throw mcr_err;
		enableDispatch();
	}
}

void Macro::copy(const Macro &copytron)
{
	if (&copytron != this) {
		setEnabled(false);
		if (mcr_Macro_copy(ptr(), copytron.ptr()))
			throw mcr_err;
		activatorsRef() = copytron.activatorsRef();
		nameRef() = copytron.nameRef();
		triggersRef() = copytron.triggersRef();
		triggerMe();
		enableDispatch();
	}
}

void Macro::removeDispatch()
{
	mcr_Dispatcher_remove_all(context()->ptr(), ptr());
	for (auto &trigPt: triggersRef()) {
		mcr_Dispatcher_remove_all(context()->ptr(), trigPt.ptr());
	}
}

void Macro::addDispatch(Signal &sigPt, Trigger &trigPt)
{
	if (mcr_Macro_add_dispatch(ptr(), trigPt.ptr(), sigPt.ptr()))
		throw mcr_err;
}

void Macro::setActivatorCount(size_t count)
{
	activatorsRef().resize(count);
}

void Macro::setActivator(size_t index, const Signal &sigPt)
{
	auto &activators = activatorsRef();
	if (index >= activators.size())
		throw EINVAL;
	activators[index] = sigPt;
}

void Macro::setTriggerCount(size_t count)
{
	triggersRef().resize(count);
	triggerMe();
}

void Macro::setTrigger(size_t index, const Trigger &trigPt)
{
	auto &triggers = triggersRef();
	if (index >= triggers.size())
		throw EINVAL;
	triggerMe(triggers[index] = trigPt);
}

void Macro::enableDispatch()
{
	if (isEnabled()) {
		for (auto &i: activatorsRef()) {
			for (auto &j: triggersRef()) {
				addDispatch(i, j);
			}
		}
	} else {
		removeDispatch();
	}
}

Stage::Stage(Libmacro *ctx, bool blocking, mcr_Signal *interceptPt,
			 unsigned int measurementError, unsigned int mods, int trigFlags)
	: _context(ctx ? ctx : Libmacro::instance())
{
	mcr_Stage_init(ptr());
	if (mcr_Stage_set_all(_context->ptr(), ptr(), blocking, interceptPt,
						  measurementError, mods, trigFlags)) {
		throw mcr_err;
	}
}

Stage::Stage(const Stage &copytron)
	: _context(copytron._context)
{
	mcr_Stage_init(ptr());
	if (mcr_Stage_copy(ptr(), copytron.ptr()))
		throw mcr_err;
}

Stage::Stage(const mcr_Stage *copytron)
	: _context(Libmacro::instance())
{
	mcr_Stage_init(ptr());
	if (mcr_Stage_copy(ptr(), copytron))
		throw mcr_err;
}

Stage::~Stage()
{
	mcr_Stage_deinit(ptr());
}

void Stage::setIntercept(mcr_Signal *val)
{
	if (mcr_Stage_set_intercept(_context->ptr(), ptr(), val))
		throw mcr_err;
}

void Stage::setGenericIntercept(mcr_Signal *val)
{
	if (mcr_Stage_set_intercept_generic(_context->ptr(), ptr(), val))
		throw mcr_err;
}
}
