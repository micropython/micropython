.. _nrf52_intro:

Getting started with MicroPython on the nRF52832
=============================================

Using MicroPython is a great way to get the most of your nRF52832 board.  And
vice versa, the nRF52832 chip is a great platform for using MicroPython.  This
tutorial will guide you through setting up MicroPython, getting a prompt, using
WebBluetooth REPL, connecting to the mesh and communicating with the bluetooth, using
the hardware peripherals, and controlling some external components.

Let's get started!

Requirements
------------

The first thing you need is a board with an nRF52832 chip.  The MicroPython
software supports the nRF52832 chip itself and any board should work.  The main
characteristic of a board is how the GPIO pins are connected to the outside
world, and whether it includes a built-in USB-serial convertor to make the
UART available to your PC.

Names of pins will be given in this tutorial using the chip names (eg Pin(2))
and it should be straightforward to find which pin this corresponds to on your
particular board.

Powering the board
------------------

If your board has a USB connector on it then most likely it is powered through
this when connected to your PC.  Otherwise you will need to power it directly.
Please refer to the documentation for your board for further details.

Getting the firmware
--------------------

The first thing you need to do is download the most recent MicroPython firmware 
.hex file to load onto your nRF52832 device. You can download it from the github website 
 <https://github.com/Afantor/Afantor_Bluefruit52_MicroPython>.
From here, you have 2 main choices:

* Stable firmware builds
* Daily firmware builds
* Stable firmware with ble stack builds
* Daily firmware with ble stack builds

If you are just starting with MicroPython, the best bet is to go for the Stable
firmware builds. If you are an advanced, experienced MicroPython nRF52832 user
who would like to follow development closely and help with testing new
features, there are daily builds.  

Deploying the firmware
----------------------

Once you have the MicroPython firmware you need to load it onto your nRF52832 device.
There are two main steps to do this: first you need to put your device in
bootloader mode, and second you need to copy across the firmware.  The exact
procedure for these steps is highly dependent on the particular board and you will
need to refer to its documentation for details.

Fortunately, most boards have a USB connector, a USB-serial convertor, and the DTR
 pins wired in a special way then deploying the firmware should be easy as
all steps can be done automatically.  Boards that have such features
include the Adafruit Feather bluefruit and Afantor Bluefruit52 dev-kits.

For best results it is recommended to first erase the entire flash of your
device before putting on new MicroPython firmware.

Currently we only support nrfjprog tool to copy across the firmware. 

First, please connect to the SWD interface of nRF52 through the SWD pin of Jlink.::

    GND---GND
    SDIO---SDIO
    SCLK---SCLK

Open cmd(windows) or Terminal(linux),Then
Using nrfjprog you can erase the flash with the command::

    nrfjprog –e

And then deploy the new firmware using::

    nrfjprog --program Bluefruit52_firmware_20190314.hex --sectorerase -f nrf52

reset the board::

    nrfjprog --reset -f nrf52

Notes:

* You might need to change the "port" setting to something else relevant for your
  PC
* You may need to check the nrfjprog tool if you get errors when flashing
  (eg nrfjprog version or JlinkARM version)
* The filename of the firmware should match the file that you have

If the above commands run without error then MicroPython should be installed on
your board!

Serial prompt
-------------

Once you have the firmware on the device you can access the REPL (Python prompt)
over UART1 (GPIO0.06=TX, GPIO0.08=RX), which might be connected to a USB-serial
convertor, depending on your board.  The baudrate is 115200.

Troubleshooting installation problems
-------------------------------------

If you experience problems during flashing or with running firmware immediately
after it, here are troubleshooting recommendations:

* To catch incorrect flash content (e.g. from a defective sector on a chip),
  add ``-e`` switch to the commands above.

