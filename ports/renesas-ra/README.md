# The Renesas RA port

This is a port of MicroPython to the Renesas RA family of microcontrollers.
Currently supported features are:

- Filesystem on the internal flash using FatFs.
- `utime` module with sleep, time, and ticks functions.
- `uos` module with VFS support.
- `machine` module with the following classes: `Pin`, `ADC`, `I2C`, `SPI`,
   `SoftI2C`, `SoftSPI`, `UART`, `RTC`
- sdcard driver if frozen driver is installed.

Currently supported board product names are:

- EK-RA6M2
- RA4M1 CLICKER
- EK-RA6M1
- EK-RA4M1
- EK-RA4W1

Please refer to the quick reference of `renesas-ra` port for more
information about the boards.

## Build instructions

* MicroPython cross-compier
The MicroPython cross-compiler must be built first, which will be
used to pre-compile (freeze) built-in Python code. THis cross-compiler
is built and run on the host machine, using:

      $ make -C mpy-cross

  This command should be executed from the root directory of this repository.
All other commands below should be executed from the ports/renesas-ra/ directory.

* Arm compiler
An `Arm compiler` is required for the build, along with the associated binary
utilities.  The default compiler is `arm-none-eabi-gcc`, which is available for
Linux, Mac and Windows hosts via https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads.
The compiler can be changed using the `CROSS_COMPILE` variable
when invoking `make`.

* Obtain submodules
First the submodules must be obtained using:

      $ make submodules

* Build binary image `.hex`
Then to build for a given board subdirectory name, run:

      $ make BOARD=RA6M2_EK clean
      $ make BOARD=RA6M2_EK

  The default board subdirectory name is RA6M2_EK (which is for EK-RA6M2 board)
but any of the names of the subdirectories in the `boards/` directory can be
passed as the argument to `BOARD=`; for example `RA4M1_CLICKER`, `RA4M1_EK`,
`RA4W1_EK` and `RA6M1_EK`.
The above command should produce binary images `firmware.hex` in the
build-RA6M2_EK/` subdirectory (or the equivalent directory for the board specified).

## Supported/Unsupprted funtions
Please refer to the `renesas-ra` quick reference.

## Known issues

### all boards
* machine.lightsleep(time_ms) and machine.deepsleep(time_ms) raise uncaught exceptions.

    >>> machine.deepsleep(1000)
    >>> Uncaught exception in ExtInt interrupt handler line 16
    TypeError:

### RA4M1-CLICKER
* mpremote: mount command is not available due to limited memory resources.

### EK-RA4M1
* mpremote: mount command is not available due to limited memory resources.

### EK-RA4W1
* mpremote: "Execution: timeout waiting for remote" error happens when importing a file on /remote.

