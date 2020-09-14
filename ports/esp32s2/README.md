# Circuitpython on ESP32-S2 #

This port adds the ESP32-S2 line of modules from Espressif to Circuitpython. ESP32-S2 modules are low power, single-core Wi-Fi microcontroller SoCs designed for IoT applications.

## How this port is organized: ##

- **bindings/** contains some required bindings to the ESP-IDF for exceptions and memory.
- **boards/** contains the configuration files for each development board and breakout available on the port.
- **common-hal/** contains the port-specific module implementations, used by shared-module and shared-bindings.
- **esp-idf/** contains the Espressif IoT development framework installation, includign all the drivers for the port.
- **modules/** contains information specific to certain ESP32-S2 hardware modules, such as the pins used for flash and RAM on the WROVER and WROOM.
- **peripherals/** contains peripheral setup files and peripheral mapping information, sorted by family and sub-variant. Most files in this directory can be generated with the python scripts in **tools/**.
- **supervisor/** contains port-specific implementations of internal flash, serial and USB, as well as the **port.c** file, which initializes the port at startup.
- **tools/** includes useful python scripts for debugging and other purposes.

At the root level, refer to **mpconfigboard.h** and **mpconfigport.mk** for port specific settings and a list of enabled circuitpython modules.

## Connecting to the ESP32-S2 ##

The USB port built into ESP32-S2 boards such as the Saola is not the native USB of the board, but a debugging and programming interface. The actual ESP32-S2 native USB which exposes the Circuitpython drive and CDC connection is located on IO pins 19 and 20:

| GPIO | USB         |
| ---- | ----------- |
| 20   | D+ (green)  |
| 19   | D- (white)  |
| GND  | GND (black) |
| 5V   | +5V (red)   |

Connect these pins using a [USB adapter](https://www.adafruit.com/product/4090) or [breakout cable](https://www.adafruit.com/product/4448) to access the Circuitpython drive.

## Building and flashing ##

Before building or flashing the ESP32-S2, you must [install the esp-idf](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html). This must be re-done ever time the esp-idf is updated, but not every time you build. From the port root, run:

```
./esp-idf/install.sh
```

Additionally, any time you open a new bash environment for building or flashing, you must add the esp-idf tools to your path:

```
. esp-idf/export.sh
```

To build for a board such as the Saola 1 Wrover and flash it over the UART interface, run the following:

```
make BOARD=espressif_saola_1_wrover flash
```

On MacOS, you may need to directly specify the port. You can determine the correct port on your machine by running `ls /dev/tty.usb*` and selecting the one that appears only when your development board is plugged in. Example:

```
make BOARD=espressif_saola_1_wrover flash PORT=/dev/tty.usbserial-1421120
```

## Debugging ##

The ESP32-S2 supports JTAG debugging over OpenOCD using a JLink or other probe hardware. The official tutorials can be found on the Espressif website [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-guides/jtag-debugging/index.html), but they are mostly for the ESP32-S2 Kaluga, which has built-in debugging.

OpenOCD is automatically installed and added to your bash environment during the esp-idf installation and setup process. You can double check that it is installed by using `openocd --version`, as per the tutorial. Attach the JTAG probe pins according to the [instructions](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-guides/jtag-debugging/configure-other-jtag.html) for JTAG debugging on boards that do not contain an integrated debugger.

Once the debugger is connected physically, you must run OpenOCD with attached configuration files specifying the **interface** (your debugger probe) and either a **target** or a **board** (targets are for SoCs only, and can be used when a full board configuration file doesn't exist). You can find the path location of these files by checking the `OPENOCD_SCRIPTS` environmental variable by running `echo $OPENOCD_SCRIPTS` in bash. Interfaces will be in the `interface/` directory, and targets and boards in the `target/` and `board/` directories, respectively.

**Note:** Unfortunately, there are no board files for the esp32-s2 other than the Kaluga, and the included `target/esp32s2.cfg` target file will not work by default on the Jlink for boards like the Saola 1, as the default speed is incorrect. In addition, these files are covered under the GPL and cannot be included in Circuitpython. Thus, you must make a copy of the esp32s2.cfg file yourself and add the following line manually, under `transport select jtag` at the start of the file:

```
adapter_khz 1000
```

Once this is complete, your final OpenOCD command may look something like this:

`openocd -f interface/jlink.cfg -f SOMEPATH/copied-esp32s2-saola-1.cfg`

Where `SOMEPATH` is the location of your copied configuration file (this can be placed in the port/boards director with a prefix to ignore it with `.gitignore`, for instance). Interface, target and board config files sourced from espressif only need their paths from the $OPENOCD_SCRIPTS location, you don't need to include their full path. Once OpenOCD is running, connect to GDB with:

`xtensa-esp32s2-elf-gdb build-espressif_saola_1_wrover/firmware.elf`

And follow the Espressif GDB tutorial [instructions](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/api-guides/jtag-debugging/using-debugger.html) for connecting, or add them to your `gdbinit`:

```
target remote :3333
set remote hardware-watchpoint-limit 2
mon reset halt
flushregs
thb app_main
c
```
