# MicroPython for KidBright32

Support KidBright32 V1.3, V1.4, V1.5, V1.5i by INEX, V1.6 by Gravitech

## Modules

All ESP32 modules support (See [Quick reference for the ESP32](http://docs.micropython.org/en/latest/esp32/quickref.html)) 

I add modules for KidBright32

### Module `display` 

Show text and number on DotMarix 16x8

```python
import display

display.show(99) # only 2-char show
display.scroll("Hello, mars !")
display.clear()
```

### Module `sensor`

Read temp and light level

```python
import sensor

lavel = sensor.light() # 0 - 100%
temp = sensor.temperature() # temperature in celsius
```

### Module `buzzer` 

Made sound on buzzer

```python
import buzzer

buzzer.tone(freq=2093, duration=0.5, duty=50)
buzzer.on(freq=2000, duty=50)
buzzer.off()
```

### Module `rtc` 

Get and set time to RTC, same [machine.RTC](http://docs.micropython.org/en/latest/library/machine.RTC.html#machine-rtc)

```python
import rtc

rtc.datetime((2017, 8, 23, 1, 12, 48, 0, 0)) # set a specific date and time
datetime = rtc.datetime() # get date and time
```

### Module `usb` 

Get and set logic on USB-A port

```python
import usb

usb.value(1) # or 0
usb.on()
usb.off()
usb.toggle()
```

### Module `switch` 

Get status from S1 ann S2

```python
import switch

value = switch.value(switch.S1) # or switch.S2
```

### Module `servo` 

Set angle of Servo motor on Servo pin (only on KidBrgiht32 V1.5 , V1.5i by INEX, V1.6 by Gravitech)

```python
import servo

servo.angle(servo.SV1, 45) # Set servo on SV0 move to angle 0
servo.angle(servo.SV2, 90) # Set servo on SV1 move to angle 1
```

### Module `imu` 

Read data from IMU sensor on KidBrgiht32 V1.5i by INEX, V1.6 by Gravitech

```python
import imu

imu.update() # read data from sensor
acc = imu.acc # array of (x, y, z)
gyro = imu.gyro # array of (x, y, z), only on KidBrgiht32 V1.6 by Gravitech
mag = imu.mag # array of (x, y, z), only on KidBrgiht32 V1.5i by INEX
```

You can see more detail. [Source all module](https://github.com/microBlock-IDE/micropython/tree/V1.0.0/ports/esp32/boards/KidBright32/modules)

## How to install on KidBright32

You can choose one

### MicroUploader (Recommend)

[Download MicroUploader](https://github.com/microBlock-IDE/MicroUploader/releases/tag/V1.2.0) and then open, Select you board, Port and MicroPython firmware version then click Upload



### esptool

See [Deploying the firmware](http://docs.micropython.org/en/latest/esp32/tutorial/intro.html#deploying-the-firmware)

### Flash Download Tools

only on Windows. you can download this tool from https://www.espressif.com/en/support/download/other-tools

Open `flash_download_tool_3.8.5.exe` then select Developer Mode > ESP32 DownloadToo

Add `MicroPython.for.KidBright32.V1.0.0.bin` @ 0x1000

Select port and baud then Click Erase for clear flash (Fixed can't upload MicroPython code file)

Click START and wait finished

## How to use

You can use [microBlock IDE](https://microblock.app) in Code mode for programming.

The MicroPython project
=======================
<p align="center">
  <img src="https://raw.githubusercontent.com/micropython/micropython/master/logo/upython-with-micro.jpg" alt="MicroPython Logo"/>
</p>

This is the MicroPython project, which aims to put an implementation
of Python 3.x on microcontrollers and small embedded systems.
You can find the official website at [micropython.org](http://www.micropython.org).

WARNING: this project is in beta stage and is subject to changes of the
code-base, including project-wide name changes and API changes.

MicroPython implements the entire Python 3.4 syntax (including exceptions,
`with`, `yield from`, etc., and additionally `async`/`await` keywords from
Python 3.5). The following core datatypes are provided: `str` (including
basic Unicode support), `bytes`, `bytearray`, `tuple`, `list`, `dict`, `set`,
`frozenset`, `array.array`, `collections.namedtuple`, classes and instances.
Builtin modules include `sys`, `time`, and `struct`, etc. Select ports have
support for `_thread` module (multithreading). Note that only a subset of
Python 3 functionality is implemented for the data types and modules.

MicroPython can execute scripts in textual source form or from precompiled
bytecode, in both cases either from an on-device filesystem or "frozen" into
the MicroPython executable.

See the repository http://github.com/micropython/pyboard for the MicroPython
board (PyBoard), the officially supported reference electronic circuit board.

Major components in this repository:
- py/ -- the core Python implementation, including compiler, runtime, and
  core library.
- mpy-cross/ -- the MicroPython cross-compiler which is used to turn scripts
  into precompiled bytecode.
- ports/unix/ -- a version of MicroPython that runs on Unix.
- ports/stm32/ -- a version of MicroPython that runs on the PyBoard and similar
  STM32 boards (using ST's Cube HAL drivers).
- ports/minimal/ -- a minimal MicroPython port. Start with this if you want
  to port MicroPython to another microcontroller.
- tests/ -- test framework and test scripts.
- docs/ -- user documentation in Sphinx reStructuredText format. Rendered
  HTML documentation is available at http://docs.micropython.org.

Additional components:
- ports/bare-arm/ -- a bare minimum version of MicroPython for ARM MCUs. Used
  mostly to control code size.
- ports/teensy/ -- a version of MicroPython that runs on the Teensy 3.1
  (preliminary but functional).
- ports/pic16bit/ -- a version of MicroPython for 16-bit PIC microcontrollers.
- ports/cc3200/ -- a version of MicroPython that runs on the CC3200 from TI.
- ports/esp8266/ -- a version of MicroPython that runs on Espressif's ESP8266 SoC.
- ports/esp32/ -- a version of MicroPython that runs on Espressif's ESP32 SoC.
- ports/nrf/ -- a version of MicroPython that runs on Nordic's nRF51 and nRF52 MCUs.
- extmod/ -- additional (non-core) modules implemented in C.
- tools/ -- various tools, including the pyboard.py module.
- examples/ -- a few example Python scripts.

The subdirectories above may include READMEs with additional info.

"make" is used to build the components, or "gmake" on BSD-based systems.
You will also need bash, gcc, and Python 3.3+ available as the command `python3`
(if your system only has Python 2.7 then invoke make with the additional option
`PYTHON=python2`).

The MicroPython cross-compiler, mpy-cross
-----------------------------------------

Most ports require the MicroPython cross-compiler to be built first.  This
program, called mpy-cross, is used to pre-compile Python scripts to .mpy
files which can then be included (frozen) into the firmware/executable for
a port.  To build mpy-cross use:

    $ cd mpy-cross
    $ make

The Unix version
----------------

The "unix" port requires a standard Unix environment with gcc and GNU make.
x86 and x64 architectures are supported (i.e. x86 32- and 64-bit), as well
as ARM and MIPS. Making full-featured port to another architecture requires
writing some assembly code for the exception handling and garbage collection.
Alternatively, fallback implementation based on setjmp/longjmp can be used.

To build (see section below for required dependencies):

    $ cd ports/unix
    $ make submodules
    $ make

Then to give it a try:

    $ ./micropython
    >>> list(5 * x + y for x in range(10) for y in [4, 2, 1])

Use `CTRL-D` (i.e. EOF) to exit the shell.
Learn about command-line options (in particular, how to increase heap size
which may be needed for larger applications):

    $ ./micropython -h

Run complete testsuite:

    $ make test

Unix version comes with a builtin package manager called upip, e.g.:

    $ ./micropython -m upip install micropython-pystone
    $ ./micropython -m pystone

Browse available modules on
[PyPI](https://pypi.python.org/pypi?%3Aaction=search&term=micropython).
Standard library modules come from
[micropython-lib](https://github.com/micropython/micropython-lib) project.

External dependencies
---------------------

Building MicroPython ports may require some dependencies installed.

For Unix port, `libffi` library and `pkg-config` tool are required. On
Debian/Ubuntu/Mint derivative Linux distros, install `build-essential`
(includes toolchain and make), `libffi-dev`, and `pkg-config` packages.

Other dependencies can be built together with MicroPython. This may
be required to enable extra features or capabilities, and in recent
versions of MicroPython, these may be enabled by default. To build
these additional dependencies, in the port directory you're
interested in (e.g. `ports/unix/`) first execute:

    $ make submodules

This will fetch all the relevant git submodules (sub repositories) that
the port needs.  Use the same command to get the latest versions of
submodules as they are updated from time to time. After that execute:

    $ make deplibs

This will build all available dependencies (regardless whether they
are used or not). If you intend to build MicroPython with additional
options (like cross-compiling), the same set of options should be passed
to `make deplibs`. To actually enable/disable use of dependencies, edit
`ports/unix/mpconfigport.mk` file, which has inline descriptions of the options.
For example, to build SSL module (required for `upip` tool described above,
and so enabled by default), `MICROPY_PY_USSL` should be set to 1.

For some ports, building required dependences is transparent, and happens
automatically.  But they still need to be fetched with the `make submodules`
command.

The STM32 version
-----------------

The "stm32" port requires an ARM compiler, arm-none-eabi-gcc, and associated
bin-utils.  For those using Arch Linux, you need arm-none-eabi-binutils,
arm-none-eabi-gcc and arm-none-eabi-newlib packages.  Otherwise, try here:
https://launchpad.net/gcc-arm-embedded

To build:

    $ cd ports/stm32
    $ make submodules
    $ make

You then need to get your board into DFU mode.  On the pyboard, connect the
3V3 pin to the P1/DFU pin with a wire (on PYBv1.0 they are next to each other
on the bottom left of the board, second row from the bottom).

Then to flash the code via USB DFU to your device:

    $ make deploy

This will use the included `tools/pydfu.py` script.  If flashing the firmware
does not work it may be because you don't have the correct permissions, and
need to use `sudo make deploy`.
See the README.md file in the ports/stm32/ directory for further details.

Contributing
------------

MicroPython is an open-source project and welcomes contributions. To be
productive, please be sure to follow the
[Contributors' Guidelines](https://github.com/micropython/micropython/wiki/ContributorGuidelines)
and the [Code Conventions](https://github.com/micropython/micropython/blob/master/CODECONVENTIONS.md).
Note that MicroPython is licenced under the MIT license, and all contributions
should follow this license.
