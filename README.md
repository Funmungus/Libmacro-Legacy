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

 Windows specific for extras and C++ module:
 * MinGW
   * MinGW only needs to link to libcrypto-1_1.dll in the bin/ folder of installed SSL libraries.  If given crypto library is named differently, extras.pro must be modified with a text editor.
   * Add qmake option "LIBS+=-L<SSL-Dir>\bin"
 * Visual Studio
   * Visual Studio needs both the libcrypto[32/64]MT(d).lib and libcrypto-1_1.dll.  If the library is named differently, extras.pro must be modified with a text editor.
   * Add both qmake options "LIBS+=-L<SSL-Dir>\lib\VC -L<SSL-Dir\bin"
 * Static, static linking may be easier for users not familiar with qmake
   * MinGW: "LIBS+=-L<SSL-Dir>\lib\MinGW"
   * Visual Studio: "LIBS+=-L<SSL-Dir>\lib\VC\static"
   * Static linking to SSL libraries must then inherit the SSL library license.
   * All programs that link to this static build will also have SSL definitions without linking to SSL libraries directly.

### Style source ###

 Libmacro has a common style to help collaboration.  The program `astyle` is
 required.  To style source execute `./style` in the project base directory.
 `./style` is a bash script, and may also be executed from any directory to
 format any h, hpp, c, and cpp files in that directory.

</center>
