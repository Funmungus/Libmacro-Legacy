TEMPLATE = app
CONFIG += console
CONFIG += static
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += thread
CONFIG += debug
QMAKE_CFLAGS += -std=c11
QMAKE_CXXFLAGS += -std=c++11

CONFIG(debug,debug|release):DEFINES += DEBUG

CONFIG(noextras) {}
else:DEFINES += MCR_EXTRAS

INCLUDEPATH += ../include
unix:LIBS += -L../lib -lmacrolibrary
win32:LIBS += ../lib/macrolibrary.lib

win32:CONFIG += win
else:win64:CONFIG += win
else:unix {
    macx:CONFIG += mac
    else:CONFIG += lnx
}
else:CONFIG += nativeless

win:DEFINES += MCR_NATIVE_DIR=win
lnx:DEFINES += MCR_NATIVE_DIR=lnx
mac:DEFINES += MCR_NATIVE_DIR=nativeless
nativeless:DEFINES += MCR_NATIVE_DIR=nativeless

lnx:LIBS += -lcrypto

HEADERS += ../include/macro.h \
    macronsole.h

SOURCES += main.c

lnx {
    SOURCES += lnx/lnxmacronsole.c
}
win {
    SOURCES += win/winmacronsole.c
}
nativeless {
    SOURCES += nativeless/nomacronsole.c
}

