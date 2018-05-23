#include <QtTest/QtTest>

#include "tlibmacro.h"
#include "signal/tgendispatch.h"

int main(int argc, char **argv)
{
	TLibmacro tlibmacro;
	TGenDispatch tgendispatch;
	QTest::qExec(&tlibmacro, argc, argv);
	QTest::qExec(&tgendispatch, argc, argv);
	return 0;
}
