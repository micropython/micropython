The bare-arm port
=================

This port is intended to be the bare-minimum amount of code and configuration
required to get MicroPython compiling and running on a bare-metal ARM-based
target.  No external dependencies or libraries are needed for this build and
it shows exactly what hardware and system functionality MicroPython needs to
run.

To build, simply run `make` in this directory.  The output will be
`build/firmware.elf` (and also corresponding `.bin` and `.dfu` files).  This
firmware can run on an STM32F405-based board (eg a PYBv1.x) and `make deploy`
will program it to such an MCU when put in USB DFU mode.  The output is a UART
at 115200 baud, with TX on PA0.

There are some simple demonstration code strings (see `main.c`) which are
compiled and executed when the firmware starts.  They produce output on the
system's stdout.

The size of the firmware (the machine code that is programmed to the
microcontroller's flash/ROM) is currently around 61200 bytes.
