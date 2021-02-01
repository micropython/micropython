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

Documentation is available at http://docs.micropython.org/en/latest/esp8266/quickref.html.

Build instructions
------------------

You need the esp-open-sdk toolchain (which provides both the compiler and libraries), which
you can obtain using one of the following two options:

 - Use a Docker image with a pre-built toolchain (**recommended**).
   To use this, install Docker, then prepend
   `docker run --rm -v $HOME:$HOME -u $UID -w $PWD larsks/esp-open-sdk ` to the start
   of the mpy-cross and firmware `make` commands below. This will run the commands using the
   toolchain inside the container but using the files on your local filesystem.

 - or, install the esp-open-sdk directly on your PC, which can be found at
   <https://github.com/pfalcon/esp-open-sdk>. Clone this repository and
   run `make` in its directory to build and install the SDK locally.  Make sure
   to add toolchain bin directory to your PATH.  Read esp-open-sdk's README for
   additional important information on toolchain setup.
   If you use this approach, then the command below will work exactly.

Add the external dependencies to the MicroPython repository checkout:
```bash
$ make -C ports/esp8266 submodules
```
See the README in the repository root for more information about external
dependencies.

The MicroPython cross-compiler must be built to pre-compile some of the
built-in scripts to bytecode.  This can be done using:
```bash
$ make -C mpy-cross
```
(Prepend the Docker command if using Docker, see above)

Then, to build MicroPython for the ESP8266, just run:
```bash
$ cd ports/esp8266
$ make
```
(Prepend the Docker command if using Docker, see above)

This will produce binary images in the `build-GENERIC/` subdirectory. If you
install MicroPython to your module for the first time, or after installing any
other firmware, you should erase flash completely:
```bash
$ esptool.py --port /dev/ttyXXX erase_flash
```

You can install esptool.py either from your system package manager or from PyPi.

Erasing the flash is also useful as a troubleshooting measure, if a module doesn't
behave as expected.

To flash MicroPython image to your ESP8266, use:
```bash
$ make deploy
```
(This should not be run inside Docker as it will need access to the serial port.)

This will use the `esptool.py` script to download the images.  You must have
your ESP module in the bootloader mode, and connected to a serial port on your PC.
The default serial port is `/dev/ttyACM0`, flash mode is `qio` and flash size is
`detect` (auto-detect based on Flash ID).

To specify other values for `esptool.py`, use, e.g.:
```bash
$ make PORT=/dev/ttyUSB0 FLASH_MODE=qio FLASH_SIZE=32m deploy
```
(note that flash size is in megabits)

If you want to flash manually using `esptool.py` directly, the image produced is
`build-GENERIC/firmware-combined.bin`, to be flashed at 0x00000.

The default board definition is the directory `boards/GENERIC`.
For a custom configuration you can define your own board in the directory `boards/`.

The `BOARD` variable can be set on the make command line, for example:
```bash
$ make BOARD=GENERIC_512K
```

__512KB FlashROM version__

The normal build described above requires modules with at least 1MB of FlashROM
onboard. There's a special configuration for 512KB modules, which can be
built with `make BOARD=GENERIC_512K`. This configuration is highly limited, lacks
filesystem support, WebREPL, and has many other features disabled. It's mostly
suitable for advanced users who are interested to fine-tune options to achieve a
required setup. If you are an end user, please consider using a module with at
least 1MB of FlashROM.

First start
-----------

Be sure to change ESP8266's WiFi access point password ASAP, see below.

__Serial prompt__

You can access the REPL (Python prompt) over UART (the same as used for
programming).
- Baudrate: 115200

Run `help()` for some basic information.

__WiFi__

Initially, the device configures itself as a WiFi access point (AP).
- ESSID: MicroPython-xxxxxx (x’s are replaced with part of the MAC address).
- Password: micropythoN (note the upper-case N).
- IP address of the board: 192.168.4.1.
- DHCP-server is activated.
- Please be sure to change the password to something non-guessable
  immediately. `help()` gives information how.

__WebREPL__

Python prompt over WiFi, connecting through a browser.
- Hosted at http://micropython.org/webrepl.
- GitHub repository https://github.com/micropython/webrepl.
  Please follow the instructions there.

__upip__

The ESP8266 port comes with builtin `upip` package manager, which can
be used to install additional modules (see the main README for more
information):

```
>>> import upip
>>> upip.install("micropython-pystone_lowmem")
[...]
>>> import pystone_lowmem
>>> pystone_lowmem.main()
```

Downloading and installing packages may requite a lot of free memory,
if you get an error, retry immediately after the hard reset.

Documentation
-------------

More detailed documentation and instructions can be found at
http://docs.micropython.org/en/latest/esp8266/ , which includes Quick
Reference, Tutorial, General Information related to ESP8266 port, and
to MicroPython in general.

Troubleshooting
---------------

While the port is in beta, it's known to be generally stable. If you
experience strange bootloops, crashes, lockups, here's a list to check against:

- You didn't erase flash before programming MicroPython firmware.
- Firmware can be occasionally flashed incorrectly. Just retry. Recent
  esptool.py versions have --verify option.
- Power supply you use doesn't provide enough power for ESP8266 or isn't
  stable enough.
- A module/flash may be defective (not unheard of for cheap modules).

Please consult dedicated ESP8266 forums/resources for hardware-related
problems.

Additional information may be available by the documentation links above.
