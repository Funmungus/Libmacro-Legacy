TEMPLATE = app
CONFIG += console
CONFIG += static
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += thread
CONFIG += debug
QMAKE_CFLAGS += -std=c11
QMAKE_CXXFLAGS += -std=c++11

DEFINES += MCR_NO_SDK

CONFIG(debug,debug|release):DEFINES += DEBUG

INCLUDEPATH += ../include
unix:LIBS += -L../project -lmacro
win32:LIBS += ../project/macro.lib

win32:DEFINES += MCR_NATIVE_DIR=win
else:unix:DEFINES += MCR_NATIVE_DIR=lnx

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
