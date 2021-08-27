# Circuitpython on STM32 #

This port brings the ST Microelectronics STM32 series of MCUs to Circuitpython. STM32 chips have a wide range of capability, from <$1 low power STM32F0s to dual-core STM32H7s running at 400+ MHz. Currently, only the F4, F7, and H7 families are supported, powered by the ARM Cortex M4 and M7 processors.

Refer to the ST Microelectronics website for more information on features sorted by family and individual chip lines: [st.com/en/microcontrollers-microprocessors/stm32-high-performance-mcus.html](https://www.st.com/en/microcontrollers-microprocessors/stm32-high-performance-mcus.html)

STM32 SoCs vary product-by-product in clock speed, peripheral capability, pin assignments, and their support within this port. Refer to **mpconfigport.mk** for a full list of enabled modules sorted by family.

## How this port is organized: ##

- **boards/** contains the configuration files for each development board and breakout available on the port, as well as system files and both shared and SoC-specific linker files. Board configuration includes a pin mapping of the board, oscillator information, board-specific build flags, and setup for OLED or TFT screens where applicable.
- **common-hal/** contains the port-specific module implementations, used by shared-module and shared-bindings.
- **packages/** contains package-specific pin bindings (LQFP100, BGA216, etc)
- **peripherals/** contains peripheral setup files and peripheral mapping information, sorted by family and sub-variant. Most files in this directory can be generated with the python scripts in **tools/**.
- **st-driver/** submodule for ST HAL and LL files generated via CubeMX. Shared with TinyUSB.
- **supervisor/** contains port-specific implementations of internal flash, serial and USB, as well as the **port.c** file, which initializes the port at startup.
- **tools/** python scripts for generating peripheral and pin mapping files in **peripherals/** and **board/**.

At the root level, refer to **mpconfigboard.h** and **mpconfigport.mk** for port specific settings and a list of enabled modules.

## Build instructions ##

Ensure your clone of Circuitpython is ready to build by following the [guide on the Adafruit Website](https://learn.adafruit.com/building-circuitpython/introduction). This includes installing the toolchain, synchronizing submodules, and running `mpy-cross`.

Once the one-time build tasks are complete, you can build at any time by navigating to the port directory:

    $ cd ports/stm

To build for a specific circuitpython board, run:

    $ make BOARD=feather_stm32f405_express

You may also build with certain flags available in the makefile, depending on your board and development goals. The following flags would enable debug information and correct flash locations for a pre-flashed UF2 bootloader:

    $ make BOARD=feather_stm32f405_express DEBUG=1 UF2_BOOTLOADER=1

## USB connection ##

Connect your development board of choice to the host PC via the USB cable. Note that for most ST development boards such as the Nucleo and Discovery series, you must use a secondary OTG USB connector to access circuitpython, as the primary USB connector will be connected to a built-in ST-Link debugger rather than the chip itself.

In many cases, this ST-Link USB connector will **still need to be connected to power** for the chip to turn on - refer to your specific product manual for details.

## Flash the bootloader ##

Most ST development boards come with a built-in STLink programming and debugging probe accessible via USB. This programmer may show up as an `MBED` drive on the host PC, enabling simple drag and drop programming with a .bin file, or they may require a tool like [OpenOCD](http://openocd.org/) or [StLink-org/stlink](https://github.com/stlink-org/stlink) to run flashing and debugging commands.

Many hobbyist and 3rd party development boards also expose SWD pins. These can be used with a cheap [stlink](https://www.adafruit.com/product/2548) debugger or other common programmers.

For non-ST products or users without a debugger, all STM32 boards in the high performance families (F4, F7 and H7) include a built-in DFU bootloader stored in ROM. This bootloader is accessed by ensuring the BOOT0 pin is held to a logic 1 and the BOOT1 pin is held to a logic 0 when the chip is reset ([ST Appnote AN2606](https://www.st.com/resource/en/application_note/cd00167594-stm32-microcontroller-system-memory-boot-mode-stmicroelectronics.pdf)). Most chips hold BOOT low by default, so this can usually be achieved by running a jumper wire from 3.3V power to the BOOT0 pin, if it is exposed, or by flipping the appropriate switch or button as the chip is reset. Once the chip is started in DFU mode, BOOT0 no longer needs to be held high and can be released. An example is available in the [Feather STM32F405 guide](https://learn.adafruit.com/adafruit-stm32f405-feather-express/dfu-bootloader-details).

Windows users will need to install [stm32cubeprog](https://www.st.com/en/development-tools/stm32cubeprog.html), while Mac and Linux users will need to install `dfu-util` with `brew install dfu-util` or `sudo apt-get install dfu-util`. More details are available in the [Feather F405 guide](https://learn.adafruit.com/adafruit-stm32f405-feather-express/dfu-bootloader-details).

## Flashing the circuitpython image with DFU-Util ##

Ensure the board is in dfu mode by following the steps in the previous section. Then run:

    $ make BOARD=feather_stm32F405_express flash

Alternatively, you can navigate to the build directory and run the raw `dfu-util` command:

`dfu-util -a 0 --dfuse-address 0x08000000 -D firmware.bin`

## Accessing the board ##

Connecting the board to the PC via the USB cable will allow code to be uploaded to the `CIRCUITPY` volume.

Circuitpython exposes a CDC virtual serial connection for REPL access and debugging. Connecting to it from OSX will look something like this:

    screen /dev/tty.usbmodem14111201 115200

You may also use a program like [mu](https://codewith.mu/) to assist with REPL access.
