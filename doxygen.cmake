if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
	option(BUILD_DOC "Build documentation" ON)
else ()
	option(BUILD_DOC "Build documentation" OFF)
endif ()

if (BUILD_DOC)
	find_package(Doxygen)
	if (Doxygen_FOUND)
		# Files used by doxygen
		set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
		set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

		configure_file("${DOXYGEN_IN}" "${DOXYGEN_OUT}" @ONLY)

		# note the option ALL which allows to build the docs together with the application
		add_custom_target(doxygen ALL
			COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_OUT}"
			WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
			COMMENT "Generating API documentation with Doxygen"
			VERBATIM
			)
		install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doxygen/html/"
			DESTINATION doc
			)
	else (Doxygen_FOUND)
		message("Doxygen is needed to generate the doxygen documentation.")
	endif (Doxygen_FOUND)
endif (BUILD_DOC)
