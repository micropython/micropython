Port of MicroPython to NXP iMX RT 10xx
======================================

Currently supports Teensy 4.0, Teensy 4.1, and the
MIMXRT1010_EVK, MIMXRT1020_EVK, MIMXRT1050_EVK, MIMXRT1060_EVK and
MIMXRT1064_EVK boards.

Features:
  - REPL over USB VCP
  - machine.ADC
  - machine.I2C
  - machine.LED
  - machine.Pin
  - machine.PWM
  - machine.RTC
  - machine.SDCard
  - machine.SPI
  - machine.Signal
  - machine.SoftI2C
  - machine.SoftSPI
  - machine.Timer
  - machine.UART
  - LFS2 file system at the internal Flash
  - SDCard support (not on MIMXRT1010_EVK)
  - Ethernet (not on Teensy 4.0 and MIMXRT1010_EVK)

Known issues:

TODO:
  - More peripherals (Counter, I2S, CAN, etc)
  - More Python options

## Build Instructions

Before building the firmware for a given board the MicroPython cross-compiler
must be built; it will be used to pre-compile some of the built-in scripts to
bytecode.  The cross-compiler is built and run on the host machine, using:

    $ make -C mpy-cross

This command should be executed from the root directory of this repository.
All other commands below should be executed from the ports/mimxrt/ directory.

An ARM compiler is required for the build, along with the associated binary
utilities.  The default compiler is `arm-none-eabi-gcc`, which is available for
Arch Linux via the package `arm-none-eabi-gcc`, for Ubuntu via instructions
[here](https://launchpad.net/~team-gcc-arm-embedded/+archive/ubuntu/ppa), or
see [here](https://launchpad.net/gcc-arm-embedded) for the main GCC ARM
Embedded page.  The compiler can be changed using the `CROSS_COMPILE` variable
when invoking `make`.

In addition newlib is required which is available for Arch Linux via the
package `arm-none-eabi-newlib`, for Ubuntu/Debian install package `libnewlib-arm-none-eabi`

Next, the board to build must be selected.  Any of the board names of the
subdirectories in the `boards/` directory is a valid board.  The board name
must be passed as the argument to `BOARD=` when invoking `make`.

All boards require certain submodules to be obtained before they can be built.
The correct set of submodules can be initialised using (with `SEEED_ARCH_MIX`
as an example of the selected board):

    $ make BOARD=SEEED_ARCH_MIX submodules

Then to build the board's firmware run:

    $ make BOARD=SEEED_ARCH_MIX

The above command should produce binary images in the `build-SEEED_ARCH_MIX/`
subdirectory (or the equivalent directory for the board specified).

## Flashing

Deploy the firmware following the instructions here
https://docs.micropython.org/en/latest/mimxrt/tutorial/intro.html#deploying-the-firmware
