# PSOC™ Edge port

This port is intended to run on PSOC™ Edge microcontrollers.

## Pre-requisites

*TODO: Provide a script that install and the MTB utilities. To be done once edge tools are publicly released.*

*TODO: Add script to setup the environment variables etc.*

## Building and running on Linux

As we are working on the `psoc-edge-main` branch, first checkout that branch after cloning this repo:

    git checkout --track origin/psoc-edge-main

Retrieve submodules:

    make submodules

-----------------------------------------------------
*TODO: From here on, this is not implemented:*

Build the firmware:

    make BOARD=KIT-PSOCE84-AI

To build and program the device:

    make deploy

Find more information about the available makefile targets:

    make help
-----------------------------------------------------

# Run micropython

Use any serial terminal (putty, minicom..) and establish a session with your device with 115200 bauds and 8-N-1 configuration. 

