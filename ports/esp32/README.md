MicroPython port to the ESP32
=============================

This is a port of MicroPython to the Espressif ESP32 series of
microcontrollers.  It uses the ESP-IDF framework and MicroPython runs as
a task under FreeRTOS.

Currently supports ESP32, ESP32-C3, ESP32-C6, ESP32-S2 and ESP32-S3
(ESP8266 is supported by a separate MicroPython port).

Supported features include:
- REPL (Python prompt) over UART0.
- 16k stack for the MicroPython task and approximately 100k Python heap.
- Many of MicroPython's features are enabled: unicode, arbitrary-precision
  integers, single-precision floats, complex numbers, frozen bytecode, as
  well as many of the internal modules.
- Internal filesystem using the flash (currently 2M in size).
- The machine module with GPIO, UART, SPI, software I2C, ADC, DAC, PWM,
  TouchPad, WDT and Timer.
- The network module with WLAN (WiFi) support.
- Bluetooth low-energy (BLE) support via the bluetooth module.

Initial development of this ESP32 port was sponsored in part by Microbric Pty Ltd.

Setting up ESP-IDF and the build environment
--------------------------------------------

MicroPython on ESP32 requires the Espressif IDF version 5 (IoT development
framework, aka SDK).  The ESP-IDF includes the libraries and RTOS needed to
manage the ESP32 microcontroller, as well as a way to manage the required
build environment and toolchains needed to build the firmware.

The ESP-IDF changes quickly and MicroPython only supports certain versions.
Currently MicroPython supports v5.2, v5.2.2, v5.3 and v5.4.

To install the ESP-IDF the full instructions can be found at the
[Espressif Getting Started guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#installation-step-by-step).

If you are on a Windows machine then the [Windows Subsystem for
Linux](https://msdn.microsoft.com/en-au/commandline/wsl/install_guide) is the
most efficient way to install the ESP32 toolchain and build the project. If
you use WSL then follow the Linux instructions rather than the Windows
instructions.

The Espressif instructions will guide you through using the `install.sh`
(or `install.bat`) script to download the toolchain and set up your environment.
The steps to take are summarised below.

To check out a copy of the IDF use git clone:

```bash
$ git clone -b v5.2.2 --recursive https://github.com/espressif/esp-idf.git
```

You can replace `v5.2.2` with any other supported version.
(You don't need a full recursive clone; see the `ci_esp32_setup` function in
`tools/ci.sh` in this repository for more detailed set-up commands.)

If you already have a copy of the IDF then checkout a version compatible with
MicroPython and update the submodules using:

```bash
$ cd esp-idf
$ git checkout v5.2.2
$ git submodule update --init --recursive
```

After you've cloned and checked out the IDF to the correct version, run the
`install.sh` script:

```bash
$ cd esp-idf
$ ./install.sh esp32 # (or install.bat on Windows)
$ source export.sh   # (or export.bat on Windows)
```

The `install.sh` step only needs to be done once. Change `esp32` if you are
targeting other chip. Use comma-separated list like `esp32,esp32s2` to
install for multiple chips. Or omit the chip to install for all Espressif
chips (which is slower).

You will need to source `export.sh` for every new session.

Building the firmware
---------------------

The MicroPython cross-compiler must be built to pre-compile some of the
built-in scripts to bytecode.  This can be done by (from the root of
this repository):

```bash
$ make -C mpy-cross
```

Then to build MicroPython for ESP32 run:

```bash
$ cd ports/esp32
$ make submodules
$ make
```

This will produce a combined `firmware.bin` image in the `build-ESP32_GENERIC/`
subdirectory (this firmware image is made up of: bootloader.bin, partitions.bin
and micropython.bin).

To flash the firmware you must have your ESP32 module in the bootloader
mode and connected to a serial port on your PC.  Refer to the documentation
for your particular ESP32 module for how to do this.
You will also need to have user permissions to access the `/dev/ttyUSB0` device.
On Linux, you can enable this by adding your user to the `dialout` group, and
rebooting or logging out and in again. (Note: on some distributions this may
be the `uucp` group, run `ls -la /dev/ttyUSB0` to check.)

```bash
$ sudo usermod -aG dialout <username>
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

The default ESP32 board build by the above commands is the `ESP32_GENERIC`
one, which should work on most ESP32 modules.  You can specify a different
board by passing `BOARD=<board>` to the make commands, for example:

```bash
$ make BOARD=ESP32_GENERIC_S3
```

Note: the above "make" commands are thin wrappers for the underlying `idf.py`
build tool that is part of the ESP-IDF.  You can instead use `idf.py` directly,
for example:

```bash
$ idf.py build
$ idf.py -D MICROPY_BOARD=ESP32_GENERIC build
$ idf.py flash
```

Some boards also support "variants", which allow for small variations of
an otherwise similar board. For example different flash sizes or features. For
example to build the `OTA` variant of `ESP32_GENERIC`.

```bash
$ make BOARD=ESP32_GENERIC BOARD_VARIANT=OTA
```

or to enable octal-SPIRAM support for the `ESP32_GENERIC_S3` board:

```bash
$ make BOARD=ESP32_GENERIC_S3 BOARD_VARIANT=SPIRAM_OCT
```


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

You can also use `idf.py monitor` or `mpremote`.

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
    wlan = network.WLAN(network.WLAN.IF_STA)
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

Defining a custom ESP32 board
-----------------------------

The default ESP-IDF configuration settings are provided by the `ESP32_GENERIC`
board definition in the directory `boards/ESP32_GENERIC`. For a custom configuration
you can define your own board directory.  Start a new board configuration by
copying an existing one (like `ESP32_GENERIC`) and modifying it to suit your board.

MicroPython specific configuration values are defined in the board-specific
`mpconfigboard.h` file, which is included by `mpconfigport.h`.  Additional
settings are put in `mpconfigboard.cmake`, including a list of `sdkconfig`
files that configure ESP-IDF settings.  Some standard `sdkconfig` files are
provided in the `boards/` directory, like `boards/sdkconfig.ble`.  You can
also define custom ones in your board directory.

Deployment instructions usually invoke the `boards/deploy.md` file (for boards
with a USB/Serial converter connection), or the `boards/deploy_nativeusb.md`
file (for boards with only a native USB port connection). These files are
formatted for each board using template strings found in the `boards.json`
files. You can also include the common `boards/deploy_flashmode.md` file if you
have a board which requires manual resetting via the RESET and BOOT buttons.
Boards with unique flashing steps can include custom `deploy.md` file(s).
Existing `board.json` files contain examples of all of these combinations.

See existing board definitions for further examples of configuration.

Configuration
Troubleshooting
---------------

* Continuous reboots after programming: Ensure `CONFIG_ESPTOOLPY_FLASHMODE` is
  correct for your board (e.g. ESP-WROOM-32 should be DIO). Then perform a
  `make clean`, rebuild, redeploy.
