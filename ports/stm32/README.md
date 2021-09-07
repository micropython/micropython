MicroPython port to STM32 MCUs
==============================

This directory contains the port of MicroPython to ST's line of STM32
microcontrollers.  Supported MCU series are: STM32F0, STM32F4, STM32F7,
STM32H7, STM32L0, STM32L4 and STM32WB.  Parts of the code here utilise the
STM32Cube HAL library.

The officially supported boards are the line of pyboards: PYBv1.0 and PYBv1.1
(both with STM32F405), PYBLITEv1.0 (with STM32F411) and PYBD-SFx (with
STM32F7xx MCUs).  See
[micropython.org/pyboard](http://www.micropython.org/pyboard/) for further
details.

Other boards that are supported include ST Discovery and Nucleo boards.
See the boards/ subdirectory, which contains the configuration files used
to build each individual board.

The STM32H7 series has preliminary support: there is a working REPL via
USB and UART, as well as very basic peripheral support, but some things do
not work and none of the advanced features of the STM32H7 are yet supported,
such as the clock tree.  At this point the STM32H7 should be considered as a
fast version of the STM32F7.

Build instructions
------------------

Before building the firmware for a given board the MicroPython cross-compiler
must be built; it will be used to pre-compile some of the built-in scripts to
bytecode.  The cross-compiler is built and run on the host machine, using:
```bash
$ make -C mpy-cross
```
This command should be executed from the root directory of this repository.
All other commands below should be executed from the ports/stm32/ directory.

An ARM compiler is required for the build, along with the associated binary
utilities.  The default compiler is `arm-none-eabi-gcc`, which is available for
Arch Linux via the package `arm-none-eabi-gcc`, for Ubuntu via instructions
[here](https://launchpad.net/~team-gcc-arm-embedded/+archive/ubuntu/ppa), or
see [here](https://launchpad.net/gcc-arm-embedded) for the main GCC ARM
Embedded page.  The compiler can be changed using the `CROSS_COMPILE` variable
when invoking `make`.

Next, the board to build must be selected.  The default board is PYBV10 but any
of the names of the subdirectories in the `boards/` directory is a valid board.
The board name must be passed as the argument to `BOARD=` when invoking `make`.

All boards require certain submodules to be obtained before they can be built.
The correct set of submodules can be initialised using (with `PYBV11` as an
example of the selected board):

    $ make BOARD=PYBV11 submodules

Then to build the board's firmware run:

    $ make BOARD=PYBV11

The above command should produce binary images in the `build-PYBV11/`
subdirectory (or the equivalent directory for the board specified).

Note that some boards require the mboot bootloader to be built and deployed
before flashing the main firmware.  For such boards an information message
about this will be printed at the end of the main firmware build.  Mboot
can be built via:

    $ make -C mboot BOARD=STM32F769DISC

For more information about mboot see mboot/README.md.


### Flashing the Firmware using DFU mode

You must then get your board/microcontroller into DFU (Device Firmware
Update) mode.

If you already have MicroPython installed on the board you can do that by
calling `machine.bootloader()` on the board, either at the REPL or using
`pyboard.py`. A nice property of this approach is that you can automate it
so you can update the board without manually pressing any buttons.

If you do not have MicroPython running yet, temporarily jumper your board's
DFU pin (typ. BOOT0) to 3.3v and reset or power-on the board.

On a pyboard the P1/DFU pin and a 3.3v pin are next to each other (on the
bottom left of the board, second row from the bottom) and the reset button
is labeled RST.

For the pyboard D-series you can enter the mboot DFU bootloader by holding down
the USR button, pressing and releasing the RST button, and continuing to hold
down USR until the LED is white (4th in the cycle), then let go of USR while
the LED is white. The LED will then flash red once per second to indicate it
is in USB DFU mode.

Once the board is in DFU mode, flash the firmware using the command:

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


### Flashing the Firmware with stlink

ST Discovery or Nucleo boards have a builtin programmer called ST-LINK. With
these boards and using Linux or OS X, you have the option to upload the
`stm32` firmware using the `st-flash` utility from the
[stlink](https://github.com/texane/stlink) project. To do so, connect the board
with a mini USB cable to its ST-LINK USB port and then use the make target
`deploy-stlink`. For example, if you have the STM32F4DISCOVERY board, you can
run:

    $ make BOARD=STM32F4DISC deploy-stlink

The `st-flash` program should detect the USB connection to the board
automatically. If not, run `lsusb` to determine its USB bus and device number
and set the `STLINK_DEVICE` environment variable accordingly, using the format
`<USB_BUS>:<USB_ADDR>`. Example:

    $ lsusb
    [...]
    Bus 002 Device 035: ID 0483:3748 STMicroelectronics ST-LINK/V2
    $ export STLINK_DEVICE="002:0035"
    $ make BOARD=STM32F4DISC deploy-stlink


### Flashing the Firmware with OpenOCD

Another option to deploy the firmware on ST Discovery or Nucleo boards with a
ST-LINK interface uses [OpenOCD](http://openocd.org/). Connect the board with
a mini USB cable to its ST-LINK USB port and then use the make target
`deploy-openocd`. For example, if you have the STM32F4DISCOVERY board:

    $ make BOARD=STM32F4DISC deploy-openocd

The `openocd` program, which writes the firmware to the target board's flash,
is configured via the file `ports/stm32/boards/openocd_stm32f4.cfg`. This
configuration should work for all boards based on a STM32F4xx MCU with a
ST-LINKv2 interface. You can override the path to this configuration by setting
`OPENOCD_CONFIG` in your Makefile or on the command line.

Accessing the board
-------------------

Once built and deployed, access the MicroPython REPL (the Python prompt) via USB
serial or UART, depending on the board.  There are many ways to do this, one of
which is via `mpremote` (install it using `pip install mpremote`):

    $ mpremote

Other options are `picocom` and `screen`, for example:

    $ picocom /dev/ttyACM0
