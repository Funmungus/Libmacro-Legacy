TEMPLATE = lib
CONFIG += console
CONFIG -= app_bundle
CONFIG += thread c++11 c11

include(../libmacro.pri)
include(../common.pri)
VERSION = $${MCR_VERSION}

nostl|stl_off {
	DEFINES += nostl
	QT += core
	TARGET = $$qtLibraryTarget(macro-extras-nostl)
} else {
	CONFIG -= qt
	TARGET = $$qtLibraryTarget(macro-extras)
}

win {
	# our name is Libmacro, use it
	TARGET = lib$$TARGET
	# resolve imports on windows
	# linkage callspec
	# TODO: Confirm callspec required for mingw
	DEFINES += MCR_EXPORTS
	# Resolve Libmacro main module
	CONFIG(debug,debug|release):LIBS += -L$$OUT_PWD/../macro/debug/
	else:LIBS += -L$$OUT_PWD/../macro/release/
	msvc {
		LIBS += -l$$qtLibraryTarget(libmacro)
		LIBS += -luser32
		contains(QMAKE_TARGET.arch, x86_64) {
			LIBS += -l$$qtLibraryTarget(libcrypto64MT)
		} else {
			LIBS += -l$$qtLibraryTarget(libcrypto32MT)
		}
	} else {
		LIBS += -l$$qtLibraryTarget(libmacro)$${MCR_VER_MAJ}
		LIBS += -llibcrypto
	}
} else {
	# Resolve Libmacro main module
	LIBS += -L$$OUT_PWD/../macro/
	LIBS += -l$$qtLibraryTarget(macro)
	unix:LIBS += -l$$qtLibraryTarget(crypto)
}

prefix=$${prefix}
isEmpty(prefix) {
    prefix=/usr
}

HEADERS += $$files(../mcr/extras/*.h, true)

# platform-agnostic
SOURCES += $$files(*.cpp)

# platform-specific
lnx {
	SOURCES += \
		lnx/nsignal_extras.cpp \
		lnx/nextras.cpp
}
win {
	SOURCES += \
		win/nsignal_extras.cpp \
		win/nextras.cpp
}
none {
	SOURCES += \
		none/nsignal_extras.cpp \
		none/nextras.cpp
}

# using ssl libs
SOURCES += \
	ssl/nsafe_string.cpp
