TARGET = $$qtLibraryTarget(macro)
TEMPLATE = lib

CONFIG += console
CONFIG -= app_bundle

include(common.pri)

# Windows target and linkage
windows {
	# Our name is Libmacro, use it
	TARGET = lib$$TARGET
	# win32 libs not implicitly linked with VC
	msvc: LIBS += -lAdvapi32 -luser32
	# Mingw and VC < 2017 do not have C threads
	SOURCES += src/util/cppthread.cpp
	stl_off:error(Error: stl_off is configured, but threads require stl on Windows.)
}
include(deployment.pri)

# import/export or function visibility.  MCR_STATIC will override this.
DEFINES += LIBMACRO_LIBRARY

HEADERS += $$files(mcr/*.h, true)

# platform-agnostic
SOURCES += $$files(src/util/*.c) \
	$$files(src/signal/*.c) \
	$$files(src/macro/*.c) \
	$$files(src/standard/*.c) \
	$$files(src/intercept/*.c) \
	$$files(src/*.c)

# platform-specific
# Change to $$files when stable, which effectively hides other
# platforms in QtCreator
#SOURCES += $$files(src/*/$${MCR_PLATFORM}/*.c)
windows {
	SOURCES += \
		src/util/windows/p_priv.c \
		src/standard/windows/p_standard.c \
		src/intercept/windows/p_grabber.c \
		src/intercept/windows/p_intercept.c
}
linux {
	SOURCES += \
		src/util/linux/p_priv.c \
		src/standard/linux/p_device.c \
		src/standard/linux/p_standard.c \
		src/intercept/linux/p_grabber.c \
		src/intercept/linux/p_intercept.c
}
none {
	SOURCES += \
		src/util/none/p_priv.c \
		src/util/none/p_mcrstring.c \
		src/standard/none/p_standard.c \
		src/standard/none/p_standard_names.c \
		src/standard/none/p_modifier.c
}

# Resolve needs for extras
noextras {
    # Without extras QT and STL are not used
    CONFIG -= qt
} else {
    noqt {
        # With STL
        CONFIG -= qt
    } else {
        QT -= gui
    }
    # Libs for extras, SSL
    windows {
        static|staticlib {
            CONFIG(debug, debug|release) {
                LIBS += -llibcrypto64MTd.lib
            } else {
                LIBS += -llibcrypto64MT.lib
            }
        } else {
            LIBS += -llibcrypto
        }
    } else {
        LIBS += -l$$qtLibraryTarget(crypto)
    }

    # Extra sources
	SOURCES += $$files(src/extras/*.cpp)
    # platform-specific
	# $$files hides platforms in QtCreator
	#SOURCES += $$files("src/extras/$${MCR_PLATFORM}/*.cpp")
	linux {
		SOURCES += \
			src/extras/linux/p_signal_extras.cpp \
			src/extras/linux/p_extras.cpp
	}
	windows {
		SOURCES += \
			src/extras/windows/p_signal_extras.cpp \
			src/extras/windows/p_extras.cpp
	}
	none {
		SOURCES += \
			src/extras/none/p_signal_extras.cpp \
			src/extras/none/p_extras.cpp
	}

    # using ssl libs
    SOURCES += \
		src/extras/ssl/p_safe_string.cpp

	# noqt: Do not include QT extras
	noqt {
		SOURCES -= src/extras/signal_functions.cpp \
			src/extras/trigger_functions.cpp
	}
}
