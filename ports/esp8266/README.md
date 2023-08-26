MicroPython port to ESP8266
===========================

This is a port of MicroPython to the Espressif ESP8266 WiFi microcontroller.
MicroPython runs on this chip without any underlying operating system, using
the ESP8266 NONOS SDK.

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

You need the esp-open-sdk toolchain, which provides both the compiler and libraries.

There are two ways to do this:
 - By running the toolchain in [Docker](https://www.docker.com/) (**recommended**).
 - By installing a pre-built toolchain and adding it to your `$PATH`.

Regardless of which toolchain you use, the first step is to make sure required
submodules are available:

```bash
$ make -C ports/esp8266 submodules
```

See the README in the repository root for more information about external
dependencies.

__Building with Docker__

Once you have installed Docker, you can run all of the following build
commands inside the Docker container by prefixing them with `docker
run --rm -v $HOME:$HOME -u $UID -w $PWD larsks/esp-open-sdk ...command...`.
This will automatically download the Docker image provided by @larsks which
contains the full toolchain and SDK.

Then you need to compile the MicroPython cross-compiler (`mpy-cross`). From
the root of this repository, run:

```bash
$ docker run --rm -v $HOME:$HOME -u $UID -w $PWD larsks/esp-open-sdk make -C mpy-cross
```

**Note:** The `mpy-cross` binary will likely only work inside the Docker
container. This will not be a problem if you're only building ESP8266
firmware, but if you're also working on other ports then you will need to
recompile for your host when switching between ports. To avoid this, use
the local toolchain instead.

Then to compile the ESP8266 firmware:

```
$ cd ports/esp8266
$ docker run --rm -v $HOME:$HOME -u $UID -w $PWD larsks/esp-open-sdk make -j BOARD=GENERIC
```

This will produce binary images in the `build-GENERIC/` subdirectory.
Substitute the board for whichever board you're using.

__Building with a local toolchain__

First download the pre-built toolchain (thanks to @jepler from Adafruit). You
will need to find somewhere to put it in your filesystem, e.g. `~/espressif`.
Create that directory first if necessary.

```
$ cd ~/espressif # Change as necessary
$ wget https://github.com/jepler/esp-open-sdk/releases/download/2018-06-10/xtensa-lx106-elf-standalone.tar.gz
$ tar zxvf xtensa-lx106-elf-standalone.tar.gz
$ rm xtensa-lx106-elf/bin/esptool.py  # Use system version of esptool.py instead.
```

Then append this to your `$PATH` variable so the compiler binaries can be
found:

```
$ export "PATH=$HOME/espressif/xtensa-lx106-elf/bin/:$PATH"
```

(You will need to do this each time you start a new terminal)

Then you need to compile the MicroPython cross-compiler (`mpy-cross`). From
the root of this repository, run:

```bash
$ make -C mpy-cross
```

Then to compile the ESP8266 firmware:

```
$ cd ports/esp8266
$ make -j BOARD=GENERIC
```

This will produce binary images in the `build-GENERIC/` subdirectory.
Substitute the board for whichever board you're using.


Installing MicroPython
----------------------

To communicate with the board you will need to install `esptool.py`. This can
be obtained from your system package manager or from PyPi via `pip`.

If you install MicroPython to your module for the first time, or after
installing any other firmware, you should erase flash completely:

```bash
$ esptool.py --port /dev/ttyXXX erase_flash
```

Erasing the flash is also useful as a troubleshooting measure, if a module doesn't
behave as expected.

To flash MicroPython image to your ESP8266, use:
```bash
$ make deploy
```

(If using the Docker instructions above, do not run this command via Docker as
it will need access to the serial port. Run it directly instead.)

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

__mip__

The ESP8266 port comes with the built-in `mip` package manager, which can
be used to install additional modules:

```
>>> import mip
>>> mip.install("hmac")
[...]
>>> import hmac
>>> hmac.new(b"1234567890", msg="hello world").hexdigest()
```

See [Package management](https://docs.micropython.org/en/latest/reference/packages.html) for more
information about `mip`.

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
