### Via dfu-util

This board can be programmed via DFU bootloader, using e.g. [dfu-util](http://dfu-util.sourceforge.net/).
To enter the DFU bootloader, double tap the recessed reset button, or you can use `machine.bootloader()` from the MicroPython REPL.

The Arduino Opta indicates its DFU state by blinking the green LED above the RESET button.

**NOTE**: The board might remain in DFU mode after the upload, a simple reset will bring the board back to Run-Time.

```bash
dfu-util -w -a 0 -d 2341:0364 -D build-ARDUINO_OPTA/firmware.dfu
```

Or from MicroPython source repository, making sure the current directory is `ports/stm32`:

```bash
make BOARD=ARDUINO_OPTA deploy
```
