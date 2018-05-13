QT -= gui
QT += testlib

CONFIG += console testcase
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../common.pri)

LIBS += "-L$${OUT_PWD}/../extras$${DIR_POSTFIX}" "-L$${OUT_PWD}/../macro$${DIR_POSTFIX}"
# Windows target and linkage
win {
	nostl|stl_off: LIBS += -l$$qtLibraryTarget(libmacro-extras-nostl)$${MCR_VER_MAJ}
	else:LIBS += -l$$qtLibraryTarget(libmacro-extras)$${MCR_VER_MAJ}
	LIBS += -l$$qtLibraryTarget(libmacro)$${MCR_VER_MAJ}
} else {
	nostl|stl_off:LIBS += -l$$qtLibraryTarget(macro-extras-nostl)
	else:LIBS += -l$$qtLibraryTarget(macro-extras)
	LIBS += -l$$qtLibraryTarget(macro)
}

HEADERS += $$files(*.h, true)

SOURCES += main.cpp \
	tlibmacro.cpp \
	signal/tgendispatch.cpp

