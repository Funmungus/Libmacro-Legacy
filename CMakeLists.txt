cmake_minimum_required(VERSION 3.5)

option(MCR_STATIC "Build as static libraries." OFF)
option(MCR_NOEXTRAS "Do not include extra functionality, or any C++.  Not yet possible on Windows because of threading." OFF)
option(MCR_NOQT "Do not include any QT functionality.  Implied by the MCR_NOEXTRAS option." OFF)
if (MCR_NOEXTRAS)
	set(MCR_NOQT ON)
endif (MCR_NOEXTRAS)
# TODO stl_off

# Defines MCR_VER, MCR_VER_MAJ, MCR_VER_MIN, MCR_DEBUG, MCR_PLATFORM
include(libmacro.cmake)
# Defines GIT_EXECUTABLE, GIT_REVISION, GIT_BRANCH
include(git.cmake)
# Create documentation: build + install targets if BUILD_DOC
include(doxygen.cmake)

project(libmacro VERSION ${MCR_VER}.${GIT_REVISION} LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT MCR_NOQT)
	set(CMAKE_AUTOUIC ON)
	set(CMAKE_AUTOMOC ON)
	set(CMAKE_AUTORCC ON)
endif (NOT MCR_NOQT)

# Openssl use multithreaded
set(OPENSSL_MSVC_STATIC_RT TRUE)
# Openssl static
if (MCR_STATIC)
	set(OPENSSL_USE_STATIC_LIBS TRUE)
endif (MCR_STATIC)

# TODO Thread optional with event loop
find_package(Threads REQUIRED)
find_package(OpenSSL)
find_package(Qt5Core)
find_package(Qt5Test)

if (OPENSSL_FOUND)
	include_directories(${OPENSSL_INCLUDE_DIR})
endif (OPENSSL_FOUND)
if (Qt5Core_FOUND)
	include_directories(${Qt5Core_INCLUDE_DIRS})
endif (Qt5Core_FOUND)
if (Qt5Test_FOUND)
	include_directories(${Qt5Test_INCLUDE_DIRS})
endif (Qt5Test_FOUND)


#include_directories(${CMAKE_SOURCE_DIR})

# Lib sources
file(GLOB LIBMACRO_SRC
	src/util/*.c
	src/signal/*.c
	src/macro/*.c
	src/standard/*.c
	src/intercept/*.c
	src/*.c
	src/*/${MCR_PLATFORM}/*.c
	src/extras/*.cpp
	src/extras/${MCR_PLATFORM}/*.cpp
	src/extras/ssl/p_safe_string.cpp
	)
file(GLOB_RECURSE HEADERS mcr/*.h)

# Windows requires C++ threads
if (windows)
	# Mingw and VC <= 2017 do not have C threads
	list(APPEND LIBMACRO_SRC src/util/cppthread.cpp)
	# TODO what is stl_off in CMAKE?
	if (stl_off)
		message(FATAL_ERROR
			"Error: stl_off is configured, but threads require stl on Windows."
			)
	endif (stl_off)
endif (windows)

# Remove extras module
if (MCR_NOEXTRAS)
	list(FILTER LIBMACRO_SRC EXCLUDE REGEX
		src/extras/
		)
# Remove the classes that use QT
elseif (MCR_NOQT)
	list(FILTER LIBMACRO_SRC EXCLUDE REGEX
		signal_functions.cpp|trigger_functions.cpp
		)
endif (MCR_NOEXTRAS)

# Library
if (MCR_STATIC)
	add_library(libmacro STATIC ${LIBMACRO_SRC} ${HEADERS})
else ()
	add_library(libmacro SHARED ${LIBMACRO_SRC} ${HEADERS})
endif (MCR_STATIC)
set_target_properties(libmacro PROPERTIES SOVERSION ${MCR_VER})

# Library link
if (NOT MCR_NOEXTRAS)
	target_link_libraries(libmacro OpenSSL::Crypto)
	if (NOT MCR_NOQT)
		target_link_libraries(libmacro ${Qt5Core_LIBRARIES})
	endif (NOT MCR_NOQT)
endif (NOT MCR_NOEXTRAS)
# Be aware msvc may require linking to Advapi32 and user32
#if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
#	target_link_libraries(libmacro Advapi32 user32)
#endif()

# Library defines
target_compile_definitions(libmacro PRIVATE
	LIBMACRO_LIBRARY
	-DVERSION=${MCR_VER}.${GIT_REVISION}
	-DGIT_BRANCH=${GIT_BRANCH}
	-DGIT_REVISION=${GIT_REVISION}
	)

# Test
if (Qt5Test_FOUND AND NOT MCR_NOQT)
	file(GLOB_RECURSE TEST_SRC test/*.h test/*.cpp)
	add_executable(tst_libmacro ${TEST_SRC})
	add_dependencies(tst_libmacro libmacro)
	target_link_libraries(tst_libmacro libmacro ${Qt5Test_LIBRARIES})
endif (Qt5Test_FOUND AND NOT MCR_NOQT)

add_custom_target(DISTFILES SOURCES
	libmacro.astylerc
	README.md
	style
	)

if (windows)
	install(TARGETS libmacro
		LIBRARY DESTINATION bin
		)
	install(TARGETS libmacro
		ARCHIVE DESTINATION lib
		)
else ()
	install(TARGETS libmacro DESTINATION lib)
endif (windows)
install(DIRECTORY mcr DESTINATION include)

# Option to build tests
include(coverage.cmake)

# Platform variables and MCR_DEBUG required from libmacro.cmake
# Requires DEPLOYQT_TARGETS
set(DEPLOYQT_TARGETS libmacro)
include(cpack.cmake)
