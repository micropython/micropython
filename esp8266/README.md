MicroPython port to ESP8266
===========================

This is an experimental port of MicroPython for the WiFi modules based
on Espressif ESP8266 chip.

WARNING: The port is experimental and many APIs are subject to change.

Supported features include:
- REPL (Python prompt) over UART0.
- Garbage collector, exceptions.
- Unicode support.
- Builtin modules: gc, array, collections, io, struct, sys, esp, network,
  many more.
- Arbitrary-precision long integers and 30-bit precision floats.
- WiFi support.
- Sockets using modlwip.
- GPIO and bit-banging I2C, SPI support.
- 1-Wire and WS2812 (aka Neopixel) protocols support.
- Internal filesystem using the flash.
- WebREPL over WiFi from a browser (clients at https://github.com/micropython/webrepl).
- Modules for HTTP, MQTT, many other formats and protocols via
  https://github.com/micropython/micropython-lib .

Work-in-progress documentation is available at
http://docs.micropython.org/en/latest/esp8266/ .

Build instructions
------------------

The tool chain required for the build is the OpenSource ESP SDK, which can be
found at <https://github.com/pfalcon/esp-open-sdk>.  Clone this repository and
run `make` in its directory to build and install the SDK locally.  Make sure
to add toolchain bin directory to your PATH.  Read esp-open-sdk's README for
additional important information on toolchain setup.

Add the external dependencies to the MicroPython repository checkout:
```bash
$ git submodule update --init
```
See the README in the repository root for more information about external
dependencies.

The MicroPython cross-compiler must be built to pre-compile some of the
built-in scripts to bytecode.  This can be done using:
```bash
$ make -C mpy-cross
```

Then, to build MicroPython for the ESP8266, just run:
```bash
$ cd esp8266
$ make axtls
$ make
```
This will produce binary images in the `build/` subdirectory. If you install
MicroPython to your module for the first time, or after installing any other
firmware, you should erase flash completely:

```
esptool.py --port /dev/ttyXXX erase_flash
```

Erase flash also as a troubleshooting measure, if a module doesn't behave as
expected.

To flash MicroPython image to your ESP8266, use:
```bash
$ make deploy
```
This will use the `esptool.py` script to download the images.  You must have
your ESP module in the bootloader mode, and connected to a serial port on your PC.
The default serial port is `/dev/ttyACM0`, flash mode is `qio` and flash size is
`detect` (auto-detect based on Flash ID). To specify other values, use, eg (note
that flash size is in megabits):
```bash
$ make PORT=/dev/ttyUSB0 FLASH_MODE=qio FLASH_SIZE=32m deploy
```

The image produced is `build/firmware-combined.bin`, to be flashed at 0x00000.

First start
-----------

__Serial prompt__

You can access the REPL (Python prompt) over UART (the same as used for
programming).
- Baudrate: 115200

__WiFi__

Initally, the device configures itself as a WiFi access point (AP).
- ESSID: MicroPython-xxxxxx (x’s are replaced with part of the MAC address).
- Password: micropythoN (note the upper-case N).
- IP address of the board: 192.168.4.1.
- DHCP-server is activated.

__WebREPL__

Python prompt over WiFi, connecting through a browser.
- Hosted at http://micropython.org/webrepl.
- GitHub repository https://github.com/micropython/webrepl.

Please follow the instructions there.

More detailed instructions can be found at
http://docs.micropython.org/en/latest/esp8266/esp8266/tutorial/intro.html

Troubleshooting
---------------

While the port is still in alpha, it's known to be generally stable. If you
experience strange bootloops, crashes, lockups, here's a list to check against:

- You didn't erase flash before programming MicroPython firmware.
- Firmware can be occasionally flashed incorrectly. Just retry. Recent
  esptool.py versions have --verify option.
- Power supply you use doesn't provide enough power for ESP8266 or isn't
  stable enough.
- A module/flash may be defective (not unheard of for cheap modules).

Please consult dedicated ESP8266 forums/resources for hardware-related
problems.
