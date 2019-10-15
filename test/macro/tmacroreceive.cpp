#include "tmacroreceive.h"

/* QCOMPARE = {actual, expected} */

void TMacroReceive::initTestCase()
{
	_ctx = mcr_allocate();
	_mcrPt = new mcr_Macro;
	QCOMPARE(mcr_Macro_init(_mcrPt), 0);
	_mcrPt->ctx = _ctx;
	mcr_MoveCursor move = {};
	mcr_NoOp n = {};
	n.sec = 1;
	mcr_Signal moveSig = {}, nSig = {};
	moveSig.isignal = &_ctx->standard.imove_cursor;
	moveSig.instance.data.data = &move;
	nSig.isignal = &_ctx->standard.inoop;
	nSig.instance.data.data = &n;
	move.is_justify = true;
	move.pos[MCR_Y] = 60;
	QCOMPARE(mcr_Macro_push_signal(_mcrPt, &moveSig), 0);
	QCOMPARE(mcr_Macro_push_signal(_mcrPt, &nSig), 0);
	move.pos[MCR_X] = -60;
	move.pos[MCR_Y] = 0;
	QCOMPARE(mcr_Macro_push_signal(_mcrPt, &moveSig), 0);
	QCOMPARE(mcr_Macro_push_signal(_mcrPt, &nSig), 0);
	move.pos[MCR_X] = 0;
	move.pos[MCR_Y] = -60;
	QCOMPARE(mcr_Macro_push_signal(_mcrPt, &moveSig), 0);
	QCOMPARE(mcr_Macro_push_signal(_mcrPt, &nSig), 0);
	move.pos[MCR_X] = 60;
	move.pos[MCR_Y] = 0;
	QCOMPARE(mcr_Macro_push_signal(_mcrPt, &moveSig), 0);
}

void TMacroReceive::cleanupTestCase()
{
	QCOMPARE(mcr_Macro_deinit(_mcrPt), 0);
	delete(_mcrPt);
	QCOMPARE(mcr_deallocate(_ctx), 0);
}

void TMacroReceive::receive()
{
	mcr_Macro_receive(_mcrPt, nullptr, 0);
	/* Macro contains 3 1-second pauses. */
	std::this_thread::sleep_for(std::chrono::seconds(4));
}
