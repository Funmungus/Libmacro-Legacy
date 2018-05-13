MCR_VER_MAJ = 0
MCR_VER_MIN = 1
MCR_VER_PAT = 4
MCR_VER = $${MCR_VER_MAJ}.$${MCR_VER_MIN}.$${MCR_VER_PAT}

DEFINES += MCR_VER=$${MCR_VER} MCR_VER_MAJ=$${MCR_VER_MAJ} \
    MCR_VER_MIN=$${MCR_VER_MIN} MCR_VER_PAT=$${MCR_VER_PAT}

CONFIG(debug,debug|release): DEFINES += DEBUG

windows: CONFIG += win
else:unix {
	macx: CONFIG += mac
	else: CONFIG += lnx
    # Unix/ELF will not use callspec for linking
    DEFINES += MCR_STATIC
}
else: CONFIG += none

win: MCR_PLATFORM = win
lnx: MCR_PLATFORM = lnx
mac: MCR_PLATFORM = mac
none: MCR_PLATFORM = none
DEFINES += MCR_PLATFORM=$${MCR_PLATFORM}

nostl|stl_off: DEFINES += MCR_NOSTL
