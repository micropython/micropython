The Micro Python project
========================
<p align="center">
  <img src="https://raw2.github.com/micropython/micropython/master/logo/upython-with-micro.jpg" alt="MicroPython Logo"/>
</p>

This is the Micro Python project, which aims to put an implementation
of Python 3.x on a microcontroller.

WARNING: this project is in its early stages and is subject to large
changes of the code-base, including project-wide name changes and API
changes.  The software will not start to mature until March 2014 at the
earliest.

See the repository www.github.com/micropython/pyboard for the Micro
Python board.

Major components in this repository:
- py/ -- the core Python implementation, including compiler and runtime.
- unix/ -- a version of Micro Python that runs on Unix.
- stmhal/ -- a version of Micro Python that runs on the Micro Python board
  with an STM32F405RG (using ST's new Cube HAL drivers).
- stm/ -- obsolete version of Micro Python for the Micro Python board
  that uses ST's old peripheral drivers.
- teensy/ -- a version of Micro Python that runs on the Teensy 3.1
  (preliminary but functional).

Additional components:
- unix-cpy/ -- a version of Micro Python that outputs bytecode (for testing).
- tests/ -- test framework and test scripts.
- tools/ -- various tools, including the pyboard.py module.
- examples/ -- a few example Python scripts.

"make" is used to build the components, or "gmake" on BSD-based systems.
You will also need bash and python (2.7 or 3.3) for the stm port.

The Unix version
----------------

The "unix" part requires a standard Unix environment with gcc and GNU make.
x86 and x64 architectures are supported (i.e. x86 32- and 64-bit). ARM to
be confirmed. Porting to other architectures require writing some assembly
code for the exception handling.

To build:

    $ cd unix
    $ make

Then to test it:

    $ ./micropython
    >>> list(5 * x + y for x in range(10) for y in [4, 2, 1])

Debian/Ubuntu/Mint derivative Linux distros will require build-essentials and
libreadline-dev packages installed. To build FFI (Foreign Function Interface)
module (recommended, enable in unix/mpconfigport.mk), libffi-dev is required.

The STM version
---------------

The "stmhal" part requires an ARM compiler, arm-none-eabi-gcc, and associated
bin-utils.  For those using Arch Linux, you need arm-none-eabi-binutils and
arm-none-eabi-gcc packages from the AUR.  Otherwise, try here:
https://launchpad.net/gcc-arm-embedded

To build:

    $ cd stmhal
    $ make

You then need to get your board into DFU mode.  On the pyboard, connect the
3V3 pin to the P1/DFU pin with a wire (on PYBv1.0 they are next to each other
on the bottom left of the board, second row from the bottom).

Then to flash the code via USB DFU to your device:

    $ dfu-util -a 0 -D build/flash.dfu

You will need the dfu-util program, on Arch Linux it's dfu-util-git in the AUR.
