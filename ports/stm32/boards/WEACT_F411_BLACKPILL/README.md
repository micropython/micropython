WeAct F411 'blackpill'
======================

The WeAct F411 blackpill board comes in a number of versions and variants.
All have a footprint for a SPI flash chip on the back, though the board is
often sold without any flash chip loaded.

At the time of writing (Sep 2024) v3.1 is the current version.
This version is sold with both 25Mhz HSE crystal (same as previous versions) and also
with a 8Mhz crystal. The end user should be careful to confirm which variant is
purchased and/or read the markings on the crystal to know which variant build to load.

The previous v2.0 boards had changed the MCU pinout for the spi flash chip so requires
soft-spi support enabled in the variant settings, unlike v3.1 or v1.3 which is
compatible with the hardware spi peripheral.

The original v1.3 boards did not include a user switch on the top, it only has
"BOOT0" and "NRST" switches to load bootloader and reset the board respectively.

For more information see: https://github.com/WeActStudio/WeActStudio.MiniSTM32F4x1

Note: The pins used by features like spiflash and USB are also broken out to the
gpio headers on the sides of the board.
If these peripherals / features are enabled then these external pins must be avoided to ensure
there are no conflicts. [pins.csv](pins.csv) should be consulted to check all pins assigned
to alternate functions on the board.

Customising the build
---------------------

After purchasing a board without any spiflash chip loaded the user can solder on
their own of any desired size. Most brands of spiflash in SO8 pinout are compatible
however some do have a slightly different protocol so may not work out of the box
with micropython. Brand compatibility is outide the scope of this doc.

Once a custom spiflash chip has been loaded onto the board micropython should
be built with the flash size specified. After doing so the spiflash chip will
be used for the FAT/LFS main filesystem.

Examples:

For a v3.1 / 25Mhz (default version) board with 16MiB flash chip loaded:
``` bash
make -C ports/stm32 BOARD=WEACT_F411_BLACKPILL SPI_FLASH_SIZE_MB=16
```

For a v3.1 / 8Mhz board with 4MiB flash chip loaded:
``` bash
make -C ports/stm32 BOARD=WEACT_F411_BLACKPILL BOARD_VARIANT=V31_XTAL_8M SPI_FLASH_SIZE_MB=4
```

For a v1.3 board with 2MiB flash chip loaded and XTAL manually replaced with 8Mhz:
``` bash
make -C ports/stm32 BOARD=WEACT_F411_BLACKPILL BOARD_VARIANT=V13 SPI_FLASH_SIZE_MB=2 XTAL_FREQ_MHZ=8
```
