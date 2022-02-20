### Pyboard v1.x via DFU

One you have downloaded the appropriate DFU file it can be flashed directly to your pyboard
using a DFU programmer. You can enter the DFU bootloader on the pyboard by executing `machine.bootloader()` at the MicroPython REPL.

Alternatively, connect 3V3 with BOOT0 and reset the board. For information about DFU programming on Windows see [this PDF](http://micropython.org/resources/Micro-Python-Windows-setup.pdf). For Linux and Mac see [here](https://github.com/micropython/micropython/wiki/Pyboard-Firmware-Update).
