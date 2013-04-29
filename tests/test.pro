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

HEADERS += \
    ../include/macro.h

SOURCES += \
    arraytest.c
#    dispatchtest.c
#    hottest.c
#    sendtest.c
#    standardtest.c
#    signaltest.c
#    maptest.c
#    arraytest.c
