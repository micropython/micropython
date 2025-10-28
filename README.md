[![Unix CI badge](https://github.com/micropython/micropython/actions/workflows/ports_unix.yml/badge.svg)](https://github.com/micropython/micropython/actions?query=branch%3Amaster+event%3Apush) [![STM32 CI badge](https://github.com/micropython/micropython/actions/workflows/ports_stm32.yml/badge.svg)](https://github.com/micropython/micropython/actions?query=branch%3Amaster+event%3Apush) [![Docs CI badge](https://github.com/micropython/micropython/actions/workflows/docs.yml/badge.svg)](https://docs.micropython.org/) [![codecov](https://codecov.io/gh/micropython/micropython/branch/master/graph/badge.svg?token=I92PfD05sD)](https://codecov.io/gh/micropython/micropython)

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
`os`, `sys`, `time`, `re`, and `struct`, etc. Some ports have support for
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

See the [online documentation](https://docs.micropython.org/) for the API
reference and information about using MicroPython and information about how
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
You will also need bash, gcc, and Python 3.3+ available as the command `python3`.
Some ports (rp2 and esp32) additionally use CMake.

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

Over twenty different MicroPython ports are provided in this repository,
split across three
[MicroPython Support Tiers](https://docs.micropython.org/en/latest/develop/support_tiers.html).

Tier 1 Ports
============

👑 Ports in [Tier 1](https://docs.micropython.org/en/latest/develop/support_tiers.html)
are mature and have the most active development, support and testing:

| Port                     | Target                                                                                 | Quick Reference                                                      |
|--------------------------|----------------------------------------------------------------------------------------|----------------------------------------------------------------------|
| [esp32](ports/esp32)*    | Espressif ESP32 SoCs (ESP32, ESP32S2, ESP32S3, ESP32C3, ESP32C6)                       | [here](https://docs.micropython.org/en/latest/esp32/quickref.html)   |
| [mimxrt](ports/mimxrt)   | NXP m.iMX RT                                                                           | [here](https://docs.micropython.org/en/latest/mimxrt/quickref.html)  |
| [rp2](ports/rp2)         | Raspberry Pi RP2040 and RP2350                                                         | [here](https://docs.micropython.org/en/latest/rp2/quickref.html)     |
| [samd](ports/samd)       | Microchip (formerly Atmel) SAMD21 and SAMD51                                           | [here](https://docs.micropython.org/en/latest/samd/quickref.html)    |
| [stm32](ports/stm32)     | STMicroelectronics STM32 MCUs (F0, F4, F7, G0, G4, H5, H7, L0, L1, L4, N6, WB, WL)     | [here](https://docs.micropython.org/en/latest/pyboard/quickref.html) |
| [unix](ports/unix)       | Linux, BSD, macOS, WSL                                                                 | [here](https://docs.micropython.org/en/latest/unix/quickref.html)    |
| [windows](ports/windows) | Microsoft Windows                                                                      | [here](https://docs.micropython.org/en/latest/unix/quickref.html)    |

An asterisk indicates that the port has ongoing financial support from the vendor.

Tier 2 Ports
============

✔ Ports in [Tier 2](https://docs.micropython.org/en/latest/develop/support_tiers.html)
are less mature and less actively developed and tested than Tier 1, but
still fully supported:

| Port                             | Target                                                      | Quick Reference                                                         |
|----------------------------------|-------------------------------------------------------------|-------------------------------------------------------------------------|
| [alif](ports/alif)               | Alif Semiconductor Ensemble MCUs (E3, E7)                   |                                                                         |
| [embed](ports/embed)             | Generates a set of .c/.h files for embedding into a project |                                                                         |
| [nrf](ports/nrf)                 | Nordic Semiconductor nRF51 and nRF52                        |                                                                         |
| [renesas-ra](ports/renesas-ra)   | Renesas RA family                                           | [here](https://docs.micropython.org/en/latest/renesas-ra/quickref.html) |
| [webassembly](ports/webassembly) | Emscripten port targeting browsers and NodeJS               |                                                                         |
| [zephyr](ports/zephyr)           | Zephyr RTOS                                                 | [here](https://docs.micropython.org/en/latest/zephyr/quickref.html)     |

Tier 3 Ports
============

Ports in [Tier 3](https://docs.micropython.org/en/latest/develop/support_tiers.html)
are built in CI but not regularly tested by the MicroPython maintainers:

| Port                       | Target                                                            | Quick Reference                                                         |
|----------------------------|-------------------------------------------------------------------|-------------------------------------------------------------------------|
| [cc3200](ports/cc3200)     | Texas Instruments CC3200                                          | [For WiPy](https://docs.micropython.org/en/latest/wipy/quickref.html)   |
| [esp8266](ports/esp8266)   | Espressif ESP8266 SoC                                             | [here](https://docs.micropython.org/en/latest/esp8266/quickref.html)    |
| [pic16bit](ports/pic16bit) | Microchip PIC 16-bit                                              |                                                                         |
| [powerpc](ports/powerpc)   | IBM PowerPC (including Microwatt)                                 |                                                                         |

Additional Ports
================

In addition to the above there is a Tier M containing ports that are used
primarily for maintenance, development and testing:

- The ["bare-arm"](ports/bare-arm) port is an example of the absolute minimum
  configuration that still includes the compiler, and is used to keep track
  of the code size of the core runtime and VM.

- The ["minimal"](ports/minimal) port provides an example of a very basic
  MicroPython port and can be compiled as both a standalone Linux binary as
  well as for ARM Cortex-M4. Start with this if you want to port MicroPython
  to another microcontroller.

- The [qemu](ports/qemu) port is a QEMU-based emulated target for Cortex-A,
  Cortex-M, RISC-V 32-bit and RISC-V 64-bit architectures.

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
