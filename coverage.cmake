if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
	option(BUILD_TESTING "Also build tests." ON)
else ()
	option(BUILD_TESTING "Also build tests." OFF)
endif ()
if (BUILD_TESTING)
	enable_testing()
	include(CTest)

	if (TARGET tst_libmacro)
		add_test(tst_libmacro tst_libmacro)
	endif (TARGET tst_libmacro)

	set(CTEST_PROJECT_NAME "Libmacro")
	set(CTEST_SOURCE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
	set(CTEST_BINARY_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif (BUILD_TESTING)
