TEMPLATE = lib
TARGET = $$qtLibraryTarget(macro)
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += thread c++11 c11

include(../libmacro.pri)
include(../common.pri)
VERSION = $${MCR_VERSION}

win {
	# our name is Libmacro, use it
	TARGET = lib$$TARGET
	# resolve imports on windows
	# linkage callspec
	# TODO: Confirm callspec required for mingw
	DEFINES += MCR_EXPORTS
	msvc {
		LIBS += -luser32
	} else:*-g++* {
		# mingw does not have std or pthread
		SOURCES += util/cppthread.cpp
	}
}

prefix=$${prefix}
isEmpty(prefix) {
    prefix=/usr
}

HEADERS += $$files(../mcr/*.h, false) \
	$$files(../mcr/util/*.h, true) \
	$$files(../mcr/signal/*.h, true) \
	$$files(../mcr/macro/*.h, true) \
	$$files(../mcr/standard/*.h, true) \
	$$files(../mcr/intercept/*.h, true) \

# util, signal, macro, standard, intercept
# platform-agnostic
SOURCES += $$files(util/*.c) \
	$$files(signal/*.c) \
	$$files(macro/*.c) \
	$$files(standard/*.c) \
	$$files(intercept/*.c) \
	$$files(*.c)

# platform-specific
win {
	SOURCES += \
		util/win/npriv.c \
		standard/win/nstandard.c \
		intercept/win/ngrabber.c \
		intercept/win/nintercept.c
}

lnx {
	SOURCES += \
		util/lnx/npriv.c \
		standard/lnx/ndevice.c \
		standard/lnx/nstandard.c \
		intercept/lnx/ngrabber.c \
		intercept/lnx/nintercept.c
}

none {
	SOURCES += \
		util/none/npriv.c \
		util/none/nmcrstring.c \
		standard/none/nstandard.c \
		standard/none/nstandard_names.c \
		standard/none/nmods.c
}