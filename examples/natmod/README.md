# Dynamic Native Modules

Dynamic Native Modules are .mpy files that contain native machine code from a
language other than Python. For more info see [the
documentation](https://docs.micropython.org/en/latest/develop/natmod.html).

This should not be confused with [User C
Modules](https://docs.micropython.org/en/latest/develop/cmodules.html) which are
a mechanism to add additional out-of-tree modules into the firmware build.

## Examples

This directory contains several examples of writing dynamic native modules, in
two main categories:

1.  Feature examples.

    * `features0` - A module containing a single "factorial" function which
      demonstrates working with integers.

    * `features1` - A module that demonstrates some common tasks:
        - defining simple functions exposed to Python
        - defining local, helper C functions
        - defining constant integers and strings exposed to Python
        - getting and creating integer objects
        - creating Python lists
        - raising exceptions
        - allocating memory
        - BSS and constant data (rodata)
        - relocated pointers in rodata

    * `features2` - This is a hybrid module containing both Python and C code,
      and additionally the C code is spread over multiple files. It also
      demonstrates using floating point (only when the target supports
      hardware floating point).

    * `features3` - A module that shows how to use types, constant objects,
      and creating dictionary instances.

    * `features4` - A module that demonstrates how to define a class.

2.  Dynamic version of existing built-ins.

    This provides a way to add missing functionality to firmware that doesn't
    include certain built-in modules. See the `heapq`, `random`, `re`,
    `deflate`, `btree`, and `framebuf` directories.

    So for example, if your firmware was compiled with `MICROPY_PY_FRAMEBUF`
    disabled (e.g. to save flash space), then it would not include the
    `framebuf` module. The `framebuf` native module provides a way to add the
    `framebuf` module dynamically.

    The way these work is they define a dynamic native module which
    `#include`'s the original module and then does the necessary
    initialisation of the module's globals dict.

## Build instructions

To compile an example, you need to have the same toolchain available as
required for your target port. e.g. `arm-none-eabi-gcc` for any ARM Cortex M
target. See the port instructions for details.

You also need to have the `pyelftools` Python package available, either via
your system package manager or installed from PyPI in a virtual environment
with `pip`.

Each example provides a Makefile. You should specify the `ARCH` argument to
make (one of x86, x64, armv6m, armv7m, xtensa, xtensawin, rv32imc):

```
$ cd features0
$ make ARCH=armv7m
$ mpremote cp features0.mpy :
```
