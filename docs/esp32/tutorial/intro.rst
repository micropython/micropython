.. _esp32_intro:

Getting started with MicroPython on the ESP32
=============================================

Using MicroPython is a great way to get the most of your ESP32 board.  And
vice versa, the ESP32 chip is a great platform for using MicroPython.  This
tutorial will guide you through setting up MicroPython, getting a prompt, using
WebREPL, connecting to the network and communicating with the Internet, using
the hardware peripherals, and controlling some external components.

Let's get started!

Requirements
------------

The first thing you need is a board with an ESP32 chip.  The MicroPython
software supports the ESP32 chip itself and any board should work.  The main
characteristic of a board is how the GPIO pins are connected to the outside
world, and whether it includes a built-in USB-serial convertor to make the
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

The first thing you need to do is download the most recent MicroPython firmware 
.bin file to load onto your ESP32 device. You can download it from the  
`MicroPython downloads page <https://micropython.org/download#esp32>`_.
From here, you have 3 main choices:

* Stable firmware builds
* Daily firmware builds
* Daily firmware builds with SPIRAM support

If you are just starting with MicroPython, the best bet is to go for the Stable
firmware builds. If you are an advanced, experienced MicroPython ESP32 user
who would like to follow development closely and help with testing new
features, there are daily builds.  If your board has SPIRAM support you can
use either the standard firmware or the firmware with SPIRAM support, and in
the latter case you will have access to more RAM for Python objects.

Deploying the firmware
----------------------

Once you have the MicroPython firmware you need to load it onto your ESP32 device.
There are two main steps to do this: first you need to put your device in
bootloader mode, and second you need to copy across the firmware.  The exact
procedure for these steps is highly dependent on the particular board and you will
need to refer to its documentation for details.

Fortunately, most boards have a USB connector, a USB-serial convertor, and the DTR
and RTS pins wired in a special way then deploying the firmware should be easy as
all steps can be done automatically.  Boards that have such features
include the Adafruit Feather HUZZAH32, M5Stack, Wemos LOLIN32, and TinyPICO
boards, along with the Espressif DevKitC, PICO-KIT, WROVER-KIT dev-kits.

For best results it is recommended to first erase the entire flash of your
device before putting on new MicroPython firmware.

Currently we only support esptool.py to copy across the firmware.  You can find
this tool here: `<https://github.com/espressif/esptool/>`__, or install it
using pip::

    pip install esptool

Versions starting with 1.3 support both Python 2.7 and Python 3.4 (or newer).
An older version (at least 1.2.1 is needed) works fine but will require Python
2.7.

Using esptool.py you can erase the flash with the command::

    esptool.py --port /dev/ttyUSB0 erase_flash

And then deploy the new firmware using::

    esptool.py --chip esp32 --port /dev/ttyUSB0 write_flash -z 0x1000 esp32-20180511-v1.9.4.bin

Notes:

* You might need to change the "port" setting to something else relevant for your
  PC
* You may need to reduce the baudrate if you get errors when flashing
  (eg down to 115200 by adding ``--baud 115200`` into the command)
* For some boards with a particular FlashROM configuration you may need to
  change the flash mode (eg by adding ``-fm dio`` into the command)
* The filename of the firmware should match the file that you have

If the above commands run without error then MicroPython should be installed on
your board!

Serial prompt
-------------

Once you have the firmware on the device you can access the REPL (Python prompt)
over UART0 (GPIO1=TX, GPIO3=RX), which might be connected to a USB-serial
convertor, depending on your board.  The baudrate is 115200.

From here you can now follow the ESP8266 tutorial, because these two Espressif chips
are very similar when it comes to using MicroPython on them.  The ESP8266 tutorial
is found at :ref:`esp8266_tutorial` (but skip the Introduction section).

Troubleshooting installation problems
-------------------------------------

If you experience problems during flashing or with running firmware immediately
after it, here are troubleshooting recommendations:

* Be aware of and try to exclude hardware problems.  There are 2 common
  problems: bad power source quality, and worn-out/defective FlashROM.
  Speaking of power source, not just raw amperage is important, but also low
  ripple and noise/EMI in general.  The most reliable and convenient power
  source is a USB port.

* The flashing instructions above use flashing speed of 460800 baud, which is
  good compromise between speed and stability. However, depending on your
  module/board, USB-UART convertor, cables, host OS, etc., the above baud
  rate may be too high and lead to errors. Try a more common 115200 baud
  rate instead in such cases.

* To catch incorrect flash content (e.g. from a defective sector on a chip),
  add ``--verify`` switch to the commands above.

* If you still experience problems with flashing the firmware please
  refer to esptool.py project page, https://github.com/espressif/esptool
  for additional documentation and a bug tracker where you can report problems.

* If you are able to flash the firmware but the ``--verify`` option returns
  errors even after multiple retries the you may have a defective FlashROM chip.
