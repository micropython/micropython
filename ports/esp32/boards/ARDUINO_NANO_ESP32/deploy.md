### Via dfu-util

This board can programmed via DFU bootloader, using e.g. [dfu-util](http://dfu-util.sourceforge.net/).
To enter the DFU bootloader, double tap the reset (blue) button, or you can use `machine.bootloader()` from the MicroPython REPL.

```bash
dfu-util -d 0x2341:0x0070 -R -D build-ARDUINO_NANO_ESP32/micropython.app-bin
```

Please note that the DFU bootloader comes factory flashed. Should you for any reason erase the
entire flash, the DFU bootloader will have to be re-installed. Please follow the instructions
[here](https://support.arduino.cc/hc/en-us/articles/9810414060188-Reset-the-Arduino-bootloader-on-the-Nano-ESP32)
to do so.

**Important** From the options below, download the `.app-bin` file for your board.
