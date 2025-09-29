# PSOC™ Edge port

This port is intended to run on PSOC™ Edge microcontrollers.

## Pre-requisites

The instruction below assumes you are running on a Linux host.

### Installation of MTB PSOC Edge Tools

1. Download the [ModusToolbox Tools Package 3.6](https://softwaretools-hosting.infineon.com/packages/com.ifx.tb.tool.modustoolbox/versions/3.6.0.17979/artifacts/modustoolbox_3.6.0.17979_Linux_x64.deb/download). and install the toolbox package:

```
sudo apt install ./modustoolbox_3.6.0.17979_Linux_x64.deb
```

And go to the `/opt/Tools/ModusToolbox/tools_3.6/modus-shell` directory and run the postinstall script:

```
cd /opt/Tools/ModusToolbox/tools_3.6/modus-shell
./postinstall
```

2. Install the compiler  [GCC ARM Embedded 14.2.1](https://softwaretools-hosting.infineon.com/packages/com.ifx.tb.tool.mtbgccpackage/versions/14.2.1.265/artifacts/mtbgccpackage_14.2.1.265_Linux_x64.deb/download):

```
sudo apt install ./mtbgccpackage_14.2.1.265_Linux_x64.deb
```
3. Install the [Edge Protect Security Suite 1.6.0](https://softwaretools-hosting.infineon.com/packages/com.ifx.tb.tool.modustoolboxedgeprotectsecuritysuite/versions/1.6.0.512/artifacts/modustoolboxedgeprotectsecuritysuite_1.6.0.512_Linux_x64.deb/download):

```
sudo apt install ./modustoolboxedgeprotectsecuritysuite_1.6.0.512_Linux_x64.deb
```

4. Install the [ModusToolbox Programming Tools 1.5.0](https://softwaretools-hosting.infineon.com/packages/com.ifx.tb.tool.modustoolboxprogtools/versions/1.5.0.1534/artifacts/ModusToolboxProgtools_1.5.0.1534.deb/download):

```
sudo apt install ./ModusToolboxProgtools_1.5.0.1534.deb
```

### Setting up the environment

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
  
And flash it to the board:

    make deploy BOARD=KIT_PSE84_AI

If you have multiple boards connected, you can specify the serial number of the board to be programmed:

    make deploy BOARD=KIT_PSE84_AI DEVICE_SN=123456

Find more information about the available makefile targets:

    make help
-----------------------------------------------------

# Run micropython

Use any serial terminal (putty, minicom..) and establish a session with your device with 115200 bauds and 8-N-1 configuration. 

