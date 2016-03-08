MicroPython port to STM32 MCUs
==============================

This directory contains the port of MicroPython to ST's line of STM32Fxxx
microcontrollers.  It is based on the STM32Cube HAL library and currently
supports: STM32F401, STM32F405, STM32F411, STM32F429, STM32F746.

The officially supported boards are the line of pyboards: PYBv1.0 and PYBv1.1
(both with STM32F405), and PYBLITEv1.0 (with STM32F411).  See
[micropython.org/pyboard](http://www.micropython.org/pyboard/) for further
details.

Other boards that are supported include ST Discovery and Nucleo boards.
See the boards/ subdirectory, which contains the configuration files used
to build each individual board.

Build instructions
------------------

An ARM compiler is required for the build, along with the associated binary
utilities.  The default compiler is `arm-none-eabi-gcc`, which is available for
Arch Linux via the package `arm-none-eabi-gcc`, for Ubuntu via instructions
[here](https://launchpad.net/~team-gcc-arm-embedded/+archive/ubuntu/ppa), or
see [here](https://launchpad.net/gcc-arm-embedded) for the main GCC ARM
Embedded page.  The compiler can be changed using the `CROSS_COMPILE` variable
when invoking `make`.

To build for a given board, run:

    $ make BOARD=PYBV11

The default board is PYBV10 but any of the names of the subdirectories in the
`boards/` directory can be passed as the argument to `BOARD=`.  The above command
should produce binary images in the `build-PYBV11/` subdirectory (or the
equivalent directory for the board specified).

You must then get your board/microcontroller into DFU mode.  On the pyboard
connect the 3V3 pin to the P1/DFU pin with a wire (they are next to each
other on the bottom left of the board, second row from the bottom) and then
reset (by pressing the RST button) or power on the board.  Then flash the
firmware using the command:

    $ make BOARD=PYBV11 deploy

This will use the included `tools/pydfu.py` script.  You can use instead the
`dfu-util` program (available [here](http://dfu-util.sourceforge.net/)) by
passing `USE_PYDFU=0`:

    $ make BOARD=PYBV11 USE_PYDFU=0 deploy

If flashing the firmware does not work it may be because you don't have the
correct permissions.  Try then:

    $ sudo make BOARD=PYBV11 deploy

Or using `dfu-util` directly:

    $ sudo dfu-util -a 0 -d 0483:df11 -D build-PYBV11/firmware.dfu

Accessing the board
-------------------

Once built and deployed, access the MicroPython REPL (the Python prompt) via USB
serial or UART, depending on the board.  For the pyboard you can try:

    $ picocom /dev/ttyACM0
