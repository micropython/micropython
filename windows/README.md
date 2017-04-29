This is experimental, community-supported Windows port of MicroPython.
It is based on Unix port, and expected to remain so.
The port requires additional testing, debugging, and patches. Please
consider to contribute.


Building on Debian/Ubuntu Linux system
---------------------------------------

    sudo apt-get install gcc-mingw-w64
    make CROSS_COMPILE=i686-w64-mingw32-

If for some reason the mingw-w64 crosscompiler is not available, you can try
mingw32 instead, but it comes with a really old gcc which may produce some
spurious errors (you may need to disable -Werror):

    sudo apt-get install mingw32 mingw32-binutils mingw32-runtime
    make CROSS_COMPILE=i586-mingw32msvc-


Bulding under Cygwin
--------------------

Install following packages using cygwin's setup.exe:

* mingw64-i686-gcc-core
* mingw64-x86_64-gcc-core
* make

Build using:

    make CROSS_COMPILE=i686-w64-mingw32-

Or for 64bit:

    make CROSS_COMPILE=x86_64-w64-mingw32-


Building using MS Visual Studio 2013 (or higher)
------------------------------------------------

In IDE, open `micropython.vcxproj` and build.

To build from command line:

    msbuild micropython.vcxproj


Running on Linux using Wine
---------------------------

The default build (MICROPY_USE_READLINE=1) uses extended Windows console
functions and thus should be run using `wineconsole` tool. Depending
on the Wine build configuration, you may also want to select the curses
backend which has the look&feel of a standard Unix console:

    wineconsole --backend=curses ./micropython.exe

For more info, see https://www.winehq.org/docs/wineusr-guide/cui-programs .

If built without line editing and history capabilities
(MICROPY_USE_READLINE=0), the resulting binary can be run using the standard
`wine` tool.
