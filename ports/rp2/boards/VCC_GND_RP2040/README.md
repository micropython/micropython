# VCC-GND Studio YD-RP2040

The VCC-GND Studio YD-RP2040 Board is based on the Raspberry Pi RP2040 and can be
purchased with 4/8/16 MiB of flash.

These boards are available from a number of resellers and often have the name
"YD-RP2040". initdc maintain the [YD-RP2040](https://github.com/initdc/YD-RP2040/tree/master)
repository containing information on the board.

## Build notes

Builds can be configured with the `BOARD_VARIANT` parameter. Valid variants
can be displayed with the `query-variant` target. An example:

```bash
> cd ports/rp2
> make BOARD=VCC_GND_RP2040 query-variants
VARIANTS: flash_4mb flash_8mb flash_16mb
> make BOARD=VCC_GND_RP2040 BOARD_VARIANT=flash_8mb submodules all  # Build the 8 MiB variant
```

`flash_4mb` is the default if `BOARD_VARIANT` is not supplied.

## Board-specific modules

The `board` module contains definitions for the onboard LED, onboard WS2812B and user button.

Example:

```python
> import board
> board.led.toggle()  # Toggles the state of the on-board LED
> board.key.value()  # Returns 0 or 1 corresponding to the state of the user key
```
