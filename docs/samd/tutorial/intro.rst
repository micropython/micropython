.. _samd_intro:

Getting started with MicroPython on the SAMD
==============================================

Using MicroPython is a great way to get the most of your SAMD board.  And
vice versa, the SAMD chip is a great platform for using MicroPython.  This
tutorial will guide you through setting up MicroPython, getting a prompt, using
the hardware peripherals, and controlling some external components.

Let's get started!

Requirements
------------

The first thing you need is a board with an SAMD chip.  The MicroPython
software supports the SAMD chip itself and any board should work.  The main
characteristic of a board is how the GPIO pins are connected to the outside
world, and whether it includes a built-in USB-serial converter to make the
UART available to your PC.

Names of pins will be given in this tutorial using the board names ("D0")
and it should be straightforward to find which pin this corresponds to on your
particular board.

Powering the board
------------------

If your board has a USB connector on it then most likely it is powered through
this when connected to your PC.  Otherwise you will need to power it directly.
Please refer to the documentation for your board for further details.

Getting the firmware
--------------------

Firmware versions are provided at the
`MicroPython download page <https://micropython.org/download/?port=samd>`_.
You can download the most recent MicroPython firmware .uf2 file to load
onto your SAMD device.  From that download page you have two main choices:

* stable firmware builds
* daily firmware builds

If you are just starting with MicroPython, the best bet is to go for the stable
firmware builds. If you are an advanced, experienced MicroPython SAMD user
who would like to follow development closely and help with testing new
features, there are daily builds. They run through the same rigid internal
test sequence as the stable builds, but the firmware features may change.

Deploying the firmware
----------------------

Once you have the MicroPython firmware you need to load it onto your
SAMD device. Most of the boards support the following easy procedure for
firmware upload.

- Connect the board by an USB-A/USB-Micro cable to your PC.
- Push the reset button twice. Then, a drive should pop up at the
  PC's file manager. Many boards have a on-board RGB LED, which first lights up
  red, and then changes it's colour to green. The timing of the double push is
  sometimes tricky. So you may have to try to get the proper rhythm.
- Copy the .uf2 file to that newly opened drive. A LED on the board may
  flash a while. When the copy is finished, the board drive will disappear and the
  RGB led will turn from green to the initial state.

Serial prompt
-------------

Once you have the firmware on the device you can access the REPL (Python prompt)
over USB.

From there you can follow the SAMD tutorial.

Troubleshooting installation problems
-------------------------------------

If you experience problems during flashing or with running firmware immediately
after it, here are some troubleshooting recommendations:

* Be aware of and try to exclude hardware problems.  There are two common
  problems: bad power source quality, and worn-out/defective Flash ROM.
  Speaking of power source, not just raw amperage is important, but also low
  ripple and noise/EMI in general.  The most reliable and convenient power
  source is a USB port.
