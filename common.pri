#Libmacro definitions, no additional linking
include(libmacro.pri)
GIT_REVISION = $$system(git rev-list --count --first-parent master)
GIT_BRANCH = $$system(git branch --show-current)
DEFINES += GIT_REVISION=$${GIT_REVISION} GIT_BRANCH=$${GIT_BRANCH}
VERSION = $${MCR_VER}.$${GIT_REVISION}
#Always threaded and C/C++ 2011 standards
CONFIG += thread c++11 c11
#All headers exist in base directory
INCLUDEPATH += "$${PWD}"
#Style files with QtCreator
DISTFILES += libmacro.astylerc
#Find output dirs
windows {
	CONFIG(debug, debug|release): DIR_POSTFIX = /debug
	else: DIR_POSTFIX = /release
} else {
	DIR_POSTFIX =
}

QMAKE_TARGET = Libmacro
QMAKE_TARGET_COMPANY = New Paradigm Software
QMAKE_TARGET_DESCRIPTION = Libmacro, a multi-platform, extendable macro and hotkey library
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2013 Jonathan Pelletier, New Paradigm Software
QMAKE_TARGET_PRODUCT = Libmacro

!noextras:noqt|contains(DEFINES, MCR_NOQT) {
    # No STL, require QT
    stl_off:error(Error: QT and STL are not available for extra capabilities, because of the CONFIG options noqt and stl_off.)
    CONFIG += noqt
    DEFINES += MCR_NOQT
}
