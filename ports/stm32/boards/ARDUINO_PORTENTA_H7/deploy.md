### Via dfu-util

This board can programmed via DFU bootloader, using e.g. [dfu-util](http://dfu-util.sourceforge.net/).
To enter the DFU bootloader, double tap the reset (blue) button, or you can use `machine.bootloader()` from the MicroPython REPL.

```bash
dfu-util -w -a 0 -d 2341:035b -D build-ARDUINO_PORTENTA_H7/firmware.dfu
```

Or from MicroPython source repository:

```bash
make BOARD=ARDUINO_PORTENTA_H7 deploy
```
