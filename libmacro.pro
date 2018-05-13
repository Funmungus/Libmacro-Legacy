TEMPLATE = subdirs
SUBDIRS += macro
!noextras:SUBDIRS += extras
SUBDIRS += test
CONFIG += ordered

OTHER_FILES = style \
	README.md

DISTFILES += libmacro.astylerc

lnx {
	# Docs targets
	documentation.depends = $${HEADERS}
	documentation.target = doxygen
	documentation.commands = doxygen "$${PWD}/Doxyfile"
	documentation_dir.target = "$${prefix}/libmacro/doc"
	documentation_dir.commands = $${QMAKE_MKDIR} "$$documentation_dir.target"
	install_documentation.depends = "$$documentation.target" "$$documentation_dir.target"
	install_documentation.path = "$$documentation_dir.target"
	install_documentation.files = doxygen/html/*
	install_documentation.commands = $${QMAKE_COPY_DIR} $$install_documentation.files "$$install_documentation.path"
	QMAKE_EXTRA_TARGETS += documentation documentation_dir install_documentation
}
