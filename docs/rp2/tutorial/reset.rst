Factory reset
=============

If something unexpected happens and your RP2xxx-based board no longer boots
MicroPython, then you may have to factory reset it. For more details, see
:ref:`soft_bricking`.

Factory resetting the MicroPython rp2 port involves fully erasing the flash and
resetting the flash memory, so you will need to re-flash the MicroPython
firmware afterwards and copy any Python files to the filesystem again.

1. Follow the instructions on the Raspberry Pi website for `resetting flash
   memory`_.
2. Copy the MicroPython .uf2 firmware file to your board. If needed, this file
   can be found on the `MicroPython downloads page`_.

.. _resetting flash memory: https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html#resetting-flash-memory
.. _MicroPython downloads page: https://micropython.org/download/?port=rp2
