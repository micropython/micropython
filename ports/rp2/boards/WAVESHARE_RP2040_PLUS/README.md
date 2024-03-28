# Waveshare RP2040-Plus

The Waveshare RP2040-Plus Board is based on the Raspberry Pi RP2040 and can be
purchased with 4/16 MiB of flash.

These boards are available from a number of resellers 
Waveshare maintain the [Wiki:RP2040-Plus](https://www.waveshare.com/wiki/RP2040-Plus)
Wiki containing information on the board.

## Build notes

Builds can be configured with the `BOARD_VARIANT` parameter. Valid variants
can be displayed with the `query-variant` target. An example:

```bash
> cd ports/rp2
> make BOARD=WAVESHARE_RP2040_PLUS query-variants
VARIANTS: flash_4mb flash_16mb
> make BOARD=WAVESHARE_RP2040_PLUS BOARD_VARIANT=flash_16mb submodules all  # Build the 16 MiB variant
```

`flash_4mb` is the default if `BOARD_VARIANT` is not supplied.
