# PSOC™ Edge port

This port is intended to run on PSOC™ Edge microcontrollers.

## Build environment

- an ARM cross compiler such as `arm-none-eabi-gcc` and associated binary utilities and libc. Built against version [GCC ARM Embedded 14.2.1](https://softwaretools-hosting.infineon.com/packages/com.ifx.tb.tool.mtbgccpackage/versions/14.2.1.265/artifacts/mtbgccpackage_14.2.1.265_Linux_x64.deb/download)
  
- [Infineon edgeprotecttools](https://github.com/Infineon/edgeprotecttools/). Built against version 1.6.0.

Furthermore, to deploy firmware to a device requires a custom [Infineon OpenOCD](https://softwaretools-hosting.infineon.com/packages/com.ifx.tb.tool.modustoolboxprogtools/versions/1.5.0.1534/artifacts/ModusToolboxProgtools_1.5.0.1534.deb/download). Validated version is 1.5.0.

### Setting up the environment

Make sure that `arm-none-eabi-gcc`, `edgeprotecttools` and `openocd` are in your `PATH`. For example, if you have installed the tools in `/opt`, you can add the following lines to your `.bashrc`:

    export PATH="/opt/gcc-arm-none-eabi-14.2.1/bin:/opt/edgeprotecttools-1.6.0/bin:/opt/openocd-1.5.0/bin:$PATH"

## Building and running on Linux

As we are working on the `psoc-edge-main` branch, first checkout that branch after cloning this repo:

    git checkout --track origin/psoc-edge-main

Before building the firmware for a given board the MicroPython cross-compiler
must be built; it will be used to pre-compile some of the built-in scripts to
bytecode.  The cross-compiler is built and run on the host machine, using:

    make -C mpy-cross

The following build commands should be run from the `ports/psoc-edge` directory:

Retrieve submodules:

    make submodules

Build the firmware:

    make BOARD=KIT_PSE84_AI

And flash it to the board:

    make deploy

> [!NOTE]
> This will also build the firmware if it has not been built yet.
> Use `qdeploy` target to avoid rebuilding the firmware.

If you have multiple boards connected, you can specify the serial number of the board to be programmed:

    make deploy DEV_SERIAL_NUMBER=123456

Find more information about the all available makefile targets:

    make help
-----------------------------------------------------

# Run micropython

Use any serial terminal (putty, minicom..) and establish a session with your device with 115200 bauds and 8-N-1 configuration. 

