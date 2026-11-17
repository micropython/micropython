### WeAct Studio STM32G474CEU6 mini coreboard


### DFU update

Hold the Boot button - the button with "BOOT0" near usb-c port - while the
board is reset (either by connecting USB or by pressing reset button). Release the Boot
button shortly after the board has reset. The board ought to now be in DFU mode
and detectable as such from a connected computer.

Use a tool like [`dfu-util`](https://dfu-util.sourceforge.net/) to update the
firmware:

```bash
dfu-util --alt 0 -D firmware.dfu
```
