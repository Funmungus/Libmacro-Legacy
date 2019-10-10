QT -= gui
QT += testlib

CONFIG += console testcase
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(../common.pri)

LIBS += "-L$${OUT_PWD}/../lib$${DIR_POSTFIX}"
!isEmpty(DESTDIR) {
	LIBS += "-L$${DESTDIR}"
}
# Windows target and linkage
windows {
    LIBS += -l$$qtLibraryTarget(libmacro)$${MCR_VER_MAJ}
    !static:!staticlib: LIBS += -llibcrypto
} else {
    LIBS += -l$$qtLibraryTarget(macro)
    !static:!staticlib: LIBS += -l$$qtLibraryTarget(crypto)
}

HEADERS += $$files(*.h, true)

SOURCES += main.cpp \
	tlibmacro.cpp \
	signal/tgendispatch.cpp \
	macro/tmacroreceive.cpp

