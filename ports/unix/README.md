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

    $ ./build-standard/micropython -h

To run the complete testsuite, use:

    $ make test

The Unix port comes with a built-in package manager called `mip`, e.g.:

    $ ./build-standard/micropython -m mip install hmac

or

    $ ./build-standard/micropython
    >>> import mip
    >>> mip.install("hmac")

Browse available modules at
[micropython-lib](https://github.com/micropython/micropython-lib). See
[Package management](https://docs.micropython.org/en/latest/reference/packages.html)
for more information about `mip`.

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
options. For example, to build the SSL module, `MICROPY_PY_SSL` should be
set to 1.

Build Variants
--------------

The default `standard` variant is suitable for most uses. Additional build
variants can be found in the `variants` subdirectory and built by setting
`VARIANT=name` environment variable (i.e. `make test VARIANT=coverage`).

Note that each variant is built in its own build directory.

Additional variants include:

* `minimal` variant has minimum code size and reduced features.
* `coverage` variant enables almost all possible features and is suitable for
  code coverage testing.

Debug Symbols
-------------

By default, builds are stripped of symbols and debug information to save size.

To build a debuggable version of the Unix port, there are two options

1. Run `make [other arguments] DEBUG=1`. Note setting `DEBUG` also reduces the
   optimisation level, so it's not a good option for builds that also want the
   best performance.
2. Run `make [other arguments] STRIP=`. Note that the value of `STRIP` is
   empty. This will skip the build step that strips symbols and debug
   information, but changes nothing else in the build configuration.

Undefined Behavior Sanitizer
----------------------------

The Unix port supports being built with the Undefined Behavior Sanitizer (aka
UBSan) (see [gcc
docs](https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html#index-fsanitize_003dundefined),
the Clang docs linked from that page have more details.)

Only the `coverage` variant is built with UBSan enabled by default. Set the
environment variable `UBSAN=1` to enable UBSan for a different variant, or set
`VARIANT=coverage UBSAN=0` to build `coverage` without UBSan.

UBSan will print an error to stderr if Undefined Behaviour is triggered, but
continues execution. This means any unit test that triggers UB fails due to the
additional output.

The `make test` target should be used to run tests if possible, because it sets
the `UBSAN_OPTIONS` environment variable to compensate for UB in some third party
code. See the `Makefile` in this directory for details.

