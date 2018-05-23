#include <QtTest/QtTest>

#include "mcr/libmacro.h"

class TGenDispatch : public QObject
{
	Q_OBJECT
public:
	TGenDispatch() : _received(false) {}

private slots:
	void initTestCase();
	void cleanupTestCase();

	void unspecific();
	void specific();

private:
	mcr_context *_ctx;
	bool _received;

	static bool receive(void *receiver,
						struct mcr_Signal * dispatchSignal, unsigned int mods);

	bool receive(struct mcr_Signal * dispatchSignal, unsigned int mods);
};
