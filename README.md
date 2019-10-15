# Libmacro #

 Libmacro is a multi-platform, extendable macro and hotkey C library.  C++
 extras are also available in the "extras" module, included by default.

# License #

 Libmacro uses the LGPL v2.1 license.  Please refer to the [LICENSE](./LICENSE)
 or [lgpl-2.1.txt](./lgpl-2.1.txt) files for more information.

## Supported compilers ##

 * GCC 4.8+
 * Visual Studio 2015+
 * Clang needs testing for version 3.3+

## Supported cpack installer compilers ##

 * NSIS
 * More research required

## Building source ##

 Required build tools:

 * cmake or qmake
   * Building with qmake is still supported, but deprecated.  Please contact
   the creator if you have trouble compiling with qmake.
   * Optionally built with QtCreator or other IDE using cmake generators.
   Instructions not included.
   * Given build configuration \<BUILD_TYPE\> cmake and cpack may require the
   same command-line switch.
     * cmake: `cmake --config <BUILD_TYPE>`
     * cpack: `cpack -C <BUILD_TYPE>`
 * Qt5, if QT extras are included
   * CMAKE_PREFIX_PATH must include the base directory of QT libraries for
   your platform, QT version, and compiler.
     * Example for windows, QT 5.13.1, using MSVC 2017:
     "<QT directory>/5.13.1/msvc2017_64"
 * All makefile generators and build tools for your environment and target
 * cmake modules
   * Threads: Always required until further notice.
   * Git: Required for versioning
   * OpenSSL: Required for extras
   * Qt5Core: Required for extras
   * Qt5Test: Required for testing

 Supported optional cmake arguments

 * MCR_STATIC: Build as static libraries.
 * MCR_NOEXTRAS: Do not include extra functionality, or any C++.  Not yet
 possible on Windows because of threading.
 * MCR_NOQT: Do not include any QT functionality. Implied by the MCR_NOEXTRAS
 option.
 * BUILD_DOC: Also build doxygen documentation.
 * OPENSSL_ROOT_DIR: Locate the root directory of OpenSSL.
 * CMAKE_BUILD_TYPE: Debug or Release
 * CMAKE_CXX_COMPILER: GCC, Visual Compiler, or Clange executable
 * CMAKE_C_COMPILER: GCC, Visual Compiler, or Clange executable
 * CMAKE_INSTALL_PREFIX: Install location
 * CMAKE_PREFIX_PATH: Custom locations for headers, libraries, and QT version
 base directories.

 Supported optional qmake arguments

 * "CONFIG+=static": Static linking
   * Static builds must inherit both the Libmacro and SSL licences.  This may
   require a license statement in source code or EULA.
 * "INCLUDEPATH+=<SSL-Dir>/include": Locate SSL library header files.
 * "LIBS+=-L<SSL-Dir>/lib": Locate SSL libraries
 * "LIBS+=-L<SSL-Dir>/lib/VC/static": Locate static SSL libraries for
 Visual Studio.
   * All programs that link to this static build will also have SSL definitions
   without linking to SSL libraries directly.

### Contributing ###

 Please follow style guides when it makes sense.  Astyle will be used for all
 style formatting, and manual formatting will not be kept.  The following are
 known style guidelines used in this project.
 * C and C++ style is Kernel Normal Form (KNF), which is similar to K&R.  Refer
 to Astyle documentation for more information.  The Astyle configuration file
 is [libmacro.astylerc](libmacro.astylerc).
 * C and C++ `#include` statements follow the Google style guide found at
 [https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes](https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes)
 * More of the Google style guide may be adopted in the future.

### Style source ###

 Astyle is required for correct formatting.  [style](./style) is a helper script
 that wraps Astyle with some options.  Run from any directory to format any C
 and C++ header and source files in that directory.  Run with `./style --help`
 for more information.
