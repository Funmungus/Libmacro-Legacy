#include "tgendispatch.h"

#include "mcr/modules.h"

/* QCOMPARE = {actual, expected} */

void TGenDispatch::initTestCase()
{
	_ctx = mcr_allocate();
	_ctx->signal.is_generic_dispatcher = true;
	QCOMPARE(_ctx->signal.generic_dispatcher_pt,
			 (mcr_Dispatcher *)&_ctx->signal.generic_dispatcher);
}

void TGenDispatch::cleanupTestCase()
{
	QCOMPARE(mcr_deallocate(_ctx), 0);
}

void TGenDispatch::unspecific()
{
	mcr_Signal siggy;
	mcr_Signal_init(&siggy);
	QCOMPARE(mcr_Dispatcher_add_generic(_ctx, NULL, this, receive), 0);
	QCOMPARE(_ctx->signal.generic_dispatcher.receivers.used, (size_t)1);
	mcr_dispatch(_ctx, NULL);
	QVERIFY(_received);
	_received = false;
	mcr_dispatch(_ctx, &siggy);
	QVERIFY(_received);
	_received = false;

	QCOMPARE(mcr_Dispatcher_remove(_ctx, NULL, this), 0);
	QCOMPARE(_ctx->signal.generic_dispatcher.receivers.used, (size_t)0);
}

void TGenDispatch::specific()
{
	mcr_Signal siggy;
	void *elementPt;
	void *keyPt = &siggy;
	mcr_DispatchPair pair, *pairPt;
	mcr_Array *arrPt;
	pair.receiver = this;
	pair.dispatch = receive;
	mcr_Signal_init(&siggy);
	QCOMPARE(mcr_Dispatcher_add_generic(_ctx, &siggy, this, receive), 0);
	QCOMPARE(_ctx->signal.generic_dispatcher.signal_receivers.set.used, (size_t)1);
	elementPt = mcr_Map_element(&_ctx->signal.generic_dispatcher.signal_receivers,
								&keyPt);
	QCOMPARE(elementPt, (void *)
			 _ctx->signal.generic_dispatcher.signal_receivers.set.array);
	keyPt = this;
	pairPt = (mcr_DispatchPair *)mcr_Array_find((const mcr_Array *)MCR_MAP_VALUEOF(
				 _ctx->signal.generic_dispatcher.signal_receivers, elementPt), &keyPt);
	QVERIFY(!memcmp(&pair, pairPt, sizeof(mcr_DispatchPair)));
	mcr_dispatch(_ctx, NULL);
	QVERIFY(!_received);
	mcr_dispatch(_ctx, &siggy);
	QVERIFY(_received);
	_received = false;

	QCOMPARE(mcr_Dispatcher_remove(_ctx, NULL, this), 0);
	keyPt = &siggy;
	arrPt = (mcr_Array *)mcr_Map_value(
				&_ctx->signal.generic_dispatcher.signal_receivers, &keyPt);
	QCOMPARE(arrPt->used, (size_t)0);
	QCOMPARE(_ctx->signal.generic_dispatcher.signal_receivers.set.used, (size_t)1);
	QCOMPARE(mcr_Map_clear(&_ctx->signal.generic_dispatcher.signal_receivers), 0);
	QCOMPARE(_ctx->signal.generic_dispatcher.signal_receivers.set.used, (size_t)0);
}

bool TGenDispatch::receive(void *receiver, mcr_Signal *dispatchSignal,
						   unsigned int mods)
{
	return ((TGenDispatch *)receiver)->receive(dispatchSignal, mods);
}

bool TGenDispatch::receive(mcr_Signal *dispatchSignal, unsigned int mods)
{
	UNUSED(dispatchSignal);
	UNUSED(mods);
	_received = true;
	return false;
}
