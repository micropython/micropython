Factory reset
=============

If something unexpected happens and your ESP32-based board no longer boots
MicroPython, then you may have to factory reset it. For more details, see
:ref:`soft_bricking`.

Factory resetting the MicroPython esp32 port involves fully erasing the flash
and resetting the flash memory, so you will need to re-flash the MicroPython
firmware afterwards and copy any Python files to the filesystem again.

1. You will need the Espressif `esptool`_ installed on your system. This is the
   same tool that you may have used to initially install MicroPython on your
   board (see :ref:`installation instructions <esp32_flashing>`).
2. Find the serial port name of your board, and then use esptool to erase the
   entire flash contents::

       esptool.py -p PORTNAME erase_flash

3. Use esptool to flash the MicroPython file to your board again. If needed,
   this file and flashing instructions can be found on the `MicroPython
   downloads page`_.

.. _esptool: https://github.com/espressif/esptool
.. _MicroPython downloads page: https://micropython.org/download/?port=esp32
