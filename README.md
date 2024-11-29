AMAS Firmware Based on MicroPython
=======================
This firmware is based on MicroPython, and as such, this repo is a fork of the main Micropython repo to preserve code acknowledgements. The main differences are in the build process; this repo contains several required libraries as frozen modules of ESP32, and contains only the sdkconfig files of the specific supported development boards. Any code related to the Smart Habitat firmware (not the base MicroPython firmware) is licensed under AGPL-v3.0, and is only used during the build process of the bin file (keeping the underlying MicroPython firmware under its own license).

Flashing the firmware
---------------------

You can download `.bin` file from `release` folder of the required platform and flash it to any ESP32 to start.
- For Windows systems, use the [Espressif Flash Download Tool](https://www.espressif.com/en/support/download/other-tools)
- For Linux/Mac use [Espressif esptool](https://github.com/espressif/esptool) -- [Documentation](https://docs.espressif.com/projects/esptool/en/latest/esp32s3/esptool/flashing-firmware.html)
- Download Thonny Python IDE (for Windows), or use any IDE capable of serial communication for Linux/Mac
- Upload 2 files to the ESP32 control unit 
  - `token` containing the token to be used for local API
  - `platform` containing the board name (only `ESP32_GENERIC_S3 (N2R8)` is supported for now)

About this repository
---------------------

This repository contains the following components:
- [py/](py/) -- the core Python implementation, including compiler, runtime, and
  core library.
- [mpy-cross/](mpy-cross/) -- the MicroPython cross-compiler which is used to turn scripts
  into precompiled bytecode.
- [ports/](ports/) -- ESP32-specific code.
- [lib/](lib/) -- submodules for external dependencies.
- [tests/](tests/) -- test framework and test scripts.
- [extmod/](extmod/) -- additional (non-core) modules implemented in C.
- [tools/](tools/) -- various tools, including the pyboard.py module.

"make" is used to build the components, or "gmake" on BSD-based systems.
You will also need bash, gcc, and Python 3.11/3.10 available as the command `python`, or `python3` 
(if your system only has Python 2.7 then invoke make with the additional option
`PYTHON=python2`). Some ports (rp2 and esp32) additionally use CMake.

Setting up ESP-IDF and the build environment
-----------------------------------

MicroPython on ESP32 requires the Espressif IDF version 5 (IoT development
framework, aka SDK).  The ESP-IDF includes the libraries and RTOS needed to
manage the ESP32 microcontroller, as well as a way to manage the required
build environment and toolchains needed to build the firmware.

The ESP-IDF changes quickly and MicroPython only supports certain versions.
Currently MicroPython supports v5.2.2 only
although other IDF v5 versions may also work.

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
$ ./install.sh       # (or install.bat on Windows)
$ source export.sh   # (or export.bat on Windows)
```

The `install.sh` step only needs to be done once. You will need to source
`export.sh` for every new session.

The MicroPython cross-compiler, mpy-cross
-----------------------------------------

Most ports require the [MicroPython cross-compiler](mpy-cross) to be built
first.  This program, called mpy-cross, is used to pre-compile Python scripts
to .mpy files which can then be included (frozen) into the
firmware/executable for a port.  To build mpy-cross use:

    $ cd mpy-cross
    $ make

Building the firmware
---------------------

The MicroPython cross-compiler must be built to pre-compile some of the
built-in scripts to bytecode.  This can be done by (from the root of
this repository):

```bash
$ make -C mpy-cross
```

Then to build MicroPython for the ESP32 run:

```bash
$ cd ports/esp32
$ make BOARD=ESP32_GENERIC_S3 submodules
$ make BOARD=ESP32_GENERIC_S3
```

This will produce a combined `firmware.bin` image in the `build-ESP32_GENERIC_S3/`
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
$ sudo adduser <username> dialout
```

If you are installing MicroPython to your module for the first time, or
after installing any other firmware, you should first erase the flash
completely:

```bash
$ make BOARD=ESP32_GENERIC_S3 PORT=/dev/ttyACM0 erase
```

To flash the MicroPython firmware to your ESP32 use:

```bash
$ make BOARD=ESP32_GENERIC_S3 PORT=/dev/ttyACM0 deploy
```
