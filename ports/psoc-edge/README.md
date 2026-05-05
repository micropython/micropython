# PSOC™ Edge port

This port is intended to run on PSOC™ Edge microcontrollers.

## Build environment

Install the following tools on your Linux host machine: 

 - An ARM cross compiler such as `arm-none-eabi-gcc` and associated binary utilities and libc. The minimum required version is 14.2.1. 
 
    Download the [Infineon MTB GCC package](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.mtbgccpackage) and install it with the command:
   
        sudo apt install ./mtbgccpackage_14.2.1.265_Linux_x64.deb
        
    Alternatively, you can also install the `gcc-arm-none-eabi` official package:
  
        sudo apt-get install gcc-arm-none-eabi libnewlib-arm-none-eabi
  
- [edgeprotecttools](https://github.com/Infineon/edgeprotecttools/). The minimum required version is 1.6.0. 

    It can be installed from PyPi via `pip`:

        sudo pip install edgeprotecttools
        
- [Infineon OpenOCD](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.modustoolboxprogtools). The minimum required version is 1.5.0.

    Download it and install with the command:

        sudo apt install ./ModusToolboxProgtools_1.5.0.1534.deb

    Alternatively, you can download it from this [release GitHub repo](https://github.com/Infineon/openocd/releases)

        wget -O openocd.tar.gz https://github.com/Infineon/openocd/releases/download/release-v5.12.0/openocd-5.12.0.4170-linux.tar.gz
        
    And unarchive it in the desired location. For example, in the `/opt/openocd` dir:

        mkdir -p /opt/openocd
        tar -xzf openocd.tar.gz -C /opt/openocd --strip-components=1

### Setting up the environment

Make sure that `arm-none-eabi-gcc`, `edgeprotecttools` and `openocd` are added to the system `PATH`.

## Building and running on Linux

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

Find more information about all the available makefile targets:

    make help
-----------------------------------------------------

# Run micropython

Use any serial terminal (e.g. putty, minicom..) and establish a session with your device with 115200 bauds and 8-N-1 configuration. 

