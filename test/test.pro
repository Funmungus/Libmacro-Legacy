QT -= gui
QT += testlib

CONFIG += c++11 console testcase
CONFIG -= app_bundle

include(../common.pri)
include(../libmacro.pri)

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += $$files(*.h, true)
SOURCES += main.cpp \
	tlibmacro.cpp \
	signal/tgendispatch.cpp

win {
	CONFIG(debug, debug|release) {
		LIBS += "-L$${OUT_PWD}/../extras/debug" "-L$${OUT_PWD}/../macro/debug"
	} else {
		LIBS += "-L$${OUT_PWD}/../extras/release" "-L$${OUT_PWD}/../macro/release"
	}
	nostl|stl_off {
		LIBS += -l$$qtLibraryTarget(libmacro-extras-nostl)$${MCR_VER_MAJ}
	} else {
		LIBS += -l$$qtLibraryTarget(libmacro-extras)$${MCR_VER_MAJ}
	}
	LIBS += -l$$qtLibraryTarget(libmacro)$${MCR_VER_MAJ}
} else {
	LIBS += "-L$${OUT_PWD}/../extras" "-L$${OUT_PWD}/../macro"
	nostl|stl_off {
		LIBS += -l$$qtLibraryTarget(macro-extras-nostl)
	} else {
		LIBS += -l$$qtLibraryTarget(macro-extras)
	}
	LIBS += -l$$qtLibraryTarget(macro)
}
