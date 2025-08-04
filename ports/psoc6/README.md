# PSoC6 port

This port is intended to run on Infineon PSoC™ 6 microcontrollers.

## Pre-requisites
 
The following port is using Infineon ModusToolbox™ to resolve the specific PSoC™ board resources and building flags. Before working with micropython:

1. Install [ModusToolbox](https://softwaretools.infineon.com/tools/com.ifx.tb.tool.modustoolbox). **The required version is 3.0**. The version is currently fixed as some future versions are incompatible.

2. Run the following script from MicroPython repository root to add the required tools to the system PATH, and install the udev rules:

    source tools/psoc6/dev-setup.sh && toolchain_setup

If the ModusToolbox™ has not been installed in the default path (`~/ModusToolbox`), add the path as positional argument of the `toolchain_setup` function:

    source tools/psoc6/dev-setup.sh && toolchain_setup [mtb_path]

### MacOS

> [!NOTE]  
> If you are using MacOS, you need to install GNU make. The default make on Mac OS is BSD make, which is not compatible with the Makefile used in this port. Remember to add GNU make to the system path PATH.

## Building and running Linux version

As we are working on the ports-psoc6-main branch (for now), first checkout that branch after cloning this repo:

    git checkout --track origin/ports-psoc6-main

Retrieve submodules:

    make submodules

Then initialize the ModusToolbox™ environment: 

    make mtb_init BOARD=<board-name>
    
Build the firmware:

    make 

To build and program the device:

    make program

Find more information about the available makefile targets:

    make help


# Run micropython

Use any serial terminal (putty, minicom..) and establish a session with your device with 115200 bauds and 8-N-1 configuration. 
