# The minimal port

This port is intended to be a minimal MicroPython port that actually runs.
It can run under Linux (or similar) and on any STM32F4xx MCU (eg the pyboard).

## Building and running Linux version

By default the port will be built for the host machine:

    $ make

To run a small test script do:

    $ make run

## Building for an STM32 MCU

The Makefile has the ability to build for a Cortex-M CPU, and by default
includes some start-up code for an STM32F4xx MCU and also enables a UART
for communication.  To build:

    $ make CROSS=1

If you previously built the Linux version, you will need to first run
`make clean` to get rid of incompatible object files.

Building will produce the build/firmware.dfu file which can be programmed
to an MCU using:

    $ make CROSS=1 deploy

This version of the build will work out-of-the-box on a pyboard (and
anything similar), and will give you a MicroPython REPL on UART1 at 9600
baud.  Pin PA13 will also be driven high, and this turns on the red LED on
the pyboard.
