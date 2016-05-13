TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += thread
QMAKE_CFLAGS += -std=c11
QMAKE_CXXFLAGS += -std=c++11

CONFIG(debug,debug|release):DEFINES += DEBUG

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

msvc:LIBS += -luser32
win {
	DEFINES += MCR_EXPORTS
	msvc {
		CONFIG(debug,debug|release):LIBS += -llibeay32MTd
		else:LIBS += -llibeay32MT
	} else {
		LIBS += -llibeay32
	}
} else {
	lnx:LIBS += -lcrypto
}

# util, signal, macro, standard, intercept, extras
# platform-agnostic
SOURCES += \
	util/array.c \
	util/threads.cpp \
	util/map.c \
	util/bimap.c \
	util/mcrstring.c \
	util/interface.c \
	util/registry.c \
	util/string_map.c \
	util/name_map.c \
	util/name_bimap.c \
	util/instance.c \
	util/string_set.c \
	util/string_index.c \
	util/error.c \
	util/util.c \
	signal/isignal.c \
	signal/dispatch_pair.c \
	signal/dispatcher.c \
	signal/mod_flags.c \
	signal/dispatch_map.c \
	signal/signal.c \
	signal/mod_signal.c \
	macro/itrigger.c \
	macro/trigger.c \
	macro/macro.c \
	macro/mod_macro.c \
	standard/mods.c \
	standard/stage.c \
	standard/key.c \
	standard/move_cursor.c \
	standard/hid_echo.c \
	standard/alarm.c \
	standard/noop.c \
	standard/scroll.c \
    standard/staged.c \
    standard/action.c \
	standard/standard.c \
    standard/mod_standard.c \
	intercept/intercept.c \
    intercept/mod_intercept.c \
	libmacro.c \
    extras/mod_extras.c

HEADERS += \
	mcr/util/def.h \
	mcr/util/map.h \
	mcr/util/bimap.h \
	mcr/util/array.h \
	mcr/util/threads.h \
	mcr/util/mcrstring.h \
	mcr/util/instance.h \
	mcr/util/interface.h \
	mcr/util/priv.h \
	mcr/util/registry.h \
	mcr/util/error.h \
	mcr/util/name_bimap.h \
	mcr/util/name_map.h \
	mcr/util/string_index.h \
	mcr/util/string_map.h \
	mcr/util/string_set.h \
	mcr/util/util.h \
	mcr/signal/def.h \
	mcr/signal/isignal.h \
	mcr/signal/dispatcher.h \
	mcr/signal/dispatch_pair.h \
	mcr/signal/dispatch_utils.h \
	mcr/signal/dispatch_map.h \
	mcr/signal/signal.h \
	mcr/signal/private.h \
	mcr/signal/mod_signal.h \
	mcr/signal/mod_flags.h \
	mcr/macro/def.h \
	mcr/macro/itrigger.h \
	mcr/macro/trigger.h \
	mcr/macro/private.h \
	mcr/macro/macro.h \
	mcr/macro/mod_macro.h \
	mcr/standard/def.h \
	mcr/standard/stage.h \
	mcr/standard/mods.h \
	mcr/standard/trigger_flags.h \
	mcr/standard/key.h \
	mcr/standard/private.h \
	mcr/standard/noop.h \
	mcr/standard/move_cursor.h \
	mcr/standard/alarm.h \
	mcr/standard/scroll.h \
	mcr/standard/hid_echo.h \
	mcr/standard/mod_standard.h \
	mcr/standard/standard.h \
	mcr/intercept/def.h \
	mcr/intercept/def.h \
	mcr/intercept/intercept.h \
	mcr/extras/macro_set.h \
	mcr/extras/safe_string.h \
	mcr/extras/signal_extras.h \
	mcr/extras/signal_option.h \
	mcr/libmacro.h \
	mcr/modules.h \
    mcr/standard/staged.h \
    mcr/standard/action.h \
    mcr/intercept/mod_intercept.h \
    mcr/intercept/private.h \
    mcr/extras/mod_extras.h \
    mcr/extras/private.h

