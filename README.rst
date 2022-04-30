Running micropython in ESP32 (Rhenan's dev board)
=================================================

Pre-requisites
==============

Install rshell using:
::
    pip3 install rshell

Open a terminal to check in which USB port the board is connected to
For MacOS:
::
    ls /dev/cu.*

A list with all connected devices shall appear. In the list the board shall connected to:
::
    /dev/cu.SLAB_USBtoUART

If the board does not show in the list, ensure the SiLabs driver has been installed.


Connecting to the board
========================

To connect to the board, using rshell, in the VSCode open the terminal and run:
::
    rshell
    connect serial /dev/cu.SLAB_USBtoUART

the following prompt shall appear on the terminal:
::
    Connecting to /dev/cu.usbserial-210 (buffer-size 512)...
    Trying to connect to REPL  connected
    Retrieving sysname ... esp32
    Testing if sys.stdin.buffer exists ... Y
    Retrieving root directories ... /boot.py/ /main.py/
    Setting time ... Apr 30, 2022 22:39:23
    Evaluating board_name ... pyboard
    Retrieving time epoch ... Jan 01, 2000


Copying files to the board
==========================