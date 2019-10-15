# Require "DEPLOYQT_TARGET" variable defined for all targets deployed.
if ("${DEPLOYQT_TARGET}" STREQUAL "")
	return()
endif ()

# QMLDIR may be defined as a directory for the deployqt --qmldir option.
if ("${QMLDIR}" STREQUAL "")
	set(QMLDIR_COMMAND "")
else ()
	set(QMLDIR_COMMAND "--qmldir \"${QMLDIR}\"")
endif ()

# If using QT, deployed targets need to be packaged with QT dependencies.
if (MCR_NOQT)
	return()
endif (MCR_NOQT)

find_package(Qt5Core REQUIRED)
# TODO QT deploy command for Linux
if (BUILD_PACKAGE AND windows)
	# Get the deploy QT executable.
	get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
	get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
	if (windows)
		find_program(DEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
	endif (windows)
	# Deploy QT dependencies to qtimport for DEPLOYQT_TARGET.
	add_custom_command(TARGET ${DEPLOYQT_TARGET} POST_BUILD
		COMMAND "${CMAKE_COMMAND}" -E
		    env "${DEPLOYQT_EXECUTABLE}"
			    --verbose 0
				--no-compiler-runtime
				\"$<TARGET_FILE:${DEPLOYQT_TARGET}>\"
				--dir "${CMAKE_CURRENT_BINARY_DIR}/qtimport"
				${QMLDIR_COMMAND}
		COMMENT "Deploying Qt..."
	)
	# All QT dependencies will be in the qtimport directory.
	# Windows shared libraries go together with binaries, otherwise use lib.
	if (windows)
		install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qtimport/" DESTINATION bin)
	else ()
		install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qtimport/" DESTINATION lib)
	endif (windows)
endif (BUILD_PACKAGE AND windows)

#add_custom_command(
#	COMMAND "${CMAKE_COMMAND}" -E
#		env "${DEPLOYQT_EXECUTABLE}"
#			--dry-run
#			--no-compiler-runtime
#			--list mapping
#			\"$<TARGET_FILE:${deployTarget}>\"
#	COMMENT "Reading deploy mapping..."
#	OUTPUT ${deployTarget}_qtimport.txt
#	OUTPUT_STRIP_TRAILING_WHITESPACE
#)