# optional
CONFIG(x11) {
	DEFINES += MCR_USE_X
	LIBS += -lX11
	HEADERS += \
		mcr/standard/lnx/nusex.h
	SOURCES += \
		standard/lnx/nusex.c
}
CONFIG(wayland) {
	DEFINES += MCR_USE_WAYLAND
	HEADERS += \
		mcr/standard/lnx/nusewayland.h
	SOURCES += \
		standard/lnx/nusewayland.c
}
# extras default is to include(we like stuff)
CONFIG(noextras) {}
else {
	CONFIG += extras
	DEFINES += MCR_EXTRAS

	HEADERS += \
		mcr/extras/def.h \
		mcr/extras/script.h \
		mcr/extras/extras.h

	SOURCES += \
		extras/safe_string.c \
		extras/signal_extras.c \
		extras/script.c \
		extras/signal_option.c \
		extras/macro_set.c \
		extras/extras.c

	lnx {
		HEADERS += \
			mcr/extras/lnx/ndef.h \
			mcr/extras/lnx/nextras.h
		SOURCES += \
			extras/lnx/nsafestring.c \
			extras/lnx/nsignalextras.c \
			extras/lnx/nextras.c
	}
	win {
		HEADERS += \
			mcr/extras/win/ndef.h \
			mcr/extras/win/nextras.h
		SOURCES += \
			extras/win/nsafestring.c \
			extras/win/nsignalextras.c \
			extras/win/nextras.c
	}
	nativeless {
		HEADERS += \
			mcr/extras/nativeless/ndef.h \
			mcr/extras/nativeless/nextras.h
		SOURCES += \
			extras/nativeless/nsafestring.c \
			extras/nativeless/nsignalextras.c \
			extras/nativeless/nextras.c
	}
}

# native
win {
	HEADERS += \
		mcr/util/win/ndef.h \
		mcr/util/win/nutil.h \
		mcr/util/win/nwarn.h \
		mcr/standard/win/ndef.h \
		mcr/standard/win/nstandard.h \
		mcr/intercept/win/ndef.h \
		mcr/intercept/win/ngrabber.h \
		mcr/intercept/win/nintercept.h \
		mcr/win/nlibmacro.h

	SOURCES += \
		standard/win/nstandard.c \
		intercept/win/ngrabber.c \
		intercept/win/nintercept.c
}
lnx {
	HEADERS += \
		mcr/util/lnx/ndef.h \
		mcr/util/lnx/npriv.h \
		mcr/util/lnx/nutil.h \
		mcr/util/lnx/nwarn.h \
		mcr/standard/lnx/ndef.h \
		mcr/standard/lnx/ndevice.h \
		mcr/standard/lnx/nstandard.h \
		mcr/intercept/lnx/ndef.h \
		mcr/intercept/lnx/ngrabber.h \
		mcr/intercept/lnx/nintercept.h \
		mcr/lnx/nlibmacro.h

	SOURCES += \
		util/lnx/npriv.c \
		standard/lnx/ndevice.c \
		standard/lnx/nstandard.c \
		intercept/lnx/ngrabber.c \
		intercept/lnx/nintercept.c
}
mac {
	message(Apple not yet implemented)
}
nativeless {
	HEADERS += \
		mcr/util/nativeless/ndef.h \
		mcr/util/nativeless/nmcrstring.h \
		mcr/util/nativeless/nutil.h \
		mcr/util/nativeless/nwarn.h \
		mcr/standard/nativeless/ndef.h \
		mcr/standard/nativeless/nstandard.h \
		mcr/intercept/nativeless/ndef.h \
		mcr/nativeless/nlibmacro.h

	SOURCES += \
		util/nativeless/nmcrstring.c \
		standard/nativeless/nstandard.c \
		standard/nativeless/nstandardnames.c \
		standard/nativeless/nmods.c
}

lnx {
	documentation.path = /usr/local/libmacro/doc
	documentation.files = doxygen/html/*
	man.path = /usr/local/share/man
	man.files = doxygen/man/*
	lib.path = /usr/lib
	lib.files = *.a *.so *.so.1
	INSTALLS += lib
}
