The Micro Python project
========================
<p align="center">
  <img src="https://raw2.github.com/micropython/micropython/master/logo/upython-with-micro.png" alt="MicroPython Logo"/>
</p>

This is the Micro Python project, which aims to put an implementation
of Python 3.x on a microcontroller.

WARNING: this project is in its early stages and is subject to large
changes of the code-base, including project-wide name changes and API
changes.  The software will not start to mature until March 2014 at the
earliest.

See the repository www.github.com/micropython/pyboard for the Micro
Python board.  At the moment, finalising the design of the board is
the top priority.

Major components in this repository:
- py/ -- the core Python implementation, including compiler and runtime.
- unix/ -- a version of Micro Python that runs on Unix.
- stm/ -- a version of Micro Python that runs on the Micro Python board
  with an STM32F405RG.

Additional components:
- unix-cpy/ -- a version of Micro Python that outputs bytecode (for testing).
- tests/ -- test framework and test scripts.
- tools/ -- various tools.

"make" is used to build the components, or "gmake" on BSD-based systems.
You will also need bash and python3, and python2 for the stm port.

The Unix version
----------------

The "unix" part requires a standard Unix environment with gcc and GNU make.
It works only for 64-bit machines due to a small piece of x86-64 assembler
for the exception handling.

To build:

    $ cd unix
    $ make

Then to test it:

    $ ./py
    >>> list(5 * x + y for x in range(10) for y in [4, 2, 1])

Ubuntu and Mint derivatives will require build-essentials and libreadline-dev
packages installed.

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
