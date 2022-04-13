### Via dfu-util

This board can programmed via DFU bootloader, using e.g. [dfu-util](http://dfu-util.sourceforge.net/).
To enter the DFU bootloader, double tap the reset (blue) button, or you can use `machine.bootloader()` from the MicroPython REPL.

```bash
dfu-util -w -d 2341:035b -a 0 -d 0x8040000 -D firmware.dfu
```

Or from MicroPython source repository:

```bash
make BOARD=ARDUINO_PORTENTA_H7 deploy
```
