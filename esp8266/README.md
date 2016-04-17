MicroPython port to ESP8266
===========================

This is a highly experimental port of MicroPython for the WiFi modules based
on Espressif ESP8266 chip.

WARNING: The port is highly experimental and any APIs are subject to change.

Currently implemented features include:
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

Then, to build MicroPython for the ESP8266, just run:
```bash
$ cd esp8266
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
The default serial port is `/dev/ttyACM0`.  To specify another, use, eg:
```bash
$ make PORT=/dev/ttyUSB0 deploy
```

The image produced is `firmware-combined.bin`, to be flashed at 0x00000.

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
