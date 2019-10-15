MCR_VER_MAJ = 0
MCR_VER_MIN = 2
MCR_VER = $${MCR_VER_MAJ}.$${MCR_VER_MIN}

DEFINES += MCR_VER=$${MCR_VER} MCR_VER_MAJ=$${MCR_VER_MAJ} \
	MCR_VER_MIN=$${MCR_VER_MIN}

CONFIG(debug,debug|release): DEFINES += MCR_DEBUG

windows: MCR_PLATFORM = windows
else:unix {
	macx: MCR_PLATFORM = apple
	linux: MCR_PLATFORM = linux
}
else: MCR_PLATFORM = none
# Define static linkage
static|staticlib: DEFINES += MCR_STATIC

CONFIG += $${MCR_PLATFORM}
DEFINES += MCR_PLATFORM=$${MCR_PLATFORM}
