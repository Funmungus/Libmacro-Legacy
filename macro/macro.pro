TEMPLATE = lib
TARGET = $$qtLibraryTarget(macro)

CONFIG += console
CONFIG -= app_bundle qt

include(../common.pri)
include(../installs.pri)

# Windows target and linkage
win {
	# Our name is Libmacro, use it
	TARGET = lib$$TARGET
	# import/export on windows
	DEFINES += MCR_EXPORTS
	# win32 libs not implicitly linked with VC
	msvc: LIBS += -lAdvapi32 -luser32
	# Mingw and VC < 2017 do not have C threads
	SOURCES += util/cppthread.cpp
}

# util, signal, macro, standard, intercept
# exclude extras headers
HEADERS += $$files(../mcr/*.h) \
	$$files(../mcr/lnx/*.h, true) \
	$$files(../mcr/win/*.h, true) \
	$$files(../mcr/none/*.h, true) \
	$$files(../mcr/util/*.h, true) \
	$$files(../mcr/signal/*.h, true) \
	$$files(../mcr/macro/*.h, true) \
	$$files(../mcr/standard/*.h, true) \
	$$files(../mcr/intercept/*.h, true) \

# platform-agnostic
SOURCES += $$files(util/*.c) \
	$$files(signal/*.c) \
	$$files(macro/*.c) \
	$$files(standard/*.c) \
	$$files(intercept/*.c) \
	$$files(*.c)

# platform-specific
# Change to $$files when stable, which effectively hides other
# platforms in QtCreator
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

