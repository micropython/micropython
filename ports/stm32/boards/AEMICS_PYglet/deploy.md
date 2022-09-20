### AEMICS PYglet via DFU

One you have downloaded or made the appropriate DFU file it can be flashed directly to your PYglet
using a DFU programmer (Like STM32 Cube Programmer, see[here](https://www.st.com/en/development-tools/stm32cubeprog.html)). 

The board can also be programmed via the ST DFU bootloader, using e.g. [dfu-util](http://dfu-util.sourceforge.net/) or [pydfu.py](https://github.com/micropython/micropython/blob/master/tools/pydfu.py).

To enter the bootloader the `BOOT0` pin can be connected to `3V3` during reset, or you can use `machine.bootloader()` from the MicroPython REPL.

```bash
dfu-util --alt 0 -D firmware.dfu
```

`3V3` is on Pin 1 of the PYglet module.
`BOOT0` is on Pin 16 of the PYglet module.

Check the [AEMICS PYg page](https://www.aemics.nl/pyg/) for more information.
