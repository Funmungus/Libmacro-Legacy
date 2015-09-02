TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += thread
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

HEADERS = ../include/macro.h

arraytest {
TARGET = arraytest
SOURCES = arraytest.c
}

maptest {
TARGET = maptest
SOURCES = maptest.c
}

signaltest {
TARGET = signaltest
SOURCES = signaltest.c
}

standardtest {
TARGET = standardtest
SOURCES = standardtest.c
}

sendtest {
TARGET = sendtest
SOURCES = sendtest.c
}

hottest {
TARGET = hottest
SOURCES = hottest.c
}

modstest {
TARGET = modstest
SOURCES = modstest.c
}

dispatchtest {
TARGET = dispatchtest
SOURCES = dispatchtest.c
}

dispatchstandardtest {
TARGET = dispatchstandardtest
SOURCES = dispatchstandardtest.c
}

stagetest {
TARGET = stagetest
SOURCES = stagetest.c
}

hotstagedtest {
TARGET = hotstagedtest
SOURCES = hotstagedtest.c
}

grabtest {
TARGET = grabtest
SOURCES = grabtest.c
}

crypttest {
TARGET = crypttest
SOURCES = crypttest.c
}
