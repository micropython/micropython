CircuitPython port to Spresense
==============================

This directory contains the port of CircuitPython to Spresense. It is a compact
development board based on Sony’s power-efficient multicore microcontroller
CXD5602.

Board features:

* Integrated GPS
  * The embedded GNSS with support for GPS, QZSS and GLONASS enables applications
    where tracking is required.
* Hi-res audio output and multi mic inputs
  * Advanced 192kHz/24 bit audio codec and amplifier for audio output, and
    support for up to 8 mic input channels.
* Multicore microcontroller
  * Spresense is powered by Sony's CXD5602 microcontroller (ARM® Cortex®-M4F × 6
    cores), with a clock speed of 156 MHz.

Currently, Spresense port does not support GNSS, Audio and Multicore.

Refer to [developer.sony.com/develop/spresense/](https://developer.sony.com/develop/spresense/)
for further information about this board.

Build instructions
------------------

Pull all submodules into your clone:

    $ git submodule update --init --recursive

Build the MicroPython cross-compiler:

    $ make -C mpy-cross

Change directory to spresense:

    $ cd ports/spresense

To build circuitpython image run:

    $ make

USB connection
--------------

Add user to `dialout` group:

    $ sudo usermod -a -G dialout <user-name>

Connect the Spresense main board to the PC via the USB cable.

Flash the bootloader
--------------------

The correct bootloader is required for the Spresense board to function.

Bootloader information:

* The bootloader has to be flashed the very first time the board is used.

* You have to accept the End User License Agreement to be able to download and use the Spresense bootloader binary.

Download the spresense binaries zip archive from: <https://developer.sony.com/file/download/download-spresense-firmware-v1-4-000>

Extract spresense binaries in your PC to ports/spresense/spresense-exported-sdk/firmware/

To flash the bootloader run the command:

    $ make flash-bootloader

Flash the circuitpython.spk image
---------------------------------

To flash the firmware run the command:

    $ make flash

Accessing the board
-------------------

Connect the Spresense extension board to the PC via the USB cable.

Once built and deployed, access the CircuitPython REPL (the Python prompt) via USB. You can run:

    $ screen /dev/ttyACM0 115200
