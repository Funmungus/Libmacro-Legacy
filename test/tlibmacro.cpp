#include "tlibmacro.h"
#include "mcr/modules.h"
#include "stdlib.h"

void TLibmacro::initTestCase()
{
	_ctx = mcr_allocate();
	QVERIFY(_ctx);
	QCOMPARE(0, mcr_err);
}

void TLibmacro::cleanupTestCase()
{
	QCOMPARE(0, mcr_deallocate(_ctx));
	QCOMPARE(0, mcr_err);
}
