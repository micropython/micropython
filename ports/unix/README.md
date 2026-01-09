MicroPython Unix port
=====================

The "unix" port runs in standard Unix-like environments including Linux, BSD,
macOS, and Windows Subsystem for Linux.

The x86 and x64 architectures are supported (i.e. x86 32- and 64-bit), as well
as ARM, MIPS, and RISC-V. Extending the unix port to another architecture
requires writing some assembly code for the exception handling and garbage
collection.  Alternatively, a fallback implementation based on setjmp/longjmp
can be used.

Building
--------

### Dependencies

To build the unix port locally then you will need:

* git command line executable, unless you downloaded a source .tar.xz file from
  https://micropython.org/download/
* an appropriate gcc (or clang for macOS) toolchain for your target
* GNU Make
* Python 3.x

To build the default "standard" variant and configuration, then you will also
need:

* `pkg-config` tool
* `libffi` library and headers

On Debian/Ubuntu/Mint and related Linux distros, you can install all these
dependencies with a command like:

```
# apt install build-essential git python3 pkg-config libffi-dev
```

(See below for steps to build either a standalone or minimal MicroPython
executable that doesn't require system `libffi` or `pkg-config`.)

### Default build steps

To set up the environment for building (not needed every time), starting from
the top-level MicroPython directory:

    $ cd ports/unix
    $ make -C ../../mpy-cross
    $ make submodules

The `mpy-cross` step builds the [MicroPython
cross-compiler](https://github.com/micropython/micropython/?tab=readme-ov-file#the-micropython-cross-compiler-mpy-cross).
The `make submodules` step can be skipped if you didn't clone the MicroPython
source from git.

Next, to build the actual executable (still in the `ports/unix` directory):

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

There are other make targets to interact with the testsuite:

    $ make test//int       # Run all tests matching the pattern "int"
    $ make test/ports/unix # Run all tests in ports/unix
    $ make test-failures   # Re-run only the failed tests
    $ make print-failures  # print the differences for failed tests
    $ make clean-failures  # delete the .exp and .out files from failed tests

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

### Minimal Variant

The "standard" variant of MicroPython is the default. It enables most features,
including external modules interfaced using `libffi`. To instead build the
"minimal" variant, which disables almost all optional features and modules:

    $ cd ports/unix
    $ make submodules
    $ make VARIANT=minimal

The executable will be built at `build-minimal/micropython`.

Additional variants can be found in the `variants` sub-directory of the port,
although these are mostly of interest to MicroPython maintainers.

### Standalone build

By default, the "standard" variant uses `pkg-config` to link to the system's
shared `libffi` library.

It is possible to instead build a standalone MicroPython where `libffi` is built
from source and linked statically into the `micropython` executable. This is
mostly useful for embedded or cross-compiled applications.

Building standalone requires `autoconf` and `libtool` to also be installed.

To build standalone:

    $ export MICROPY_STANDALONE=1
    $ make submodules                # fetches libffi submodule
    $ make deplibs                   # build just the external libraries
    $ make                           # build MicroPython itself

`make deplibs` causes all supported external libraries (currently only `libffi`)
to be built inside the build directory, so it needs to run again only after
`make clean`.

If you intend to build MicroPython with additional options (like
cross-compiling), the same set of options should be passed to both `make
deplibs` and `make`.

### Other dependencies

To actually enable/disable use of dependencies, edit the
`ports/unix/mpconfigport.mk` file, which has inline descriptions of the
options. For example, to build the SSL module, `MICROPY_PY_SSL` should be
set to 1.

### Debug Symbols

By default, builds are stripped of symbols and debug information to save size.

To build a debuggable version of the Unix port, there are two options:

1. Run `make [other arguments] DEBUG=1`. Note setting `DEBUG` also reduces the
   optimisation level and enables assertions, so it's not a good option for
   builds that also want the best performance.
2. Run `make [other arguments] STRIP=`. Note that the value of `STRIP` is
   empty. This will skip the build step that strips symbols and debug
   information, but changes nothing else in the build configuration.

### Optimisation Level

The default compiler optimisation level is -Os, or -Og if `DEBUG=1` is set.

Setting the variable `COPT` will explicitly set the optimisation level. For
example `make [other arguments] COPT=-O0 DEBUG=1` will build a binary with no
optimisations, assertions enabled, and debug symbols.

### Sanitizers

Sanitizers are extra runtime checks supported by gcc and clang. The CI process
supports building with the "undefined behavior" (UBSan) or "address" (ASan)
sanitizers. The script `tools/ci.sh` is the source of truth about how to build
and run in these modes.

Several classes of checks are disabled via compiler flags:

* In the undefined behavior sanitizer, checks based on the presence of the
  `non_null` attribute are disabled because the code makes technically incorrect
  calls like `memset(NULL, 0, 0)`. A future C standard is likely to permit such
  calls.
* In the address sanitizer, `detect_stack_use_after_return` is disabled. This
  check is intended to make sure locals in a "returned from" stack frame are not
  used. However, this mode interferes with various assumptions that
  MicroPython's stack checking, NLR, and GC rely on.

### Notes about x86 (i686) support

It used to be possible to create an x86 (32-bits) build on a x64 (64-bits) host
by passing `MICROPY_FORCE_32BIT=1` to `make`.  That option was retired: x86
usage has declined quite a bit in the past few years, and MicroPython now
supports at least one more mixed 32/64-bits target architecture for which
enabling `MICROPY_FORCE_32BIT` would make builds fail.

x86 will be treated as a cross-compilation target from now on.  This means you
will need to install a suitable compiler (on Ubuntu you can install the
`gcc-i686-linux-gnu` and `g++-i686-linux-gnu` packages, for example) and pass
the toolchain's command prefix to the `CROSS_COMPILE` command line variable.

This change is also extended to native modules, for which you may need to pass
the toolchain's command prefix to the `CROSS` command line variable if your
x86 compiler cannot be invoked with `i686-linux-gnu-gcc`.

Or, as an example:

```bash
$ printf "%s %s %s %s\n" $(lsb_release -d | cut -f 2) $(uname -m)
Ubuntu 24.04.3 LTS x86_64

$ i686-linux-gnu-gcc --version | head -n 1
i686-linux-gnu-gcc (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0

$ CROSS_COMPILE=i686-linux-gnu- make -C ports/unix
make: Entering directory '/ports/unix'
Use make V=1 or set BUILD_VERBOSE in your environment to increase build verbosity.
...
LINK build-standard/micropython
   text    data     bss     dec     hex filename
 718479   36016    2092  756587   b8b6b build-standard/micropython
make: Leaving directory '/ports/unix'

$ file -b ports/unix/build-standard/micropython | cut -d, -f-2
ELF 32-bit LSB pie executable, Intel 80386

# `i686-linux-gnu-` is the default prefix for x86 native modules now, it has
# been explicitly mentioned here in case you want to see how to set it.

$ make -C examples/natmod/features0 ARCH=x86 CROSS=i686-linux-gnu-
make: Entering directory '/examples/natmod/features0'
GEN build/features0.config.h
CC features0.c
LINK build/features0.o
arch:         EM_386
text size:    196
bss size:     0
GOT entries:  4
GEN features0.mpy
make: Leaving directory '/examples/natmod/features0'

$ cd examples/natmod/features0 && ../../../ports/unix/build-standard/micropython
MicroPython v1.28.0-preview.63.g8189e9935a.dirty on 2026-01-09; linux [GCC 13.3.0] version
Type "help()" for more information.
>>> import features0
>>> features0.factorial(10)
3628800
```
