[![Build Status][travis-img]][travis-repo]
[travis-img]:  https://travis-ci.org/micropython/micropython.png?branch=master
[travis-repo]: https://travis-ci.org/micropython/micropython

The Micro Python project
========================
<p align="center">
  <img src="https://raw2.github.com/micropython/micropython/master/logo/upython-with-micro.jpg" alt="MicroPython Logo"/>
</p>

This is the Micro Python project, which aims to put an implementation
of Python 3.x on a microcontroller.

WARNING: this project is in early beta stage and is subject to large
changes of the code-base, including project-wide name changes and API
changes.

Micro Python implements the entire Python 3.4 syntax (including exceptions,
"with", "yield from", etc.).  The following core datatypes are provided:
str (no Unicode support yet), bytes, bytearray, tuple, list, dict, set,
array.array, collections.namedtuple, classes and instances.  Builtin
modules include sys, time, and struct.  Note that only subset of
Python 3.4 functionality implemented for the data types and modules.

See the repository www.github.com/micropython/pyboard for the Micro
Python board, the officially supported reference electronic circuit board.

Major components in this repository:
- `py/` - the core Python implementation, including compiler and runtime.
- `unix/` - a version of Micro Python that runs on Unix.
- `stmhal/` - a version of Micro Python that runs on the Micro Python board
  with an STM32F405RG (using ST's Cube HAL drivers).
- `teensy/` - a version of Micro Python that runs on the Teensy 3.1
  (preliminary but functional).

Additional components:
- `bare-arm/` - a bare minimum version of Micro Python for ARM MCUs.  Start
  with this if you want to port Micro Python to another microcontroller.
- `qemu/` - a version of MicroPython that runs on Qemu emulating Cortex-M3.
- `unix-cpy/` - a version of Micro Python that outputs bytecode (for testing).
- `tests/` - test framework and test scripts.
- `tools/` - various tools, including the pyboard.py module.
- `examples/` - a few example Python scripts.

`make` is used to build the components, or `gmake` on BSD-based systems.
You will also need bash and Python (at least 2.7 or 3.3).

The Unix version
----------------

The `unix` port requires a standard Unix environment with gcc and GNU make.
Both of Intel x86 architectures are supported, as well as ARMv7. It will
probably work on other architectures if you set `MICROPY_NLR_SETJMP` in the
`unix/mpconfigport.h` header, test results are wellcome.

To build:

    $ cd unix
    $ make

Then to test it:

    $ ./micropython
    >>> list(5 * x + y for x in range(10) for y in [4, 2, 1])

Debian/Ubuntu/Mint derivative Linux distros will require build-essentials and
libreadline-dev packages installed. To build FFI (Foreign Function Interface)
module, libffi-dev package is required. If you have problems with some
dependencies, they can be disabled in unix/mpconfigport.mk .

The STM version
---------------

The `stmhal` port requires an ARM compiler, `arm-none-eabi-gcc`, and associated
bin-utils.  For those using Arch Linux, you need `arm-none-eabi-binutils` and
`arm-none-eabi-gcc` packages from the AUR.  Otherwise, try here:
https://launchpad.net/gcc-arm-embedded

To build:

    $ cd stmhal
    $ make

You then need to get your board into DFU mode.  On the pyboard, connect the
3V3 pin to the P1/DFU pin with a wire (on PYBv1.0 they are next to each other
on the bottom left of the board, second row from the bottom).

Then to flash the code via USB DFU to your device:

    $ make deploy

You will need the dfu-util program, on Arch Linux it's dfu-util-git in the
AUR.  If the above does not work it may be because you don't have the
correct permissions.  Try then:

    $ sudo dfu-util -a 0 -D build-PYBV10/firmware.dfu

Qemu version
------------

The purposes of this port had been enable:
  1. continuos integration
    - run test agains architecture specific part of code base
  2. experimentation
    - simulation & prototyping of anything that has architecture-specific code
    - exploring instruction set in terms of optimising some part of uPy or a
  module
  3. speedy debugging
    - no need for JTAG or even the chip itself
    - no need to use OpenOCD or anything else the might slow down the process in terms of plugging things together and pressing buttons etc

This port will only work with with [GCC ARM Embedded](launchpad.net/gcc-arm-embedded) toolchain and not with CodeSourcery toolchain. You will need to modify `LDFLAGS` if you want to use CodeSourcery's version of `arm-none-eabi`. The difference is that CodeSourcery needs `-T generic-m-hosted.ld` while ARM's version  requires `--specs=nano.specs --specs=rdimon.specs` to be passed to the linker.
