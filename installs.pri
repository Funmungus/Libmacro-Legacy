prefix = $${prefix}
isEmpty(prefix) {
	prefix = /usr
}

lnx {
	# libmacro.so and versions
	LOCAL_LIB = lib$${TARGET}.so
	LOCAL_LIB_VER = $${LOCAL_LIB}.$${MCR_VER}
	LOCAL_DIR = "$${prefix}/lib"
	LINKOUTPUT = "$${LOCAL_DIR}/$${LOCAL_LIB_VER}"
	LINKBASE = "$${LOCAL_DIR}/$${LOCAL_LIB}"

	# Install modes
	QMAKE_INSTALL_FILE = install -m 644 -p
	QMAKE_INSTALL_PROGRAM = install -m 755 -p
	# Symbolic link is not defined?
	isEmpty(QMAKE_SYMBOLIC_LINK) {
		QMAKE_SYMBOLIC_LINK = "ln -f -s"
	}

	# Install targets
	lib.depends = $${LOCAL_LIB_VER}
	lib.path = "$${LOCAL_DIR}"
	lib.files = $${LOCAL_LIB_VER}
	# No version => full version
	links.path = "$${LOCAL_DIR}"
	links.commands = $${QMAKE_SYMBOLIC_LINK} "$${LINKOUTPUT}" "$${LINKBASE}"
	# Maj version => full version
	linksmaj.path = "$${LOCAL_DIR}"
	linksmaj.commands = $${QMAKE_SYMBOLIC_LINK} "$${LINKOUTPUT}" "$${LINKBASE}.$${MCR_VER_MAJ}"
	# Maj.Min version => full version
	linksmin.path = "$${LOCAL_DIR}"
	linksmin.commands = $${QMAKE_SYMBOLIC_LINK} "$${LINKOUTPUT}" "$${LINKBASE}.$${MCR_VER_MAJ}.$${MCR_VER_MIN}"
	INSTALLS += lib links linksmaj linksmin
}
