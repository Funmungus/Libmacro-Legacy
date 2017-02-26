TEMPLATE = lib
TARGET = macro
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += thread c++11 c11
TARGET = $$qtLibraryTarget($$TARGET)

include(click.pri)
include(libmacro.pri)
VERSION = $${MCR_VERSION}

win {
	# our name is Libmacro, use it
	TARGET = lib$$TARGET
	# resolve imports on windows
	DEFINES += MCR_EXPORTS
	msvc {
		LIBS += -luser32
		contains(QMAKE_TARGET.arch, x86_64) {
			LIBS += -l$$qtLibraryTarget(libcrypto64MT)
		} else {
			LIBS += -l$$qtLibraryTarget(libcrypto32MT)
		}
	} else {
		LIBS += -l$$qtLibraryTarget(libcrypto-1_1)
	}
} else {
	# Unix does not require symbol exports
	DEFINES += MCR_STATIC
	unix:LIBS += -l$$qtLibraryTarget(crypto)
}

prefix=$${prefix}
isEmpty(prefix) {
    prefix=/usr
}

# util, signal, macro, standard, intercept, extras(below)
# platform-agnostic
SOURCES += $$files(util/*.c) \
	$$files(signal/*.c) \
	$$files(macro/*.c) \
	$$files(standard/*.c) \
	$$files(standard/*.cpp) \
	$$files(intercept/*.c) \
	$$files(*.c)

HEADERS = $$files(*.h, true)

OTHER_FILES = style \
	README.md

# Optional and native source files are explicit to view in QTCreator
# optional
# extras default is to include(we like stuff)
extras: SOURCES += $$files(extras/*.c)

# native
win {
	SOURCES += \
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

nativeless {
	SOURCES += \
		util/nativeless/nmcrstring.c \
		standard/nativeless/nstandard.c \
		standard/nativeless/nstandardnames.c \
		standard/nativeless/nmods.c
}

# Optional natives
extras {
	lnx {
		SOURCES += \
			extras/lnx/nsafestring.c \
			extras/lnx/nsignalextras.c \
			extras/lnx/nextras.c
	}
	win {
		SOURCES += \
			extras/win/nsafestring.c \
			extras/win/nsignalextras.c \
			extras/win/nextras.c
	}
	nativeless {
		SOURCES += \
			extras/nativeless/nsafestring.c \
			extras/nativeless/nsignalextras.c \
			extras/nativeless/nextras.c
	}
}

unix {
	LOCAL_LIB = lib$${TARGET}.so
	OUTPUT_FILE = $${LOCAL_LIB}.$${MCR_VERSION}
	LOCAL_LIBDIR = "$${prefix}/lib"
	LINKOUTPUT = "$${LOCAL_LIBDIR}/$${OUTPUT_FILE}"
	LINKBASE = "$${LOCAL_LIBDIR}/$${LOCAL_LIB}"
	lnx {
		QMAKE_INSTALL_FILE    = install -m 644 -p
		QMAKE_INSTALL_PROGRAM = install -m 755 -p
	}
	# symbolic link is not defined?
	isEmpty(QMAKE_SYMBOLIC_LINK) {
		QMAKE_SYMBOLIC_LINK = "ln -f -s"
	}

	documentation.depends = $${HEADERS}
	documentation.target = doxygen
	documentation.commands = doxygen $${PWD}/Doxyfile
	documentation_dir.target = "$${prefix}/libmacro/doc"
	documentation_dir.commands = $${QMAKE_MKDIR} $$documentation_dir.target
	install_documentation.depends = $$documentation.target $$documentation_dir.target
	install_documentation.path = $$documentation_dir.target
	install_documentation.files = doxygen/html/*
	install_documentation.commands = $${QMAKE_COPY_DIR} $$install_documentation.files $$install_documentation.path
	QMAKE_EXTRA_TARGETS += documentation documentation_dir install_documentation

	lib.depends = $${OUTPUT_FILE}
	lib.path = $${LOCAL_LIBDIR}
	lib.files = $${OUTPUT_FILE}
	# custom install links to fully versioned library
	links.path = $${LOCAL_LIBDIR}
	links.commands = $${QMAKE_SYMBOLIC_LINK} $${LINKOUTPUT} $${LINKBASE}
	linksmaj.path = $${LOCAL_LIBDIR}
	linksmaj.commands = $${QMAKE_SYMBOLIC_LINK} $${LINKOUTPUT} $${LINKBASE}.$${MCR_VER_MAJ}
	linksmin.path = $${LOCAL_LIBDIR}
	linksmin.commands = $${QMAKE_SYMBOLIC_LINK} $${LINKOUTPUT} $${LINKBASE}.$${MCR_VER_MAJ}.$${MCR_VER_MIN}
	INSTALLS += lib links linksmaj linksmin
}
