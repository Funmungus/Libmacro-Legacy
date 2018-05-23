<center>

# Libmacro #

 A multi-platform, extendable macro and hotkey C library.
 Extra C++ capabilities are also available in the "extras" module.

## Supported compilers ##

 * GCC 4.8+
 * MinGW 4.9+
 * Visual Studio 2013+
 * Clang needs testing for version 3.3+

## Building source ##

 Required build tools:
 * qmake
   * Optionally built with QtCreator, instructions not included
 * All makefile generators and build tools for your environment and target

 Windows specific for the C++ "extras" module:
 * Static linking, this may be easier for users not familiar with qmake.
   * For SSL headers add qmake option "INCLUDEPATH+=<SSL-Dir>\include"
   * MinGW: "LIBS+=-L<SSL-Dir>\lib\MinGW"
   * Visual Studio: "LIBS+=-L<SSL-Dir>\lib\VC\static"
   * Static linking to SSL libraries must then inherit the SSL library license.
   * All programs that link to this static build will also have SSL definitions without linking to SSL libraries directly.
 * Dynamic linking
   * For SSL headers add the qmake option "INCLUDEPATH+=<SSL-Dir>\include"
   * To link SSL libraries add the qmake option "LIBS+=-L<SSL-Dir>\bin -L<SSL-Dir>\lib"
 * MinGW
   * MinGW only needs to link to the binary libcrypto dll in the bin/ folder of installed SSL libraries.  The SSL lib folder is optional.
 * Visual Studio
   * Visual Studio needs both the libcrypto(32|64)MT(d).lib and binary libcrypto dll.  The SSL lib folder is required.
   * Add both qmake options "LIBS+=-L<SSL-Dir>\lib\VC -L<SSL-Dir>\bin"

### Contributing ###
 Please follow style guides when it makes sense.  Astyle will be used for all
 style formatting, and manual formatting will not be kept.  The following are
 known style guidelines used in this project.
 * C and C++ style is Kernel Normal Form (KNF), which is similar to K&R.  Refer to astyle documentation for more information.  The astyle configuration file is [libmacro.astylerc](libmacro.astylerc).
 * C and C++ `#include` statements follow the Google style guide found at [https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes](https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes)
 * More of the Google style guide may be adopted in the future.

### Style source ###

 Astyle is required for correct formatting.  [style](style) is a helper script
 that wraps astyle with some options.  Run from any directory to format any C
 and C++ header and source files in that directory.  Run with `./style --help`
 for more information.

</center>
