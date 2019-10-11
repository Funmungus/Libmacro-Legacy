# Require "DEPLOYQT_TARGETS" variable defined if anything requires QT.

if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
	option(BUILD_PACKAGE "Also build installer or package requirements." ON)
else ()
	option(BUILD_PACKAGE "Also build installer or package requirements." OFF)
endif ()

# If using QT, deployed targets need to be packaged with QT dependencies.
# TODO QT deploy command for Linux
find_package(Qt5Core)
if (BUILD_PACKAGE AND Qt5Core_FOUND AND windows)
	# Get the deploy QT executable.
	get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
	get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
	if (windows)
		find_program(DEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
	endif (windows)
	# Deploy QT dependencies to qtimport for all DEPLOYQT_TARGETS.
	foreach (deployTarget IN LISTS DEPLOYQT_TARGETS)
		add_custom_command(TARGET ${deployTarget} POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E
				env "${DEPLOYQT_EXECUTABLE}"
					--verbose 0
					--no-compiler-runtime
					\"$<TARGET_FILE:${deployTarget}>\"
					--dir "${CMAKE_CURRENT_BINARY_DIR}/qtimport"
			COMMENT "Deploying Qt..."
		)
	endforeach ()
	# All QT dependencies will be in the qtimport directory.
	# Windows shared libraries go together with binaries, otherwise use lib.
	if (windows)
		install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qtimport/" DESTINATION bin)
	else ()
		install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/qtimport/" DESTINATION lib)
	endif (windows)
endif (BUILD_PACKAGE AND Qt5Core_FOUND AND windows)

# build a CPack driven installer package
if (BUILD_PACKAGE)
	include (InstallRequiredSystemLibraries)

	set (CPACK_RESOURCE_FILE_LICENSE
		 "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
	set (CPACK_RESOURCE_FILE_README
		  "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
	set (CPACK_RESOURCE_FILE_WELCOME
		"${CMAKE_CURRENT_SOURCE_DIR}/README.md")
	set (CPACK_PACKAGE_VERSION_MAJOR "${MCR_VER_MAJ}")
	set (CPACK_PACKAGE_VERSION_MINOR "${MCR_VER_MIN}")
	set (CPACK_PACKAGE_VERSION_PATCH "${GIT_REVISION}")
	set (CPACK_PACKAGE_VENDOR "New Paradigm Software")
	set (CPACK_PACKAGE_NAME "Libmacro")
	set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Libmacro is a multi-platform, extendable macro and hotkey C library.")
	set (CPACK_PACKAGE_INSTALL_DIRECTORY "Libmacro ${MCR_VER}")

	set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY "Libmacro ${MCR_VER}.${GIT_REVISION}")

	if (WIN32 AND NOT UNIX)
		# There is a bug in NSI that does not handle full unix paths properly. Make
		# sure there is at least one set of four (4) backlasshes.
	#	set (CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")

	#	CPACK_NSIS_MUI_ICON
	#	An icon filename. The name of a *.ico file used as the main icon for the generated install program.

	#	CPACK_NSIS_MUI_UNIICON
	#	An icon filename. The name of a *.ico file used as the main icon for the generated uninstall program.
		set (CPACK_NSIS_INSTALL_ROOT "C:\\\\Program Files\\\\New Paradigm Software")
		set (CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
#		set (CPACK_NSIS_MODIFY_PATH ON)
		set (CPACK_NSIS_DISPLAY_NAME "Libmacro")
		set (CPACK_NSIS_PACKAGE_NAME "Libmacro")
#		set (CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
		set (CPACK_NSIS_HELP_LINK "http:\\\\\\\\sites.google.com\\\\view\\\\newparadigmsoftware")
		set (CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\sites.google.com\\\\view\\\\newparadigmsoftware")
		set (CPACK_NSIS_CONTACT "newparadigmsoftware@gmail.com")

	endif (WIN32 AND NOT UNIX)

	include (CPack)
endif (BUILD_PACKAGE)


## for multi-config builds (e.g. msvc)
#foreach (OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
#	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
#	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/out)
#	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/out)
#	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/out)
#endforeach (OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

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
