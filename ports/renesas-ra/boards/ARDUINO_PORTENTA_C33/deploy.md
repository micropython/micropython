### dfu-util

This board can programmed via DFU bootloader, using e.g. [dfu-util](http://dfu-util.sourceforge.net/).
To enter the DFU bootloader, double tap the reset (blue) button, or you can use `machine.bootloader()` from the MicroPython REPL.

```bash
dfu-util -a 0 -d 2341:0368 -D build-ARDUINO_PORTENTA_C33/firmware.bin -Q # -s :leave
```
