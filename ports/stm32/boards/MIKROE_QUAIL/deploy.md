### Quail via DFU

Quail can be programmed via USB with the ST DFU bootloader, using
e.g. [dfu-util](http://dfu-util.sourceforge.net/) or
[pydfu.py](https://github.com/micropython/micropython/blob/master/tools/pydfu.py).

To enter the bootloader press and release the Reset button while holding the
Boot button.  Alternatively, you can use `machine.bootloader()` from the
MicroPython REPL.

```bash
dfu-util --alt 0 -D firmware.dfu
```
