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

The first thing you need to do is download the most recent MicroPython firmware
.bin file to load onto your ESP32 device. You can download it from the
`MicroPython download page`_. Search for your particular board on this page.

.. note:: If you don't see your specific board on the download page, then it's
          very likely that one of the generic firmwares will work. These are
          listed at the top of the download page and have names matching the
          onboard Espressif chip (i.e. `ESP32 / WROOM`_, `ESP32-C3`_,
          `ESP32-S3`_, etc).

          However, you may need to double check with the vendor you purchased
          the board from.

From here, you have a choice to make:

* Download a stable firmware release.
* Download a daily firmware "Preview" build.

If you are just starting with MicroPython, the best bet is to go for the stable
Release firmware builds. If you are an advanced, experienced MicroPython ESP32
user who would like to follow development closely and help with testing new
features, then you may find the Preview builds useful.

.. _esp32_flashing:

Deploying the firmware
----------------------

Once you have the MicroPython firmware you need to load it onto your ESP32
device. There are two main steps to do this: first you need to put your device
in bootloader mode, and second you need to copy across the firmware. The exact
procedure for these steps is highly dependent on the particular board.

Detailed steps can be found on the same `MicroPython download page`_ for your
board. It's recommended that you follow the steps on the download page, as they
are customised for your particular board.

If the above commands run without error then MicroPython should be installed on
your board! Skip ahead to :ref:`esp32_serial_prompt`.

.. _esp32_troubleshooting_install:

Troubleshooting installation problems
-------------------------------------

If you experience problems during flashing or with running firmware immediately
after flashing, here are some troubleshooting recommendations:

* Esptool will try to detect the serial port where your ESP32 is connected. If
  this doesn't work, or you have multiple serial ports, then you may need to
  manually specify the port by adding the ``--port`` option to the start of the
  ``esptool.py`` command line. For example, ``esptool.py --port /dev/ttyUSB0
  <rest of line>`` for Linux or ``esptool --port COM4 <rest of line>`` for
  Windows.
* If the board isn't responding to esptool at all, it may need to be manually
  reset into the bootloader download mode. Look for a button marked "BOOT" or
  "IO0" on your board and a second button marked "RESET" or "RST". If you have
  both buttons, try these steps:

  1. Press "BOOT" (or "IO0") and hold it down.
  2. Press "RESET" (or "RST") and immediately release it.
  3. Release "BOOT" (or "IO0").
  4. Re-run the flashing steps from the download page.

  If your board doesn't have these buttons, consult the board manufacturer's
  documentation about entering bootloader download mode.
* If you get errors part-way through the flashing process then try reducing the
  speed of data transfer by removing the ``--baud 460800`` argument.
* Hardware problems can cause flashing to fail. There are two common problems:
  bad power source quality, and defective hardware (especially very low cost
  unbranded development boards). Speaking of power source, not just raw amperage
  is important, but also low ripple and noise/EMI in general. The most reliable
  and convenient power source is a USB port.
* If you still experience problems with flashing the firmware then please also
  refer to the `esptool Troubleshooting documentation`_.

.. _esp32_serial_prompt:

Serial prompt
-------------

Once you have the firmware on the device you can access the REPL (Python prompt)
over either UART0, which might be connected to a USB-serial converter depending
on your board, or the chip's built-in USB device. The baudrate is 115200.

From here you can now follow the ESP8266 tutorial, because these two Espressif chips
are very similar when it comes to using MicroPython on them.  The ESP8266 tutorial
is found at :ref:`esp8266_tutorial` (but skip the Introduction section).

.. _esptool Troubleshooting documentation: https://docs.espressif.com/projects/esptool/en/latest/esp32/troubleshooting.html
.. _MicroPython download page: https://micropython.org/download/?port=esp32
.. _ESP32 / WROOM: https://micropython.org/download/ESP32_GENERIC
.. _ESP32-C3: https://micropython.org/download/ESP32_GENERIC_C3
.. _ESP32-S3: https://micropython.org/download/ESP32_GENERIC_S3
