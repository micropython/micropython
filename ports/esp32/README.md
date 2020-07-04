MicroPython port to the ESP32
=============================

This is an experimental port of MicroPython to the Espressif ESP32
microcontroller.  It uses the ESP-IDF framework and MicroPython runs as
a task under FreeRTOS.

Supported features include:
- REPL (Python prompt) over UART0.
- 16k stack for the MicroPython task and 96k Python heap.
- Many of MicroPython's features are enabled: unicode, arbitrary-precision
  integers, single-precision floats, complex numbers, frozen bytecode, as
  well as many of the internal modules.
- Internal filesystem using the flash (currently 2M in size).
- The machine module with GPIO, UART, SPI, software I2C, ADC, DAC, PWM,
  TouchPad, WDT and Timer.
- The network module with WLAN (WiFi) support.

Development of this ESP32 port was sponsored in part by Microbric Pty Ltd.

Setting up the toolchain and ESP-IDF
------------------------------------

There are two main components that are needed to build the firmware:
- the Xtensa cross-compiler that targets the CPU in the ESP32 (this is
  different to the compiler used by the ESP8266)
- the Espressif IDF (IoT development framework, aka SDK)

If you are on a Windows machine then the [Windows Subsystem for
Linux](https://msdn.microsoft.com/en-au/commandline/wsl/install_guide) is the
most efficient way to install the ESP32 toolchain and build the project. If
you use WSL then follow the Linux instructions rather than the Windows
instructions.

The ESP-IDF changes quickly and MicroPython only supports certain versions.
The git hash of these versions (one for 3.x, one for 4.x) can be found by
running `make` without a configured `ESPIDF`. Then you can fetch the
required IDF using the following command:

```bash
$ cd ports/esp32
$ make ESPIDF=  # This will print the supported hashes, copy the one you want.
$ export ESPIDF=$HOME/src/github.com/espressif/esp-idf  # Or any path you like.
$ mkdir -p $ESPIDF
$ cd $ESPIDF
$ git clone https://github.com/espressif/esp-idf.git $ESPIDF
$ git checkout <Current supported ESP-IDF commit hash>
$ git submodule update --init --recursive
```

Note: The ESP IDF v4.x support is currently experimental. It does not
currently support PPP or wired Ethernet.

Python dependencies
===================

You will also need other dependencies from the IDF, see
`$ESPIDF/requirements.txt`, but at a minimum you need `pyserial>=3.0` and
`pyparsing>=2.0.3,<2.4.0`.

You can use Python 2 or Python 3. If you need to override the system default
add (for example) `PYTHON=python3` to any of the `make` commands below.

It is recommended to use a Python virtual environment. Even if your system
package manager already provides these libraries, the IDF v4.x is currently
incompatible with pyparsing 2.4 and higher.

For example, to set up a Python virtual environment from scratch:

```bash
$ cd ports/esp32
$ python3 -m venv build-venv
$ source build-venv/bin/activate
$ pip install --upgrade pip
$ pip install -r path/to/esp-idf/requirements.txt
```

To re-enter this virtual environment in future sessions, you only need to
source the `activate` script, i.e.:

```bash
$ cd ports/esp32
$ source build-venv/bin/activate
```

Then, to install the toolchain (which includes the GCC compiler, linker, binutils,
etc), there are two options:

1. Using the IDF scripts to install the toolchain (IDF 4.x only)
================================================================

Follow the steps at the [Espressif Getting Started guide](https://docs.espressif.com/projects/esp-idf/en/v4.0/get-started/index.html#step-3-set-up-the-tools).

This will guide you through using the `install.sh` (or `install.bat`) script
to download the toolchain and add it to your `PATH`. The steps are summarised
below:

After you've cloned and checked out the IDF to the correct version (see
above), run the `install.sh` script:

```bash
$ cd $ESPIDF
$ ./install.sh    # (or install.bat on Windows)
```

Then in the `ports/esp32` directory, source the `export.sh` script to set the
`PATH`.

```bash
$ cd micropython/ports/esp32
$ source $ESPIDF/export.sh   # (or path\to\esp-idf\export.bat on Windows)
$ # Run make etc, see below.
```

The `install.sh` step only needs to be done once. You will need to source
`export.sh` for every new session.

Note: If you get an error about `--no-site-packages`, then modify
`$ESPIDF/tools/idf_tools.py` and make the same change as [this
commit](https://github.com/espressif/esp-idf/commit/7a18f02acd7005f7c56e62175a8d1968a1a9019d).

2. or, Downloading pre-built toolchain manually (IDF 3.x and 4.x)
=============================================================

Note: while this works with 4.x, if you're using the 4.x IDF, it's much
simpler to use the guide above, which will also get a more recent version of
the toolchain.

You can follow the 3.x guide at:

  * [Linux installation](https://docs.espressif.com/projects/esp-idf/en/v3.3.2/get-started/linux-setup.html)
  * [MacOS installation](https://docs.espressif.com/projects/esp-idf/en/v3.3.2/get-started/macos-setup.html)
  * [Windows installation](https://docs.espressif.com/projects/esp-idf/en/v3.3.2/get-started/windows-setup.html)

You will need to update your `PATH` environment variable to include the ESP32
toolchain. For example, you can issue the following commands on (at least)
Linux:

    $ export PATH=$PATH:$HOME/esp/crosstool-NG/builds/xtensa-esp32-elf/bin

You can put this command in your `.profile` or `.bash_login`, or do it manually.

Configuring the MicroPython build
---------------------------------

You then need to set the `ESPIDF` environment/makefile variable to point to
the root of the ESP-IDF repository. The recommended way to do this is to have
a custom `makefile` in `ports/esp32` which sets any additional variables, then
includes the main `Makefile`. Note that GNU Make will preferentially run
`GNUmakefile`, then `makefile`, then `Makefile`, which is what allows this to
work. On case-insensitive filesystems, you'll need to use `GNUmakefile` rather
than `makefile`.

Create a new file in the esp32 directory called `makefile` (or `GNUmakefile`)
and add the following lines to that file:

```
ESPIDF ?= <path to root of esp-idf repository>
BOARD ?= GENERIC
#PORT ?= /dev/ttyUSB0
#FLASH_MODE ?= qio
#FLASH_SIZE ?= 4MB
#CROSS_COMPILE ?= xtensa-esp32-elf-

include Makefile
```

Be sure to enter the correct path to your local copy of the IDF repository
(and use `$(HOME)`, not tilde (`~`), to reference your home directory).

If the Xtensa cross-compiler is not in your path you can use the
`CROSS_COMPILE` variable to set its location.  Other options of interest are
`PORT` for the serial port of your ESP32 module, and `FLASH_MODE` (which may
need to be `dio` for some modules) and `FLASH_SIZE`.  See the Makefile for
further information.

The default ESP IDF configuration settings are provided by the `GENERIC`
board definition in the directory `boards/GENERIC`. For a custom configuration
you can define your own board directory.

Any of these variables can also be set on the make command line, e.g. to set
the `BOARD` variable, use:

```bash
$ make BOARD=TINYPICO
```

Note the use of `?=` in the `makefile` which allows them to be overridden on
the command line. There is also a `GENERIC_SPIRAM` board for for ESP32
modules that have external SPIRAM, but prefer to use a specific board target
(or define your own as necessary).

Building the firmware
---------------------

The MicroPython cross-compiler must be built to pre-compile some of the
built-in scripts to bytecode.  This can be done by (from the root of
this repository):

```bash
$ cd mpy-cross
$ make mpy-cross
```

Then to build MicroPython for the ESP32 run:

```bash
$ cd ports/esp32
$ make submodules
$ make
```

This will produce binary firmware images in the `build/` subdirectory
(three of them: bootloader.bin, partitions.bin and application.bin).

To flash the firmware you must have your ESP32 module in the bootloader
mode and connected to a serial port on your PC.  Refer to the documentation
for your particular ESP32 module for how to do this.  The serial port and
flash settings are set in the `Makefile`, and can be overridden in your
local `makefile`; see above for more details.

You will also need to have user permissions to access the /dev/ttyUSB0 device.
On Linux, you can enable this by adding your user to the `dialout` group, and
rebooting or logging out and in again. (Note: on some distributions this may
be the `uucp` group, run `ls -la /dev/ttyUSB0` to check.)

```bash
$ sudo adduser <username> dialout
```

If you are installing MicroPython to your module for the first time, or
after installing any other firmware, you should first erase the flash
completely:

```bash
$ make erase
```

To flash the MicroPython firmware to your ESP32 use:

```bash
$ make deploy
```

This will use the `esptool.py` script (provided by ESP-IDF) to flash the
binary images to the device.

Getting a Python prompt on the device
-------------------------------------

You can get a prompt via the serial port, via UART0, which is the same UART
that is used for programming the firmware.  The baudrate for the REPL is
115200 and you can use a command such as:

```bash
$ picocom -b 115200 /dev/ttyUSB0
```

or

```bash
$ miniterm.py /dev/ttyUSB0 115200
```

Configuring the WiFi and using the board
----------------------------------------

The ESP32 port is designed to be (almost) equivalent to the ESP8266 in
terms of the modules and user-facing API.  There are some small differences,
notably that the ESP32 does not automatically connect to the last access
point when booting up.  But for the most part the documentation and tutorials
for the ESP8266 should apply to the ESP32 (at least for the components that
are implemented).

See http://docs.micropython.org/en/latest/esp8266/esp8266/quickref.html for
a quick reference, and http://docs.micropython.org/en/latest/esp8266/esp8266/tutorial/intro.html
for a tutorial.

The following function can be used to connect to a WiFi access point (you can
either pass in your own SSID and password, or change the defaults so you can
quickly call `wlan_connect()` and it just works):
```python
def wlan_connect(ssid='MYSSID', password='MYPASS'):
    import network
    wlan = network.WLAN(network.STA_IF)
    if not wlan.active() or not wlan.isconnected():
        wlan.active(True)
        print('connecting to:', ssid)
        wlan.connect(ssid, password)
        while not wlan.isconnected():
            pass
    print('network config:', wlan.ifconfig())
```

Note that some boards require you to configure the WiFi antenna before using
the WiFi.  On Pycom boards like the LoPy and WiPy 2.0 you need to execute the
following code to select the internal antenna (best to put this line in your
boot.py file):
```python
import machine
antenna = machine.Pin(16, machine.Pin.OUT, value=0)
```

Troubleshooting
---------------

* Continuous reboots after programming: Ensure FLASH_MODE is correct for your
  board (e.g. ESP-WROOM-32 should be DIO). Then perform a `make clean`, rebuild,
  redeploy.
