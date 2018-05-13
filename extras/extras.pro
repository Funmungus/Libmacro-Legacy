TEMPLATE = lib

CONFIG += console
CONFIG -= app_bundle

include(../common.pri)

# Compile with STL?
nostl|stl_off {
	QT += core
	TARGET = $$qtLibraryTarget(macro-extras-nostl)
} else {
	CONFIG -= qt
	TARGET = $$qtLibraryTarget(macro-extras)
}

LIBS += "-L$${OUT_PWD}/../macro$${DIR_POSTFIX}/"
# Windows target and linkage
win {
	# Our name is Libmacro, use it
	TARGET = lib$$TARGET
	# import/export on windows
	DEFINES += MCR_EXTRAS_EXPORTS
	LIBS += -l$$qtLibraryTarget(libmacro)$${MCR_VER_MAJ}
#	msvc {
#		contains(QMAKE_TARGET.arch, x86_64) {
#			LIBS += -l$$qtLibraryTarget(libcrypto64MD)
#		} else {
#			LIBS += -l$$qtLibraryTarget(libcrypto32MD)
#		}
#	} else {
		# Mingw does not require an import library
		LIBS += -llibcrypto
#	}
} else {
	# Resolve Libmacro main module
	LIBS += -l$$qtLibraryTarget(macro) -l$$qtLibraryTarget(crypto)
}

include(../installs.pri)

HEADERS += $$files(../mcr/extras/*.h, true)

# platform-agnostic
SOURCES += $$files(*.cpp)

# platform-specific
# Change to $$files when stable, which effectively hides other
# platforms in QtCreator
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

