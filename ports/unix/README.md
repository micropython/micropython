The Unix version
----------------

The "unix" port requires a standard Unix-like environment with gcc and GNU
make. This includes Linux, BSD, macOS, and Windows Subsystem for Linux. The
x86 and x64 architectures are supported (i.e. x86 32- and 64-bit), as well as
ARM and MIPS. Making a full-featured port to another architecture requires
writing some assembly code for the exception handling and garbage collection.
Alternatively, a fallback implementation based on setjmp/longjmp can be used.

To build (see section below for required dependencies):

    $ cd ports/unix
    $ make submodules
    $ make

Then to give it a try:

    $ ./build-standard/micropython
    >>> list(5 * x + y for x in range(10) for y in [4, 2, 1])

Use `CTRL-D` (i.e. EOF) to exit the shell.

Learn about command-line options (in particular, how to increase heap size
which may be needed for larger applications):

    $ ./micropython -h

To run the complete testsuite, use:

    $ make test

The Unix port comes with a builtin package manager called upip, e.g.:

    $ ./micropython -m upip install micropython-pystone
    $ ./micropython -m pystone

Browse available modules on
[PyPI](https://pypi.python.org/pypi?%3Aaction=search&term=micropython).
Standard library modules come from the
[micropython-lib](https://github.com/micropython/micropython-lib) project.

External dependencies
---------------------

The `libffi` library and `pkg-config` tool are required. On Debian/Ubuntu/Mint
derivative Linux distros, install `build-essential`(includes toolchain and
make), `libffi-dev`, and `pkg-config` packages.

Other dependencies can be built together with MicroPython. This may
be required to enable extra features or capabilities, and in recent
versions of MicroPython, these may be enabled by default. To build
these additional dependencies, in the unix port directory first execute:

    $ make submodules

This will fetch all the relevant git submodules (sub repositories) that
the port needs.  Use the same command to get the latest versions of
submodules as they are updated from time to time. After that execute:

    $ make deplibs

This will build all available dependencies (regardless whether they are used
or not). If you intend to build MicroPython with additional options
(like cross-compiling), the same set of options should be passed to `make
deplibs`. To actually enable/disable use of dependencies, edit the
`ports/unix/mpconfigport.mk` file, which has inline descriptions of the
options. For example, to build SSL module (required for the `upip` tool
described above, and so enabled by default), `MICROPY_PY_USSL` should be set
to 1.
