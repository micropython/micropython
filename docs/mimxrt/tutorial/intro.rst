.. _mimxrt_intro:

Getting started with MicroPython on the i.MXRT
==============================================

Using MicroPython is a great way to get the most of your i.MXRT board.  And
vice versa, the i.MXRT chip is a great platform for using MicroPython.  This
tutorial will guide you through setting up MicroPython, getting a prompt, using
the hardware peripherals, and controlling some external components.

Let's get started!

Requirements
------------

The first thing you need is a board with an i.MXRT chip.  The MicroPython
software supports the i.MXRT chip itself and any board should work.  The main
characteristic of a board is how the GPIO pins are connected to the outside
world, and whether it includes a built-in USB-serial converter to make the
UART available to your PC.

Names of pins will be given in this tutorial using the chip names (eg GPIO2)
and it should be straightforward to find which pin this corresponds to on your
particular board.

Powering the board
------------------

If your board has a USB connector on it then most likely it is powered through
this when connected to your PC.  Otherwise you will need to power it directly.
Please refer to the documentation for your board for further details.

Getting the firmware
--------------------

At the moment the MicroPython web site does not offer pre-built packages. So
you have to build the firmware from the sources, as detailed in the README.md
files of MicroPyton. The make procedure for the respective board will create
the firmware.bin or firmware.hex file.

Once firmware versions are provided by MicroPython, you can download the
most recent MicroPython firmware .hex or .bin file to load onto your
i.MXRT device. You can download it from the 
`MicroPython downloads page <https://micropython.org/download/all>`_.
From here, you have 2 main choices:

* Stable firmware builds
* Daily firmware builds

If you are just starting with MicroPython, the best bet is to go for the Stable
firmware builds. If you are an advanced, experienced MicroPython i.MXRT user
who would like to follow development closely and help with testing new
features, there are daily builds.

Deploying the firmware
----------------------

Once you have the MicroPython firmware you need to load it onto your
i.MXRT device. The exact procedure for these steps is highly dependent
on the particular board and you will need to refer to its documentation
for details.

Teensy 4.0 and 4.1
~~~~~~~~~~~~~~~~~~

For Teensy 4.0 and 4.1 you have to use the built-in loader together with the PC
loader provided by PJRC. The built-in loader will be activated by pushing the
button on the board. Then you can upload the firmware with the command:

teensy_loader_cli --mcu=imxrt1062 -v -w firmware.hex

IMXRT10xx-EVK boards
~~~~~~~~~~~~~~~~~~~~

The IMXRT10xx-EVK boards have a second USB port connected to a support MCU.
Connecting that USB port to your PC will register
a disk drive with the name of the board. Just copy the firmware.bin file to this
drive. That will start the flashing procedure. You will know that the flash
was complete, if that drive disappears and reappears.
If you decided to install the very useful Segger open-SDA firmware on that
sidekick MCU, then you have to use the debugger software
to upload the i.MXRT firmware.

Seed ARCH MIX board
~~~~~~~~~~~~~~~~~~~

Firmware upload to the Seed ARCH MIX board is less comfortable. The vendor suggest
using J-Link as a method and tool. For that, follow the instructions given by Seed
in their Wiki at 
https://wiki.seeedstudio.com/Arch_Mix/#flashing-arduino-bootloader-to-arch-mix. 
You will need a J-Link debug probe and software. What worked for me was the
Segger JLlink edu or Segger JLink edu mini. As matching loader you can use
JFlashLite. The target address for loading is 0x60000000.

The MIMXRT family also support a serial upload method. The software for Serial
Upload is provided by NXP. A Linux shell script is included in the MicroPython
repository. Steps using it:

- Connect J3, Pin 19 to 3.3V (GPIO_AD_B0_05).
- Change the DIP-Switch settings from off-off-on-off to off-off-off-on
- Push Reset
- Run the upload with: ./FLASH.sh <firmware_image_file name>
- Once the upload has finished, set the DIP-switch back to off-off-on-off.
- Remove the Jumper to J3, Pin19 and push reset

To avoid, running the Flash loader as superuser, you can cope provided the udev-rules
script to /etc/udev/rules.d/. The sources for the binaries of blhost and sdphost are
available at NXP provided under BSD-3-Clause License. The FLASH.sh script is provided
by user Boris Lobosevic. FLASH.sh calls two binaries, blhost and sdphost, which are
provided by NXP as well. A version of these binaries and the script can be downloaded
at https://github.com/robert-hh/Shared-Stuff/blob/master/mimxrt_serial_downloader.zip.

Serial downloading can be used for the NXP MIMXRT boards as well. But the built-in loader
is much more convenient to use.


Serial prompt
-------------

Once you have the firmware on the device you can access the REPL (Python prompt)
over USB.

From here you can now follow the i.MXRT tutorial.

Troubleshooting installation problems
-------------------------------------

If you experience problems during flashing or with running firmware immediately
after it, here are troubleshooting recommendations:

* Be aware of and try to exclude hardware problems.  There are 2 common
  problems: bad power source quality, and worn-out/defective Flash ROM.
  Speaking of power source, not just raw amperage is important, but also low
  ripple and noise/EMI in general.  The most reliable and convenient power
  source is a USB port.