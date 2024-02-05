Port of MicroPython to Microchip SAMD MCUs
==========================================

Supports SAMD21 and SAMD51. For each supported device there is a
subdirectory in the `boards/` directory.

The entry point for the specific port documentation is at
https://docs.micropython.org/en/latest/samd/quickref.html, which also
shows the assignment of IO-Functions to pins. The generic MicroPython
documentation applies for anything not specific for the SAM port.

Due to the different flash sizes of SAMD21 and SAMD51 devices, the
coverage of MicroPython modules differ. Use help("modules") to tell,
which MicroPython modules are provided.

Build instructions
------------------

Before building the firmware for a given board the MicroPython cross-compiler
must be built; it will be used to pre-compile some of the built-in scripts to
bytecode.  The cross-compiler is built and run on the host machine, using:

    $ make -C mpy-cross

This command should be executed from the root directory of this repository.
All other commands below should be executed from the ports/stm32/ directory.

An ARM compiler is required for the build, along with the associated binary
utilities.  The default compiler is `arm-none-eabi-gcc`, which is available for
Arch Linux via the package `arm-none-eabi-gcc`, for Ubuntu via instructions
[here](https://launchpad.net/~team-gcc-arm-embedded/+archive/ubuntu/ppa), or
see [here](https://launchpad.net/gcc-arm-embedded) for the main GCC ARM
Embedded page.  The compiler can be changed using the `CROSS_COMPILE` variable
when invoking `make`.

Next, the board to build must be selected.  There is no default board. Any
of the names of the subdirectories in the `boards/` directory is a valid board.
The board name must be passed as the argument to `BOARD=` when invoking `make`.

All boards require certain submodules to be obtained before they can be built.
The correct set of submodules can be initialised using (with
`ADAFRUIT_ITSYBITSY_M4_EXPRESS` as an example of the selected board):

    $ make BOARD=ADAFRUIT_ITSYBITSY_M4_EXPRESS submodules

Then to build the board's firmware run:

    $ make BOARD=ADAFRUIT_ITSYBITSY_M4_EXPRESS clean
    $ make BOARD=ADAFRUIT_ITSYBITSY_M4_EXPRESS

The above command produces binary images in the
`build-ADAFRUIT_ITSYBITSY_M4_EXPRESS/` subdirectory (or the equivalent
directory for the board specified).

### Flashing the Firmware

Most SAMD21 and SAMD51 boards have a built in firmware loader. To start it, push
the reset button of the boards twice. The speed varies a little bit. If the
firmware loader starts, a drive will appear in the file manager of your PC.
Copy the created `firmware.uf2` file to that drive. If the upload is finished, the
drive will disappear and the board will reboot.
