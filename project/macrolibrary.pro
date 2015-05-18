TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += staticlib
CONFIG += thread
CONFIG += debug
QMAKE_CFLAGS += -std=c11
QMAKE_CXXFLAGS += -std=c++11

DEFINES += MCR_NO_SDK

CONFIG(debug,debug|release):DEFINES += DEBUG

INCLUDEPATH += ../include

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

# native util
win {
    HEADERS += \
        ../include/util/win/def.h \
        ../include/util/win/mcrstrings.h

    SOURCES += \
        ../util/win/winmcrstrings.c
}
lnx {
    HEADERS += \
        ../include/util/lnx/def.h \
        ../include/util/lnx/mcrstrings.h

    SOURCES += \
        ../util/lnx/lnxmcrstrings.c
}
mac {
    message(mac util not yet implemented)
}
nativeless {
    HEADERS += \
        ../include/util/nativeless/def.h \
        ../include/util/nativeless/mcrstrings.h
    SOURCES += \
        ../util/nativeless/nomcrstrings.c
}

# agnostic util
SOURCES += \
    ../util/array.c \
    ../util/threads.cpp \
    ../util/map.c \
    ../util/util.c \
    ../util/mcrstrings.c

HEADERS += \
    ../include/util/map.h \
    ../include/util/def.h \
    ../include/util/util.h \
    ../include/util/array.h \
    ../include/util/threads.h \
    ../include/util/mcrstrings.h

# native signal
win {
    HEADERS += \
        ../include/signal/win/def.h \
        ../include/signal/win/standard.h
    SOURCES += \
        ../signal/win/winstandard.c
}
lnx {
    HEADERS += \
        ../include/signal/lnx/def.h \
        ../include/signal/lnx/device.h \
        ../include/signal/lnx/standard.h

    SOURCES += \
        ../signal/lnx/lnxdevice.c \
        ../signal/lnx/lnxstandard.c
}
mac {
    message(mac signal not yet implemented)
}
nativeless {
    HEADERS += \
        ../include/signal/nativeless/def.h \
        ../include/signal/nativeless/standard.h

    SOURCES += \
        ../signal/nativeless/nostandard.c
}

# agnostic signal
HEADERS += \
    ../include/signal/def.h \
    ../include/signal/isignal.h \
    ../include/signal/signal.h \
    ../include/signal/standard.h

SOURCES +=  \
    ../signal/signal.c \
    ../signal/standard.c

# agnostic hotkey
HEADERS += \
    ../include/hotkey/def.h \
    ../include/hotkey/dispatchutils.h \
    ../include/hotkey/dispatch.h \
    ../include/hotkey/dispatchstandard.h \
    ../include/hotkey/hot.h \
    ../include/hotkey/hotkey.h \
    ../include/hotkey/mods.h \
    ../include/hotkey/stage.h \
    ../include/hotkey/hotstaged.h

SOURCES += \
    ../hotkey/dispatch.c \
    ../hotkey/hot.c \
    ../hotkey/dispatchstandard.c \
    ../hotkey/hotkey.c \
    ../hotkey/mods.c \
    ../hotkey/stage.c \
    ../hotkey/hotstaged.c

# agnostic intercept
HEADERS += \
    ../include/intercept/def.h \
    ../include/intercept/intercept.h

SOURCES += \
    ../intercept/intercept.c

win {
HEADERS += \
    ../include/intercept/win/def.h \
    ../include/intercept/win/grabber.h \
    ../include/intercept/win/intercept.h
SOURCES += \
    ../intercept/win/wingrabber.c \
    ../intercept/win/winintercept.c
}
lnx {
HEADERS += \
    ../include/intercept/lnx/def.h \
    ../include/intercept/lnx/grabber.h \
    ../include/intercept/lnx/intercept.h
SOURCES += \
    ../intercept/lnx/lnxgrabber.c \
    ../intercept/lnx/lnxintercept.c
}
mac {
    message(mac not yet implemented)
}
nativeless {
HEADERS += \
    ../include/intercept/nativeless/def.h
}

# wrapper header and init/cleanup
HEADERS += \
    ../include/macro.h

SOURCES += \
    ../macrolibrary.c

lnx {
    documentation.path = /usr/local/macrolibrary/doc
    documentation.files = ../doxygen/html/*
    man.path = /usr/local/share/man
    man.files = ../doxygen/man/*
    lib.path = /usr/lib/macrolibrary
    lib.files = *.a *.so
    INSTALLS += documentation lib man
}
