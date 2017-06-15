TEMPLATE = subdirs
SUBDIRS = macro

!noextras {
	SUBDIRS += extras
	extras.depends = macro
}

OTHER_FILES = style \
	README.md

DISTFILES += libmacro.astylerc

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

	# Docs targets
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
