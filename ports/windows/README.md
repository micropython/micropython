This is the experimental, community-supported Windows port of MicroPython.
It is based on Unix port, and expected to remain so.
The port requires additional testing, debugging, and patches. Please
consider to contribute.

All gcc-based builds use the gcc compiler from [Mingw-w64](https://www.mingw-w64.org/),
which is the advancement of the original mingw project. The latter is
getting obsolete and is not actively supported by MicroPython.

Build instruction assume you're in the ports/windows directory.

Building on Debian/Ubuntu Linux system
---------------------------------------

    sudo apt-get install python3 build-essential gcc-mingw-w64
    make -C ../../mpy-cross
    make CROSS_COMPILE=i686-w64-mingw32-


Building under Cygwin
---------------------

Install Cygwin, then install following packages using Cygwin's setup.exe:

* mingw64-i686-gcc-core
* mingw64-x86_64-gcc-core
* make
* python3

Build using:

    make -C ../../mpy-cross CROSS_COMPILE=i686-w64-mingw32-
    make CROSS_COMPILE=i686-w64-mingw32-

Or for 64bit:

    make -C ../../mpy-cross CROSS_COMPILE=x86_64-w64-mingw32-
    make CROSS_COMPILE=x86_64-w64-mingw32-


Building under MSYS2
--------------------

Install MSYS2 from http://repo.msys2.org/distrib, start the msys2.exe shell and
install the build tools:

    pacman -Syuu
    pacman -S make mingw-w64-x86_64-gcc pkg-config python3

Start the mingw64.exe shell and build:

    make -C ../../mpy-cross STRIP=echo SIZE=echo
    make


Building using MS Visual Studio 2013 (or higher)
------------------------------------------------

Install Python. There are several ways to do this, for example: download and install the
latest Python 3 release from https://www.python.org/downloads/windows or from
https://docs.conda.io/en/latest/miniconda.html,
or open the Microsoft Store app and search for Python and install it.

Install Visual Studio and the C++ toolset (for recent versions: install
the free Visual Studio Community edition and the *Desktop development with C++* workload).

In the IDE, open `micropython-cross.vcxproj` and `micropython.vcxproj` and build.

To build from the command line:

    msbuild ../../mpy-cross/mpy-cross.vcxproj
    msbuild micropython.vcxproj

__Variants__

The msvc port supports variants (like the unix and windows mingw ports) and the one which gets built is
controlled by the `PyVariant` msbuild property. It defaults to `standard`.
The other variants can be built like:

    msbuild micropython.vcxproj /p:PyVariant=dev

Or by adding a file [Directory.build.props](https://docs.microsoft.com/en-us/visualstudio/msbuild/customize-your-build#directorybuildprops-and-directorybuildtargets) in this directory or a parent directory:

```xml
<Project>
  <PropertyGroup>
    <PyVariant>dev</PyVariant>
  </PropertyGroup>
</Project>
```

See [paths.props](msvc/paths.props) for other related variables like build and variant directories.

__Stack usage__

The msvc compiler is quite stack-hungry which might result in a "maximum recursion depth exceeded"
RuntimeError for code with lots of nested function calls.
There are several ways to deal with this:
- increase the threshold used for detection by altering the argument to `mp_stack_set_limit` in `ports/unix/main.c`
- disable detection all together by setting `MICROPY_STACK_CHECK` to "0" in `ports/windows/mpconfigport.h`
- disable the /GL compiler flag by setting `WholeProgramOptimization` to "false"

See [issue 2927](https://github.com/micropython/micropython/issues/2927) for more information.


Running the tests
-----------------

This is similar for all ports:

    cd ../../tests
    python ./run-tests.py

Though when running on Cygwin and using Cygwin's Python installation you'll need:

    python3 ./run-tests.py

Depending on the combination of platform and Python version used it might be
needed to first set the MICROPY_MICROPYTHON environment variable to
the full path of micropython.exe.


Running on Linux using Wine
---------------------------

The default build (MICROPY_USE_READLINE=1) uses extended Windows console
functions and thus should be ran using the `wineconsole` tool. Depending
on the Wine build configuration, you may also want to select the curses
backend which has the look&feel of a standard Unix console:

    wineconsole --backend=curses ./micropython.exe

For more info, see https://www.winehq.org/docs/wineusr-guide/cui-programs .

If built without line editing and history capabilities
(MICROPY_USE_READLINE=0), the resulting binary can be run using the standard
`wine` tool.
