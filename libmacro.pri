MCR_VER_MAJ = 0
MCR_VER_MIN = 1
MCR_VER_PAT = 2
MCR_VERSION = $${MCR_VER_MAJ}.$${MCR_VER_MIN}.$${MCR_VER_PAT}

DEFINES += MCR_VERSION=$${MCR_VERSION} \
    MCR_VER_MAJ=$${MCR_VER_MAJ} MCR_VER_MIN=$${MCR_VER_MIN}

CONFIG(debug,debug|release):DEFINES += DEBUG

CONFIG(noextras){}
else {
    CONFIG += extras
    DEFINES += MCR_EXTRAS
}

win32|win64:CONFIG += win
else:unix {
    macx:CONFIG += mac
    else:CONFIG += lnx
    # Unix/ELF will not use callspec for linking
    DEFINES += MCR_STATIC
}
else:CONFIG += none

win: MCR_PLATFORM_DIR = win
else:lnx: MCR_PLATFORM_DIR = lnx
#else:mac: MCR_PLATFORM_DIR = mac
else: MCR_PLATFORM_DIR = none
DEFINES += MCR_PLATFORM_DIR=$${MCR_PLATFORM_DIR}
