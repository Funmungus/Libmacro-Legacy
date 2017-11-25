lnx {
	# libmacro.so and versions
	LOCAL_LIB = lib$${TARGET}.so
	OUTPUT_FILE = $${LOCAL_LIB}.$${MCR_VERSION}
	LOCAL_LIBDIR = "$${prefix}/lib"
	LINKOUTPUT = "$${LOCAL_LIBDIR}/$${OUTPUT_FILE}"
	LINKBASE = "$${LOCAL_LIBDIR}/$${LOCAL_LIB}"

	# Install modes
	QMAKE_INSTALL_FILE    = install -m 644 -p
	QMAKE_INSTALL_PROGRAM = install -m 755 -p
	# Symbolic link is not defined?
	isEmpty(QMAKE_SYMBOLIC_LINK) {
		QMAKE_SYMBOLIC_LINK = "ln -f -s"
	}

	# Install targets
	lib.depends = $${OUTPUT_FILE}
	lib.path = $${LOCAL_LIBDIR}
	lib.files = $${OUTPUT_FILE}
	# No version => full version
	links.path = $${LOCAL_LIBDIR}
	links.commands = $${QMAKE_SYMBOLIC_LINK} $${LINKOUTPUT} $${LINKBASE}
	# Maj version => full version
	linksmaj.path = $${LOCAL_LIBDIR}
	linksmaj.commands = $${QMAKE_SYMBOLIC_LINK} $${LINKOUTPUT} $${LINKBASE}.$${MCR_VER_MAJ}
	# Maj.Min version => full version
	linksmin.path = $${LOCAL_LIBDIR}
	linksmin.commands = $${QMAKE_SYMBOLIC_LINK} $${LINKOUTPUT} $${LINKBASE}.$${MCR_VER_MAJ}.$${MCR_VER_MIN}
	INSTALLS += lib links linksmaj linksmin
}
