MicroPython port to STM32F103xx MCUs
==============================

This directory contains the port of MicroPython to ST's line of STM32F103
microcontrollers. Parts of the code here utilise the STM32Cube HAL library.

> Other STM32 mcus see [STM32 port README](ports/stm32/README.md)

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

and next step you must copy files to `lib/stm32lib/` from STM32CUBE Repository:

```bash
$ export STM32CUBE_REPO="path to STM32Cube repo root"
$ export STM32CUBE_F1=${STM32CUBE_REPO}/STM32Cube_FW_F1_V1.8.0

$ mkdir -p lib/stm32lib/CMSIS/STM32F1xx
$ mkdir -p lib/stm32lib/STM32F1xx_HAL_Driver

$ cp -r ${STM32CUBE_F1}/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
        lib/stm32lib/CMSIS/STM32F1xx/

$ cp -r ${STM32CUBE_F1}/Drivers/STM32F1xx_HAL_Driver/Inc \
        lib/stm32lib/STM32F1xx_HAL_Driver/
$ cp -r ${STM32CUBE_F1}/Drivers/STM32F1xx_HAL_Driver/Src \
        lib/stm32lib/STM32F1xx_HAL_Driver/
```


An ARM compiler is required for the build, along with the associated binary
utilities.  The default compiler is `arm-none-eabi-gcc`, which is available for
Arch Linux via the package `arm-none-eabi-gcc`, for Ubuntu via instructions
[here](https://launchpad.net/~team-gcc-arm-embedded/+archive/ubuntu/ppa), or
see [here](https://launchpad.net/gcc-arm-embedded) for the main GCC ARM
Embedded page.  The compiler can be changed using the `CROSS_COMPILE` variable
when invoking `make`.

To build for a given board, run:

    $ make BOARD=QiMingF103RC

The default board is QiMingF103RC but any of the names of the subdirectories in the
`boards/` directory can be passed as the argument to `BOARD=`.  The above command
should produce binary images in the `build-QiMingF103RC/` subdirectory (or the
equivalent directory for the board specified).

### Flashing the Firmware with stlink

No Test.

### Flashing the Firmware with OpenOCD

Another option to deploy the firmware on STM32F103xx boards with a
ST-LINK interface uses [OpenOCD](http://openocd.org/). Connect the board with
a mini USB cable to its ST-LINK USB port and then use the make target
`deploy-openocd`. For example, if you have the STM32F1DISCOVERY board:

    $ make BOARD=QiMingF103RC deploy-openocd

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

Some Changes to Official
------------------

- class `pyb.RTC` (or `machine.RTC`)
    - `RTC`.`datetime(` [datetimetuple] `)`
        > the `subseconds` not used when set datetime
    - `RTC`.`wakeup(timeout, callback=None)`
        > `timeout`'s unit is `ms`, but used as second(timeout/1000)

        > wakeup just run once(stm32f1xx has no WAKEUP Function)
    - `RTC`.`calibration(cal)`
        > cal in range [0, 0x7F], that means the clock can be slowed down by up to 121 ppm

        >other valid value is:
        - 0x80: turn off (PC13) 1Hz output
        - 0x81: trun on (PC13) 1Hz output
- The `USB` and `CAN` can't be used at the same time
- The `network/socket` modules is not support