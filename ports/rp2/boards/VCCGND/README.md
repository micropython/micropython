# VCC-GND Studio RP2040

The YD-RP2040 Board from VCC-GND Studio is based on the Raspberry Pi RP2040
and can be found with various size of flash.

These boards are available from a number of resellers and often simply
names as a "Black RP2040 Board". Board information is available from
links on http://vcc-gnd.com/ site.

## Build notes

Builds can be configured with the `BOARD_VARIANT` parameter. Valid variants
can be displayed with the `query-variant` target. An example:

```bash
> cd ports/rp2
> make BOARD=VCCGND query-variants
VARIANTS: flash_2mb flash_4mb flash_8mb flash_16mb
> make BOARD=VCCGND BOARD_VARIANT=flash_8mb submodules all  # Build the 8 MiB variant
```

`flash_16mb` is the default if `BOARD_VARIANT` is not supplied.

## Board-specific modules

The `board` module contains definitions for the onboard LED and user button.

Example:

```python
> import board
> board.led.toggle()  # Toggles the state of the on-board LED
> board.key.value()  # Returns 0 or 1 corresponding to the state of the user key
```
