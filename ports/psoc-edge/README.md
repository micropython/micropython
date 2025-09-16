# PSOC™ Edge port

This port is intended to run on PSOC™ Edge microcontrollers.

## Pre-requisites

*TODO: Provide a script that install and the MTB utilities. To be done once edge tools are publicly released.*

Run the following script from MicroPython repository root to add the required tools to the system PATH:

    source tools/psoc-edge/dev-setup.sh && toolchain_setup

## Building and running on Linux

As we are working on the `psoc-edge-main` branch, first checkout that branch after cloning this repo:

    git checkout --track origin/psoc-edge-main

Retrieve submodules:

    make submodules

Then initialize the ModusToolbox™ environment: 

    make mtb_init BOARD=KIT_PSE84_AI

Build the firmware:

    make BOARD=KIT_PSE84_AI

> [!WARNING]
> Work in progress ->  This is currently only triggering the MTB project compilation.

-----------------------------------------------------
*TODO: From here on, this is not implemented:*

    make deploy

Find more information about the available makefile targets:

    make help
-----------------------------------------------------

# Run micropython

Use any serial terminal (putty, minicom..) and establish a session with your device with 115200 bauds and 8-N-1 configuration. 

