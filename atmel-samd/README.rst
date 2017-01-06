SAMD21x18
=========

This port brings MicroPython to SAMD21x18 based development boards including the
Arduino Zero, Adafruit Feather M0 Basic and Adafruit M0 Bluefruit LE.

Building
--------

To build for the Arduino Zero:

    make

To build for other boards you must change it by setting ``BOARD``. For example:

    make BOARD=feather_m0_basic

Board names are the directory names in the `boards <https://github.com/adafruit/circuitpython/tree/master/atmel-samd/boards>`_ folder.

Deploying
---------

Arduino Bootloader
^^^^^^^^^^^^^^^^^^

If your board has an existing Arduino bootloader on it then you can use bossac
to flash MicroPython. First, activate the bootloader. On Adafruit Feathers you
can double click the reset button and the #13 will fade in and out. Finally,
run bossac:

    tools/bossac_osx -e -w -v -b -R build-feather_m0_basic/firmware.bin

No Bootloader via GDB
^^^^^^^^^^^^^^^^^^^^^

This method works for loading MicroPython onto the Arduino Zero via the
programming port rather than the native USB port.

Note: These instructions are tested on Mac OSX and will vary for different
platforms.

    openocd -f ~/Library/Arduino15/packages/arduino/hardware/samd/1.6.6/variants/arduino_zero/openocd_scripts/arduino_zero.cfg

In another terminal from `micropython/atmel-samd`:

    arm-none-eabi-gdb build-arduino_zero/firmware.elf
    (gdb) tar ext :3333
    ...
    (gdb) load
    ...
    (gdb) monitor reset init
    ...
    (gdb) continue

Connecting
----------

Serial
^^^^^^

All boards are currently configured to work over USB rather than UART. To
connect to it from OSX do something like this:

    screen /dev/tty.usbmodem142422 115200

You may not see a prompt immediately because it doesn't know you connected. To
get one either hit enter to get `>>>` or do CTRL-B to get the full header.

Mass storage
^^^^^^^^^^^^

All boards will also show up as a mass storage device. Make sure to eject it
before referring to any files.

Port Specific modules
---------------------

.. toctree::
    bindings/samd/__init__
