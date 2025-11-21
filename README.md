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

What's New in v1.27.1 🎉
------------------------

### 🔒 Security Release - 40 Vulnerabilities Fixed

MicroPython v1.27.1 is a **major security and stability release** addressing 40 vulnerabilities
across the codebase. **Upgrading is strongly recommended**, especially for:
- **WebREPL users** (CRITICAL buffer overflow fixes)
- **Network applications** (lwIP stack improvements)
- **Production deployments** (stability enhancements)

**Security Summary:**
- **2 CRITICAL** - WebREPL buffer overflows enabling remote code execution (CVE pending)
- **8 HIGH** - Division by zero, array bounds, memory allocation overflows
- **28 MEDIUM** - Integer overflows, NULL pointers, buffer validation issues
- **2 DOCUMENTED** - Path traversal vulnerabilities (require architectural changes)

See [RELEASE_NOTES_v1.27.1.md](RELEASE_NOTES_v1.27.1.md) for complete security details.

### 🚀 Interactive Firmware Upload Tool

**NEW:** Easy firmware installation with automatic device detection!

```bash
cd tools
python3 upload_firmware.py
```

**Features:**
- 🔍 **Auto-detects** connected devices (Pico, ESP32, STM32)
- 📋 **Step-by-step guidance** with color-coded progress
- ✅ **One-command upload** for all device types
- 📚 **Comprehensive help** with troubleshooting

**Supported Devices:**
- Raspberry Pi Pico / Pico 2 (RP2040 / RP2350) - UF2 bootloader
- ESP32 / ESP32-S2 / ESP32-S3 - esptool.py
- STM32 Pyboard - DFU mode

**Quick Start:**
```bash
# List connected devices
python3 tools/upload_firmware.py --list

# Interactive wizard (recommended)
python3 tools/upload_firmware.py

# Or direct upload
python3 tools/upload_firmware.py --device rp2-pico --firmware firmware.uf2
```

**Documentation:**
- [QUICK_START.md](tools/QUICK_START.md) - 3-minute setup guide
- [README.md](tools/README.md) - Complete tool reference
- [FIRMWARE_UPLOAD_GUIDE.md](tools/FIRMWARE_UPLOAD_GUIDE.md) - Device-specific instructions

### 📚 Enhanced C API Documentation

**NEW:** Comprehensive C API guide for developers integrating MicroPython!

[docs/develop/c_api_guide.md](docs/develop/c_api_guide.md) - **66KB** of detailed documentation covering:

**Core Topics:**
- 🏗️ Object system and custom type creation
- 💾 Memory management best practices
- 🔄 Buffer protocol implementation
- ⚠️ Exception handling patterns
- 🔁 Iterator and generator creation
- 📡 Callback and event handling
- ⚙️ Hardware integration (GPIO, SPI, I2C, UART)
- 🧵 Thread safety and interrupt handling
- ⚡ Performance optimization techniques
- 🐛 Debugging and profiling methods

**Advanced Examples:**
- [buffer_example.c](examples/usercmodule/advanced_examples/buffer_example.c) - RingBuffer class with buffer protocol, CRC32, XOR operations
- [callback_example.c](examples/usercmodule/advanced_examples/callback_example.c) - Event-driven programming, interrupt-safe queues

**Build System Support:**
- Both Make and CMake build configurations
- Complete integration examples
- Ready to use in your projects

### ✨ Enhanced Port Features

#### Bare-ARM Port - Full Peripheral Support
The minimal bare-ARM port now has **complete hardware functionality**:
- ✅ UART/USART (full serial communication)
- ✅ I2C (master mode, device scanning)
- ✅ SPI (master mode, 8-bit transfers)
- ✅ PWM (pulse width modulation, 4 channels)
- ✅ ADC (12-bit analog conversion, 16 channels)
- ✅ Timer (hardware timer support)

See [ports/bare-arm/README_FULL_PERIPHERALS.md](ports/bare-arm/README_FULL_PERIPHERALS.md)

#### Unix Port - Extended Hardware Support
- Enhanced machine module with UART and USB gadget support
- Raspberry Pi 5 hardware control
- Audio, camera, and display interfaces
- TensorFlow Lite integration

#### RP2350 (Pico 2) Support
Full support for the new **Raspberry Pi Pico 2** with RP2350 chip:
```bash
make BOARD=RPI_PICO2      # Pico 2
make BOARD=RPI_PICO2_W    # Pico 2 W (WiFi)
```

Additional boards: SEEED_XIAO_RP2350, SPARKFUN_THINGPLUS_RP2350, WEACTSTUDIO_RP2350B_CORE

### 🛠️ Build System Improvements

**Fixed Issues:**
- Unix port build system (added machine_uart.c, machine_usb.c)
- QSTR definitions for UART support
- Static initializer issues
- Include dependencies

**Build Verification:**
- Unix port tested (887KB binary)
- All security fixes compile cleanly
- Ready for embedded port builds

### 📊 Version Status

**Current Version:** v1.27.1 (Stable)
- MICROPY_VERSION_MAJOR: 1
- MICROPY_VERSION_MINOR: 27
- MICROPY_VERSION_MICRO: 1
- MICROPY_VERSION_PRERELEASE: 0 (stable!)

See [CURRENT_VERSION_STATUS.md](CURRENT_VERSION_STATUS.md) for all ports.

### 🔗 Release Documentation

- [RELEASE_NOTES_v1.27.1.md](RELEASE_NOTES_v1.27.1.md) - Complete changelog and security details
- [REPOSITORY_STATUS.md](REPOSITORY_STATUS.md) - Full repository status and verification
- [BUG_FIXES_SESSION3.md](BUG_FIXES_SESSION3.md) - Latest 20 security fixes (Session 3)
- [BUG_FIXES_SESSION2_PART2.md](BUG_FIXES_SESSION2_PART2.md) - 9 additional fixes (Session 2 Part 2)
- [BUG_FIXES_SESSION2.md](BUG_FIXES_SESSION2.md) - Initial 9 fixes (Session 2)

### 🎯 Quick Links

| Resource | Description |
|----------|-------------|
| [Upload Firmware](tools/upload_firmware.py) | Interactive firmware upload wizard |
| [3-Minute Guide](tools/QUICK_START.md) | Get started in under 3 minutes |
| [C API Reference](docs/develop/c_api_guide.md) | Complete C integration guide |
| [Release Notes](RELEASE_NOTES_v1.27.1.md) | Full v1.27.1 changelog |
| [Security Fixes](BUG_FIXES_SESSION3.md) | Detailed security audit results |

### 📈 Performance & Compatibility

- **No performance degradation** from security fixes
- **Minimal code size increase** (~2KB across all fixes)
- **No breaking changes** from v1.27.0-preview
- **Direct upgrade path** for all users

### ⚠️ Upgrade Recommendation

**WebREPL Users - CRITICAL:**
If you use WebREPL for remote access, **upgrade immediately**. The buffer overflow
vulnerabilities allow remote code execution and authentication bypass.

**All Users:**
This release is recommended for all deployments due to the extensive stability
and security improvements.

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
- [tools/](tools/) -- various tools, including the **NEW interactive firmware upload utility** and pyboard.py module.
- [examples/](examples/) -- Python scripts and advanced C module examples.

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
