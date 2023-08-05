### Via dfu-util

This board can programmed via DFU bootloader, using e.g. [dfu-util](http://dfu-util.sourceforge.net/).
To enter the DFU bootloader, double tap the reset (blue) button, or you can use `machine.bootloader()` from the MicroPython REPL.

```bash
dfu-util -d 0x2341:0x0070 -R -D build-ARDUINO_NANO_ESP32/micropython.bin
```
