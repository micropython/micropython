### STM32H743 CORE

STM32H743 CORE can be updated using
[DFU](https://en.wikipedia.org/wiki/USB?useskin=vector#Device_Firmware_Upgrade_mechanism).

### DFU update

Hold the Boot button - while the board is reset (either by connecting USB or by pressing reset).
Release the Boot button shortly after the board has reset.
The board ought to now be in DFU mode and detectable as such from a connected computer.

Use a tool like [`dfu-util`](https://dfu-util.sourceforge.net/) to update the
firmware:

```bash
dfu-util --alt 0 -D firmware.dfu
```
