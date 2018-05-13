#Libmacro definitions, no additional linking
include(libmacro.pri)
VERSION = $${MCR_VER}
#Always threaded and C/C++ 2011 standards
CONFIG += thread c++11 c11
#All headers exist in base directory
INCLUDEPATH += "$${PWD}"
#Style files with QtCreator
DISTFILES += libmacro.astylerc
#Find output dirs
win {
	CONFIG(debug, debug|release): DIR_POSTFIX = /debug
	else: DIR_POSTFIX = /release
} else {
	DIR_POSTFIX =
}
