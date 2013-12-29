The Micro Python project
========================

This is the Micro Python project, which aims to put an implementation
of Python 3.x on a microcontroller.  The project also includes a small
microcontroller board based around the STM32F405RG.

Major components:
- py/ -- the core Python implementation, including compiler and runtime.
- unix/ -- a version of Micro Python that runs on Unix.
- stm/ -- a version of Micro Python that runs on the Micro Python board
  with an STM32F405RG.
- pybv3/ -- schematics, gerbers and EAGLE files for revision 3 of the
  Micro Python board.

Additional components:
- unix-cpy/ -- a version of Micro Python that outputs bytecode (for testing).
- tests/ -- test framework and test scripts.
- tools/ -- various tools.

"make" is used to build the components.

The Unix version
----------------

The "unix" part requires a standard Unix environment with gcc.  It works
only for 64-bit machines due to a small piece of x86-64 assembler for
the exception handling.

To build:

    $ cd unix
    $ make

Then to test it:

    $ ./py
    >>> list(5 * x + y for x in range(10) for y in [4, 2, 1])

The STM version
---------------

The "stm" part requires an ARM compiler, arm-none-eabi-gcc, and associated
bin-utils.  For those using Arch Linux, you need arm-none-eabi-binutils and
arm-none-eabi-gcc packages from the AUR.  Otherwise, try here:
https://launchpad.net/gcc-arm-embedded

To build:

    $ cd stm
    $ make

Then to flash it via USB DFU to your device:

    $ dfu-util -a 0 -D build/flash.dfu

You will need the dfu-util program, on Arch Linux it's dfu-util-git in the AUR.
