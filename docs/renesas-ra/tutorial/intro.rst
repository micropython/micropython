.. _renesas-ra_intro:

Getting started with MicroPython on the Renesas RA
==================================================

This tutorial will guide you through setting up MicroPython,
getting a prompt, using the hardware peripherals, using internal
flash file system, reset and boot modes, and Factory reset the
internal file system.

Requirements
------------

You need a board. For the information of available boards,
please refer to the general information about Renesas RA port: :ref:`renesas-ra_general`.

You need a USB-Serial conversion cable to connect the board and your PC.
Please get a type with separate signal pins so that you can connect to
the UART TX and RX pins on the board.

Flashing the MicroPython image with J-Link OB
---------------------------------------------

The board has a builtin programmer interface called J-Link OB.
Using the J-Link Software, you can flash the binary image "firmware.hex"
of MicroPython on the board via J-Link OB.

You can download the J-Link Software and Documentation pack from https://www.segger.com/downloads/jlink/.

After installing J-Link Software, start J-Flash-Lite program.  Then specify following device in Device select menu in J-Flash-Lite.

===============  ================
    Board             Device
---------------  ----------------
 EK-RA4M1           R7FA4M1AB
 EK-RA4W1         R7FA4W1AD2CNG
 EK-RA6M1           R7FA6M1AD
 EK-RA6M2           R7FA6M2AF
 RA4M1 CLICKER      R7FA4M1AB
===============  ================

Select a firmware hex file in Data File of J-Link-Lite, and push Program Device button to flash the firmware.

Getting a prompt of MicroPython
-------------------------------

Cross connect USB-Serial conversion cable RX/TX/GND pins to TX/RX/GND pins on the board.

===============  ===============  ===============
     Board        USB Serial RX    USB Serial TX
---------------  ---------------  ---------------
 EK-RA4M1             P411             P410
 EK-RA4W1             P205             P206
 EK-RA6M1             P411             P410
 EK-RA6M2             P411             P410
 RA4M1 CLICKER        P401             P402
===============  ===============  ===============

Access the MicroPython REPL (the Python prompt) via USB serial or UART with 115200 baud rate, 1 stop bit and no parity bit using your favorite terminal software, picocom on Linux or Tera Term on Windows.  You can try on Linux::

      $ picocom /dev/ttyACM0

You can see the MicroPython REPL prompt like below::

   MicroPython v1.18-293-g339aa09b8-dirty on 2022-03-26; RA6M2_EK with RA6M2
   Type "help()" for more information.
   >>>
