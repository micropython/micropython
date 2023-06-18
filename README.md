# Micropython + lvgl

**Micropython bindings to LVGL for Embedded devices, Unix and JavaScript**

[![Build lv_micropython unix port](https://github.com/lvgl/lv_micropython/actions/workflows/unix_port.yml/badge.svg)](https://github.com/lvgl/lv_micropython/actions/workflows/unix_port.yml)
[![Build lv_micropython stm32 port](https://github.com/lvgl/lv_micropython/actions/workflows/stm32_port.yml/badge.svg)](https://github.com/lvgl/lv_micropython/actions/workflows/stm32_port.yml)
[![esp32 port](https://github.com/lvgl/lv_micropython/actions/workflows/ports_esp32.yml/badge.svg)](https://github.com/lvgl/lv_micropython/actions/workflows/ports_esp32.yml) [![Build lv_micropython rp2 port](https://github.com/lvgl/lv_micropython/actions/workflows/rp2_port.yml/badge.svg)](https://github.com/lvgl/lv_micropython/actions/workflows/rp2_port.yml)
[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/lvgl/lv_micropython)  

To quickly run Micropython + LVGL from your web browser you can also use the [Online Simulator](https://sim.lvgl.io/).

**For information about Micropython lvgl bindings please refer to [lv_binding_micropython/README.md](https://github.com/lvgl/lv_binding_micropython/blob/master/README.md)**

See also [Micropython + LittlevGL](https://blog.lvgl.io/2019-02-20/micropython-bindings) blog post. (LittlevGL is LVGL's previous name.)  
For questions and discussions - please use the forum: https://forum.lvgl.io/c/micropython

Original micropython README: https://github.com/micropython/micropython/blob/master/README.md

## Relationship between `lv_micropython` and `lv_binding_micropython`

Originally, `lv_micropython` was created as an example of how to use [lv_binding_micropython](https://github.com/lvgl/lv_binding_micropython) on a Micropython fork.  
As such, we try to keep changes here as minimal as possible and we try to keep it in sync with Micropython upstream releases. We also try to add changes to `lv_binding_micropython` instead of to `lv_micropython`, when possible. (for example we keep all drivers in `lv_binding_micropython`, the ESP32 CMake functionality etc.)

Eventually it turned out that many people prefer using `lv_micropython` directly and only a few use it as a reference to support LVGL on their own Micropython fork.
If you are only starting with Micropython+LVGL, it's recommended that you use `lv_micropython`, while porting a Micropython fork to LVGL is for advanced users.

## Build Instructions

First step is always to clone lv_micropython and update its submodules recursively:

```
git clone https://github.com/lvgl/lv_micropython.git
cd lv_micropython
git submodule update --init --recursive lib/lv_bindings
```

Next you should build mpy-cross

```
make -C mpy-cross
```

Port specific steps usually include updating the port's submodules with `make submodules` and running make for the port itself.

### Unix (Linux) port

1. `sudo apt-get install build-essential libreadline-dev libffi-dev git pkg-config libsdl2-2.0-0 libsdl2-dev python3.8 parallel`
Python 3 is required, but you can install some other version of python3 instead of 3.8, if needed.
2. `git clone https://github.com/lvgl/lv_micropython.git`
3. `cd lv_micropython`
4. `git submodule update --init --recursive lib/lv_bindings`
5. `make -C mpy-cross`
6. `make -C ports/unix submodules`
7. `make -C ports/unix`
8. `./ports/unix/micropython`

## Unix (MAC OS) port

1. `brew install sdl2 pkg-config`
2. `git clone https://github.com/lvgl/lv_micropython.git`
3. `cd lv_micropython`
4. `git submodule update --init --recursive lib/lv_bindings`
5. `sudo mkdir -p /usr/local/lib/`
6. `sudo cp /opt/homebrew/Cellar/sdl2/2.24.0/lib/libSDL2.dylib /usr/local/lib/`
7. `sudo cp -r /opt/homebrew/Cellar/sdl2/2.24.0/include /usr/local/`
8. `sed -i '' 's/ -Werror//' ports/unix/Makefile` Remove -Werror from compiler parameters as Mac fails compilation otherwise
9. `make -C mpy-cross`
10. `make -C ports/unix submodules`
11. `make -C ports/unix`
12. `./ports/unix/micropython`

### ESP32 port

Please run `esp-idf/export.sh` from your ESP-IDF installation directory as explained in the [Micropython ESP32 Getting Started documentation](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/#get-started-export)  
ESP-IDF version needs to match Micropython expected esp-idf, otherwise a warning will be displayed (and build will probably fail)
For more details refer to [Setting up the toolchain and ESP-IDF](https://github.com/lvgl/lv_micropython/blob/master/ports/esp32/README.md#setting-up-the-toolchain-and-esp-idf)

When using IL9341 driver, the color depth need to be set to match ILI9341. This can be done from the command line.
Here is the command to build ESP32 + LVGL which is compatible with ILI9341 driver:

```
make -C mpy-cross
make -C ports/esp32 LV_CFLAGS="-DLV_COLOR_DEPTH=16" BOARD=GENERIC_SPIRAM deploy
```

Explanation about the paramters:
- `LV_CFLAGS` are used to override color depth, for ILI9341 compatibility.
  - `LV_COLOR_DEPTH=16` is needed if you plan to use the ILI9341 driver.
- `BOARD` - I use WROVER board with SPIRAM. You can choose other boards from `ports/esp32/boards/` directory.
- `deploy` - make command will create ESP32 port of Micropython, and will try to deploy it through USB-UART bridge.

For more details please refer to [Micropython ESP32 README](https://github.com/micropython/micropython/blob/master/ports/esp32/README.md).

### JavaScript port

Refer to the README of the `lvgl_javascript` branch: https://github.com/lvgl/lv_micropython/tree/lvgl_javascript_v8#javascript-port

### Raspberry Pi Pico port

This port uses [Micropython infrastructure for C modules](https://docs.micropython.org/en/latest/develop/cmodules.html#compiling-the-cmodule-into-micropython) and `USER_C_MODULES` must be given:

1. `git clone https://github.com/lvgl/lv_micropython.git`
2. `cd lv_micropython`
3. `git submodule update --init --recursive lib/lv_bindings`
4. `make -C ports/rp2 BOARD=PICO submodules`
5. `make -j -C mpy-cross`
6. `make -j -C ports/rp2 BOARD=PICO USER_C_MODULES=../../lib/lv_bindings/bindings.cmake`

#### Troubleshooting

If you experience unstable behaviour, it is worth checking the value of *MICROPY_HW_FLASH_STORAGE_BASE* against the value of *__flash_binary_end* from the firmware.elf.map file.
If the storage base is lower than the binary end, parts of the firmware will be overwritten when the micropython filesystem is initialised.

## Super Simple Example

First, LVGL needs to be imported and initialized

```python
import lvgl as lv
lv.init()
```

Then event loop, display driver and input driver needs to be registered.
Refer to [Porting the library](https://docs.lvgl.io/8.0/porting/index.html) for more information.
Here is an example of registering SDL drivers on Micropython unix port:

```python
# Create an event loop and Register SDL display/mouse/keyboard drivers.
from lv_utils import event_loop

WIDTH = 480
HEIGHT = 320

event_loop = event_loop()
disp_drv = lv.sdl_window_create(WIDTH, HEIGHT)
mouse = lv.sdl_mouse_create()
keyboard = lv.sdl_keyboard_create()
keyboard.set_group(self.group)
```

Here is an alternative example, for registering ILI9341 drivers on Micropython ESP32 port:

```python
import lvgl as lv

# Import ILI9341 driver and initialized it

from ili9341 import ili9341
disp = ili9341()

# Import XPT2046 driver and initalize it

from xpt2046 import xpt2046
touch = xpt2046()
```

By default, both ILI9341 and XPT2046 are initialized on the same SPI bus with the following parameters:

- ILI9341: `miso=5, mosi=18, clk=19, cs=13, dc=12, rst=4, power=14, backlight=15, spihost=esp.HSPI_HOST, mhz=40, factor=4, hybrid=True`
- XPT2046: `cs=25, spihost=esp.HSPI_HOST, mhz=5, max_cmds=16, cal_x0 = 3783, cal_y0 = 3948, cal_x1 = 242, cal_y1 = 423, transpose = True, samples = 3`

You can change any of these parameters on ili9341/xpt2046 constructor.
You can also initalize them on different SPI buses if you want, by providing miso/mosi/clk parameters. Set them to -1 to use existing (initialized) spihost bus.

Now you can create the GUI itself:

```python

# Create a screen with a button and a label

scr = lv.obj()
btn = lv.btn(scr)
btn.align_to(lv.scr_act(), lv.ALIGN.CENTER, 0, 0)
label = lv.label(btn)
label.set_text("Hello World!")

# Load the screen

lv.scr_load(scr)

```

## More information

More info about LVGL:
- Website https://lvgl.io
- GitHub: https://github.com/lvgl/lvgl
- Documentation: https://docs.lvgl.io/master/get-started/bindings/micropython.html
- Examples: https://docs.lvgl.io/master/examples.html
- More examples: https://github.com/lvgl/lv_binding_micropython/tree/master/examples

More info about lvgl Micropython bindings:
- https://github.com/lvgl/lv_binding_micropython/blob/master/README.md

Discussions about the Micropython binding: https://github.com/lvgl/lvgl/issues/557

More info about the unix port: https://github.com/micropython/micropython/wiki/Getting-Started#debian-ubuntu-mint-and-variants

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
Python 3.5 and some select features from later versions). The following core
datatypes are provided: `str`(including basic Unicode support), `bytes`,
`bytearray`, `tuple`, `list`, `dict`, `set`, `frozenset`, `array.array`,
`collections.namedtuple`, classes and instances. Builtin modules include
`os`, `sys`, `time`, `re`, and `struct`, etc. Select ports have support for
`_thread` module (multithreading), `socket` and `ssl` for networking, and
`asyncio`. Note that only a subset of Python 3 functionality is implemented
for the data types and modules.

MicroPython can execute scripts in textual source form (.py files) or from
precompiled bytecode (.mpy files), in both cases either from an on-device
filesystem or "frozen" into the MicroPython executable.

MicroPython also provides a set of MicroPython-specific modules to access
hardware-specific functionality and peripherals such as GPIO, Timers, ADC,
DAC, PWM, SPI, I2C, CAN, Bluetooth, and USB.

Getting started
---------------

See the [online documentation](https://docs.micropython.org/) for API
references and information about using MicroPython and information about how
it is implemented.

We use [GitHub Discussions](https://github.com/micropython/micropython/discussions)
as our forum, and [Discord](https://discord.gg/RB8HZSAExQ) for chat. These
are great places to ask questions and advice from the community or to discuss your
MicroPython-based projects.

For bugs and feature requests, please [raise an issue](https://github.com/micropython/micropython/issues/new/choose)
and follow the templates there.

For information about the [MicroPython pyboard](https://store.micropython.org/pyb-features),
the officially supported board from the
[original Kickstarter campaign](https://www.kickstarter.com/projects/214379695/micro-python-python-for-microcontrollers),
see the [schematics and pinouts](http://github.com/micropython/pyboard) and
[documentation](https://docs.micropython.org/en/latest/pyboard/quickref.html).

Contributing
------------

MicroPython is an open-source project and welcomes contributions. To be
productive, please be sure to follow the
[Contributors' Guidelines](https://github.com/micropython/micropython/wiki/ContributorGuidelines)
and the [Code Conventions](https://github.com/micropython/micropython/blob/master/CODECONVENTIONS.md).
Note that MicroPython is licenced under the MIT license, and all contributions
should follow this license.

About this repository
---------------------

This repository contains the following components:
- [py/](py/) -- the core Python implementation, including compiler, runtime, and
  core library.
- [mpy-cross/](mpy-cross/) -- the MicroPython cross-compiler which is used to turn scripts
  into precompiled bytecode.
- [ports/](ports/) -- platform-specific code for the various ports and architectures that MicroPython runs on.
- [lib/](lib/) -- submodules for external dependencies.
- [tests/](tests/) -- test framework and test scripts.
- [docs/](docs/) -- user documentation in Sphinx reStructuredText format. This is used to generate the [online documentation](http://docs.micropython.org).
- [extmod/](extmod/) -- additional (non-core) modules implemented in C.
- [tools/](tools/) -- various tools, including the pyboard.py module.
- [examples/](examples/) -- a few example Python scripts.

"make" is used to build the components, or "gmake" on BSD-based systems.
You will also need bash, gcc, and Python 3.3+ available as the command `python3`
(if your system only has Python 2.7 then invoke make with the additional option
`PYTHON=python2`). Some ports (rp2 and esp32) additionally use CMake.

Supported platforms & architectures
-----------------------------------

MicroPython runs on a wide range of microcontrollers, as well as on Unix-like
(including Linux, BSD, macOS, WSL) and Windows systems.

Microcontroller targets can be as small as 256kiB flash + 16kiB RAM, although
devices with at least 512kiB flash + 128kiB RAM allow a much more
full-featured experience.

The [Unix](ports/unix) and [Windows](ports/windows) ports allow both
development and testing of MicroPython itself, as well as providing
lightweight alternative to CPython on these platforms (in particular on
embedded Linux systems).

The ["minimal"](ports/minimal) port provides an example of a very basic
MicroPython port and can be compiled as both a standalone Linux binary as
well as for ARM Cortex M4. Start with this if you want to port MicroPython to
another microcontroller. Additionally the ["bare-arm"](ports/bare-arm) port
is an example of the absolute minimum configuration, and is used to keep
track of the code size of the core runtime and VM.

In addition, the following ports are provided in this repository:
 - [cc3200](ports/cc3200) -- Texas Instruments CC3200 (including PyCom WiPy).
 - [esp32](ports/esp32) -- Espressif ESP32 SoC (including ESP32S2, ESP32S3, ESP32C3).
 - [esp8266](ports/esp8266) -- Espressif ESP8266 SoC.
 - [mimxrt](ports/mimxrt) -- NXP m.iMX RT (including Teensy 4.x).
 - [nrf](ports/nrf) -- Nordic Semiconductor nRF51 and nRF52.
 - [pic16bit](ports/pic16bit) -- Microchip PIC 16-bit.
 - [powerpc](ports/powerpc) -- IBM PowerPC (including Microwatt)
 - [qemu-arm](ports/qemu-arm) -- QEMU-based emulated target, for testing)
 - [renesas-ra](ports/renesas-ra) -- Renesas RA family.
 - [rp2](ports/rp2) -- Raspberry Pi RP2040 (including Pico and Pico W).
 - [samd](ports/samd) -- Microchip (formerly Atmel) SAMD21 and SAMD51.
 - [stm32](ports/stm32) -- STMicroelectronics STM32 family (including F0, F4, F7, G0, G4, H7, L0, L4, WB)
 - [teensy](ports/teensy) -- Teensy 3.x.
 - [webassembly](ports/webassembly) -- Emscripten port targeting browsers and NodeJS.
 - [zephyr](ports/zephyr) -- Zephyr RTOS.

The MicroPython cross-compiler, mpy-cross
-----------------------------------------

Most ports require the [MicroPython cross-compiler](mpy-cross) to be built
first.  This program, called mpy-cross, is used to pre-compile Python scripts
to .mpy files which can then be included (frozen) into the
firmware/executable for a port.  To build mpy-cross use:

    $ cd mpy-cross
    $ make

External dependencies
---------------------

The core MicroPython VM and runtime has no external dependencies, but a given
port might depend on third-party drivers or vendor HALs. This repository
includes [several submodules](lib/) linking to these external dependencies.
Before compiling a given port, use

    $ cd ports/name
    $ make submodules

to ensure that all required submodules are initialised.
