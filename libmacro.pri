MCR_VER_MAJ = 0
MCR_VER_MIN = 1
MCR_VER_PAT = 1
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
    DEFINES += MCR_STATIC
}
else:CONFIG += nativeless

win: MCR_NATIVE_DIR = win
else:lnx: MCR_NATIVE_DIR = lnx
#else:mac: MCR_NATIVE_DIR = mac
else: MCR_NATIVE_DIR = nativeless
DEFINES += MCR_NATIVE_DIR=$${MCR_NATIVE_DIR}
