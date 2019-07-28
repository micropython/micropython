MicroPython port to STM32F103xx MCUs
==============================

This directory contains the port of MicroPython to ST's line of STM32
microcontrollers. Parts of the code here utilise the STM32Cube HAL library.

Build instructions
------------------

Before building the firmware for a given board the MicroPython cross-compiler
must be built; it will be used to pre-compile some of the built-in scripts to
bytecode.  The cross-compiler is built and run on the host machine, using:
```bash
$ make -C mpy-cross
```
This command should be executed from the root directory of this repository.
All other commands below should be executed from the ports/stm32f1/ directory.

An ARM compiler is required for the build, along with the associated binary
utilities.  The default compiler is `arm-none-eabi-gcc`, which is available for
Arch Linux via the package `arm-none-eabi-gcc`, for Ubuntu via instructions
[here](https://launchpad.net/~team-gcc-arm-embedded/+archive/ubuntu/ppa), or
see [here](https://launchpad.net/gcc-arm-embedded) for the main GCC ARM
Embedded page.  The compiler can be changed using the `CROSS_COMPILE` variable
when invoking `make`.

To build for a given board, run:

    $ make BOARD=QiMing_F103RC

The default board is PYBV10 but any of the names of the subdirectories in the
`boards/` directory can be passed as the argument to `BOARD=`.  The above command
should produce binary images in the `build-QiMing_F103RC/` subdirectory (or the
equivalent directory for the board specified).

You must then get your board/microcontroller into DFU mode.  On the board
connect the 3V3 pin to the P1/DFU pin with a wire (they are next to each
other on the bottom left of the board, second row from the bottom) and then
reset (by pressing the RST button) or power on the board.  Then flash the
firmware using the command:

    $ make BOARD=QiMing_F103RC deploy

This will use the included `tools/pydfu.py` script.  You can use instead the
`dfu-util` program (available [here](http://dfu-util.sourceforge.net/)) by
passing `USE_PYDFU=0`:

    $ make BOARD=QiMing_F103RC USE_PYDFU=0 deploy

If flashing the firmware does not work it may be because you don't have the
correct permissions.  Try then:

    $ sudo make BOARD=QiMing_F103RC deploy

Or using `dfu-util` directly:

    $ sudo dfu-util -a 0 -d 0483:df11 -D build-QiMing_F103RC/firmware.dfu


### Flashing the Firmware with stlink

No Test.

### Flashing the Firmware with OpenOCD

Another option to deploy the firmware on STM32F103xx boards with a
ST-LINK interface uses [OpenOCD](http://openocd.org/). Connect the board with
a mini USB cable to its ST-LINK USB port and then use the make target
`deploy-openocd`. For example, if you have the STM32F1DISCOVERY board:

    $ make BOARD=QiMing_F103RC deploy-openocd

The `openocd` program, which writes the firmware to the target board's flash,
is configured via the file `ports/stm32f1/boards/openocd_stm32f1.cfg`. This
configuration should work for all boards based on a STM32F1xx MCU with a
ST-LINKv2 interface. You can override the path to this configuration by setting
`OPENOCD_CONFIG` in your Makefile or on the command line.

Accessing the board
-------------------

Once built and deployed, access the MicroPython REPL (the Python prompt) via USB
serial or UART, depending on the board.  For the pyboard you can try:

    $ picocom /dev/ttyACM0

On MacOS, you can try:

	$ screen /dev/tty.xxxxx 115200