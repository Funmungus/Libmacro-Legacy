# Build a CPack driven installer package
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

	if (WIN32)
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

	endif (WIN32)

	include (CPack)
endif (BUILD_PACKAGE)


## for multi-config builds (e.g. msvc)
#foreach (OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
#	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
#	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/out)
#	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/out)
#	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR}/out)
#endforeach (OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
