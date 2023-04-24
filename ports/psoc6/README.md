# PSoC6 port

This port is intended to run on Infineon PSoC™ 6 microcontrollers.

## Pre-requisites
 
The following port is using Infineon ModusToolbox™ to resolve the specific PSoC™ board resources and building flags. Before working with micropython:

1. Install [ModusToolbox](https://www.infineon.com/cms/en/design-support/tools/sdk/modustoolbox-software/). Minimum version required is 3.0.

2. In order to use the same gcc for compiling both mpy sources and ModusToolbox™ add also the latest to the system PATH.

    export PATH=/home/{user}/ModusToolbox/tools_{version}/gcc/bin:$PATH

## Building and running Linux version

As we are working on the ports/psoc6 branch (for now), first checkout that branch after cloning this repo:

    git checkout --track origin/ports/psoc6

Then initialize the ModusToolbox™ environment: 

    make mpy_mtb_init

Build the firmware:

    make

To build and program the device:

    make mpy_program

### Working from WSL

If you are working from WSL and you cannot access the USB resources of your machine. Copy the .hex file from *ports/psoc6/boards/{board}/mtb-app/APP_{board}/Debug/{hexname}.hex* to some location in your windows machine where you can flash the device with Cypress Programmer. 

# Run micropython

Use any serial terminal (putty, minicom..) and establish a session with your device with 115200 bauds and 8-N-1 configuration. 
