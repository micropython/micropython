# VCC-GND Studio YD-RP2040 

The VCC-GND Studio YD-RP2040 Board is based on the Raspberry Pi RP2040 and can be
purchased with 4/8/16 MiB of flash.

These boards are available from VCC-GND Studio's [aliexpress page](https://www.aliexpress.com/item/1005004004120604.html). 

## Build notes

Builds can be configured with the `BOARD_VARIANT` parameter. Valid variants
can be displayed with the `query-variant` target. An example:

```bash
> cd ports/rp2
> make BOARD=VCC_GND_YD_RP2040 query-variants
VARIANTS: flash_4mb flash_8mb flash_16mb
> make BOARD=VCC_GND_YD_RP2040 BOARD_VARIANT=flash_8mb submodules all  # Build the 8 MiB variant
```

`flash_8mb` is the default if `BOARD_VARIANT` is not supplied.

## Board-specific modules

The `board` module contains definitions for the onboard LED(GPIO25), WS2812 addressable RGB led (GPIO23) and user button(GPIO24).

Example:

```python
> import board
> board.led.toggle()  # Toggles the state of the on-board LED
> board.key.value()  # Returns 0 or 1 corresponding to the state of the user key
> 
> import neopixel
> from time import sleep
> n = neopixel.NeoPixel(board.neopixel, 1)
> n[0] = (255,0,0)
> n.write()
> sleep(1)
> n[0] = (0,255,0)
> n.write()
> sleep(1)
> n[0] = (0,0,255)
> n.write()
> sleep(1)
```
