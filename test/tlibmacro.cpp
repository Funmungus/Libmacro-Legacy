#include "tlibmacro.h"
#include "mcr/modules.h"
#include "stdlib.h"

void TLibmacro::initTestCase()
{
	_ctx = mcr_allocate(false, false);
	QVERIFY(_ctx);
	QCOMPARE(0, mcr_error());
}

void TLibmacro::cleanupTestCase()
{
	QCOMPARE(0, mcr_deallocate(_ctx));
	QCOMPARE(0, mcr_error());
}
