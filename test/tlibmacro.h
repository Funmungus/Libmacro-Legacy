#include <QtTest/QtTest>

#include "mcr/libmacro.h"

class TLibmacro : public QObject
{
	Q_OBJECT
private slots:
	void initTestCase();
	void cleanupTestCase();

private:
	mcr_context *_ctx;
};
