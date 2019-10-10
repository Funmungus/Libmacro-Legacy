#include <QtTest/QtTest>

#include "tlibmacro.h"
#include "signal/tgendispatch.h"
#include "macro/tmacroreceive.h"

int main(int argc, char **argv)
{
	TLibmacro tlibmacro;
	TGenDispatch tgendispatch;
	TMacroReceive tmacroreceive;
	QTest::qExec(&tlibmacro, argc, argv);
	QTest::qExec(&tgendispatch, argc, argv);
	QTest::qExec(&tmacroreceive, argc, argv);
	return 0;
}
