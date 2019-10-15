# TODO cache variables or PARENT_SCOPE?
set(MCR_VER_MAJ 0)
set(MCR_VER_MIN 2)
set(MCR_VER ${MCR_VER_MAJ}.${MCR_VER_MIN})
add_definitions(-DMCR_VER=${MCR_VER} -DMCR_VER_MAJ=${MCR_VER_MAJ}
	-DMCR_VER_MIN=${MCR_VER_MIN} -DMCR_VER=${MCR_VER})

if (CMAKE_BUILD_TYPE MATCHES "^[Dd]ebug")
	set(MCR_DEBUG true)
	# TODO: Should MCR_DEBUG not be defined outside of building Libmacro?
	add_definitions(-DMCR_DEBUG=1)
else ()
	unset(MCR_DEBUG)
endif ()

# LINUX was not set by cmake
if (UNIX AND NOT APPLE)
    set (LINUX true)
endif (UNIX AND NOT APPLE)
if (WIN32)
	set(MCR_PLATFORM windows)
elseif (LINUX)
	set(MCR_PLATFORM linux)
elseif (APPLE)
	set(MCR_PLATFORM apple)
else ()
	set(MCR_PLATFORM none)
endif (WIN32)
set(${MCR_PLATFORM} true)
add_definitions(-DMCR_PLATFORM=${MCR_PLATFORM})
