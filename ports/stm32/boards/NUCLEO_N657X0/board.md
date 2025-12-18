The mboot bootloader must first be built and deployed to this board.  Make sure that
CN9 is in position 1-2 to select STLK as the 5V power source, that JP1 is in position
1-2 (lower position) and JP2 is in position 2-3 (upper position).  Then plug in a USB
cable into the ST-LINK port CN10.  This will allow mboot firmware to be programmed to
the external SPI flash via ST's tools, eg:

    make -C ports/stm32/mboot BOARD=NUCLEO_N657X0 deploy-trusted

Once mboot is installed, change CN9 to position 3-4 to select USB as the 5V power
source, change JP2 back to position 1-2 (lower position) and change the USB cable to
CN8.  mboot will present a USB DFU device on this USB port, and the red LED2 should be
blinking at 1Hz to indicate that mboot is active.  If it's not active then hold the
USER button and press NRST, and wait until all three LEDs are on, then release USER.
Now mboot will be active.

Once the USB DFU port can be seen, the firmware below can be programmed as usual with
any DFU loader.
