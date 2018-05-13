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

### Style source ###

 Libmacro has a common style to help collaboration.  The program `astyle` is
 required.  To style source execute `./style` in the project base directory.
 `./style` is a bash script, and may also be executed from any directory to
 format any h, hpp, c, and cpp files in that directory.

</center>
