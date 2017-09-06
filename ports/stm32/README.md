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
serial or UART, depending on the board.  For the pyboard you can try:

    $ picocom /dev/ttyACM0
