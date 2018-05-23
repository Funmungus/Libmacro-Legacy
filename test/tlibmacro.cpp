#include "tlibmacro.h"

#include <stdlib.h>

#include "mcr/modules.h"

void TLibmacro::initTestCase()
{
	_ctx = mcr_allocate();
	QVERIFY(_ctx);
	QCOMPARE(mcr_err, 0);
}

void TLibmacro::cleanupTestCase()
{
	mcr_deallocate(_ctx);
	QCOMPARE(mcr_err, 0);
}
