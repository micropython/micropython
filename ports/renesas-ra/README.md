# The Renesas RA port

This is a port of MicroPython to the Renesas RA family of microcontrollers.
Currently supported features are:

- Filesystem on the internal flash using FatFs.
- `time` module with sleep, time, and ticks functions.
- `os` module with VFS support.
- `machine` module with the following classes: `Pin`, `ADC`, `PWM`, `DAC`, `I2C`,
  `SPI`, `SoftI2C`, `SoftSPI`, `UART`, `RTC`, `SDCard`.
- `sdcard` module for MCUs without native `machine.SDCard` support.

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

* Protocol buffer compiler
Building for the `ARDUINO_PORTENTA_C33` board will build the `esp_hosted` driver, that depends
on the Protocol Buffer Compiler being installed. On Debian/Ubuntu, this can be installed with
`sudo apt-get install protobuf-compiler`.

* Obtain submodules
First the submodules must be obtained using:

      $ make submodules

* Build binary image `.hex`
Then to build for a given board subdirectory name, run:

      $ make BOARD=EK_RA6M2 clean
      $ make BOARD=EK_RA6M2

  The default board subdirectory name is EK_RA6M2 (which is for EK-RA6M2 board)
but any of the names of the subdirectories in the `boards/` directory can be
passed as the argument to `BOARD=`; for example `RA4M1_CLICKER`, `EK_RA4M1`,
`EK_RA4W1` and `EK_RA6M1`.
The above command should produce binary images `firmware.hex` in the
build-EK_RA6M2/` subdirectory (or the equivalent directory for the board specified).

## Board definition auto-generated code

The supported board definitions contain auto-generated configuration files in
the `boards/<BOARD_NAME>/ra_cfg` and `boards/<BOARD_NAME>/ra_gen` folders.

These are generated with the [RA Smart Configurator](https://www.renesas.com/us/en/software-tool/ra-smart-configurator)
tool which is used to define peripheral configuration, pinouts, interrupts etc. for each board.

This tool can be installed either as part of the "Renesas e² studio", or separately with
the fsp driver package from https://github.com/renesas/fsp/releases eg.
* [setup_fsp_v4_4_0_rasc_v2023-04.exe](https://github.com/renesas/fsp/releases/download/v4.4.0/setup_fsp_v4_4_0_rasc_v2023-04.exe)
* [setup_fsp_v4_4_0_rasc_v2023-04.exe](https://github.com/renesas/fsp/releases/download/v4.4.0/setup_fsp_v4_4_0_rasc_v2023-04.AppImage)

This tool can be used to create new board definitions or modify existing ones
by opening one of the `configuration.xml` files in the board folders.

Once the `configuration.xml` file is opened in RA Smart Configurator and modified as
needed, the "Generate Project Content" button can be pressed to export new copies
of the `ra_cfg` and `ra_gen` folders.

## Supported/Unsupported functions
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

