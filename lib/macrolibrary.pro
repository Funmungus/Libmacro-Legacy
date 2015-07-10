TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += thread
CONFIG += debug
QMAKE_CFLAGS += -std=c11
QMAKE_CXXFLAGS += -std=c++11

CONFIG(debug,debug|release):DEFINES += DEBUG

CONFIG(noextras) {}
else {
    CONFIG += extras
    DEFINES += MCR_EXTRAS
}

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

lnx:LIBS += -lcrypto

# native util
win {
    HEADERS += \
        ../include/util/win/def.h \
	../include/util/win/mcrstrings.h \
	../include/util/win/util.h

    SOURCES += \
        ../util/win/winmcrstrings.c
}
lnx {
    HEADERS += \
        ../include/util/lnx/def.h \
	../include/util/lnx/mcrstrings.h \
	../include/util/lnx/priv.h \
	../include/util/lnx/util.h

    SOURCES += \
	../util/lnx/lnxmcrstrings.c \
	../util/lnx/lnxpriv.c
}
mac {
    message(mac util not yet implemented)
}
nativeless {
    HEADERS += \
	../include/util/nativeless/def.h \
	../include/util/nativeless/mcrstrings.h \
	../include/util/nativeless/util.h
    SOURCES += \
        ../util/nativeless/nomcrstrings.c
}

# agnostic util
SOURCES += \
    ../util/array.c \
    ../util/threads.cpp \
    ../util/map.c \
    ../util/util.c \
    ../util/mcrstrings.c \
    ../util/interface.c

HEADERS += \
    ../include/util/map.h \
    ../include/util/def.h \
    ../include/util/util.h \
    ../include/util/array.h \
    ../include/util/threads.h \
    ../include/util/mcrstrings.h \
    ../include/util/interface.h

# native signal
win {
    HEADERS += \
	../include/signal/win/def.h \
	../include/signal/win/standard.h \
	../include/signal/win/signal.h
    SOURCES += \
	../signal/win/winstandard.c \
	../signal/win/winstandardnames.c
}
lnx {
    HEADERS += \
        ../include/signal/lnx/def.h \
	../include/signal/lnx/device.h \
	../include/signal/lnx/standard.h \
	../include/signal/lnx/signal.h

    SOURCES += \
        ../signal/lnx/lnxdevice.c \
	../signal/lnx/lnxstandard.c \
	../signal/lnx/lnxstandardnames.c
}
mac {
    message(mac signal not yet implemented)
}
nativeless {
    HEADERS += \
	../include/signal/nativeless/def.h \
	../include/signal/nativeless/standard.h \
	../include/signal/nativeless/signal.h

    SOURCES += \
	../signal/nativeless/nostandard.c \
	../signal/nativeless/nostandardnames.c
}

# agnostic signal
HEADERS += \
    ../include/signal/def.h \
    ../include/signal/isignal.h \
    ../include/signal/signal.h \
    ../include/signal/standard.h

SOURCES +=  \
    ../signal/signal.c \
    ../signal/standard.c \
    ../signal/signalreg.c \
    ../signal/standardnames.c

# native hotkey
win {
    SOURCES += \
	../hotkey/win/winmods.c
}
lnx {
    SOURCES += \
	../hotkey/lnx/lnxmods.c
}
mac {
    message(mac hotkey not yet implemented)
}
nativeless {
    SOURCES += \
	../hotkey/nativeless/nomods.c
}

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
    ../include/hotkey/hotstandard.h

SOURCES += \
    ../hotkey/dispatch.c \
    ../hotkey/hot.c \
    ../hotkey/dispatchstandard.c \
    ../hotkey/hotkey.c \
    ../hotkey/mods.c \
    ../hotkey/stage.c \
    ../hotkey/hotstandard.c

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

# agnostic macro
HEADERS += \
    ../include/macro/def.h \
    ../include/macro/macro.h

SOURCES += \
    ../macro/macro.c

# wrapper header and init/cleanup
HEADERS += \
    ../include/macro.h

SOURCES += \
    ../macrolibrary.c

win {
HEADERS += \
    ../include/win/macro.h
}
lnx {
HEADERS += \
    ../include/lnx/macro.h
}
nativeless {
HEADERS += \
    ../include/nativeless/macro.h
}

# excludable extras
extras {
HEADERS += \
    ../include/extras/def.h \
    ../include/extras/safestring.h \
    ../include/extras/signalextras.h \
    ../include/extras/script.h \
    ../include/extras/signaloption.h \
    ../include/extras/macroset.h \
    ../include/extras/extras.h

SOURCES += \
    ../extras/safestring.c \
    ../extras/signalextras.c \
    ../extras/script.c \
    ../extras/signaloption.c \
    ../extras/macroset.c \
    ../extras/extras.c

    lnx {
    HEADERS += \
	../include/extras/lnx/def.h
    SOURCES += \
	../extras/lnx/lnxsafestring.c \
	../extras/lnx/lnxsignalextras.c \
	../extras/lnx/lnxextras.c
    }
    win {
    HEADERS += \
	../include/extras/win/def.h
    SOURCES += \
	../extras/win/winsafestring.c \
	../extras/win/winsignalextras.c \
	../extras/win/winextras.c
    }
    nativeless {
    HEADERS += \
	../include/extras/nativeless/def.h
    SOURCES += \
	../extras/nativeless/nosafestring.c \
	../extras/nativeless/nosignalextras.c \
	../extras/nativeless/noextras.c
    }
}

OTHER_FILES += ../TODO.md

lnx {
    documentation.path = /usr/local/macrolibrary/doc
    documentation.files = ../doxygen/html/*
    man.path = /usr/local/share/man
    man.files = ../doxygen/man/*
    lib.path = /usr/lib/macrolibrary
    lib.files = *.a *.so
    INSTALLS += documentation lib man
}
