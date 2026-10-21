The mboot bootloader must first be built and deployed to this board.  Once its USB DFU
port can be seen, the firmware below can be programmed as usual with any DFU loader.

## Flashing mboot with ST-LINK

Make sure that JP2 is in position 1-2 to select STLK as the 5V power source, that SW2
(BOOT0) is set to 0 (left position) and SW1 (BOOT1) is set to 1 (right position).  Then
plug in a USB cable into the ST-LINK port CN6.  This will allow mboot firmware to be
programmed to the external SPI flash via ST's tools, eg:

    make -C ports/stm32/mboot BOARD=STM32N6570_DK deploy-trusted

Once mboot is installed, change JP2 to position 3-4 to select USB as the 5V power
source, change SW1 (BOOT1) back to 0 (left position) and change the USB cable to CN18.
mboot will present a USB DFU device on this USB port, and the red LED2 should be
blinking at 1Hz to indicate that mboot is active.  If it's not active then hold the
USER1 button and press NRST, and wait until both LEDs are on, then release USER1.  Now
mboot will be active.

## Flashing mboot without ST-LINK

mboot can also be installed using only the STM32N6's own USB port, CN18, using the DFU
mode of the chip's built-in bootloader (the boot ROM).  Do not plug anything into the
ST-LINK port CN6.  Make sure that JP2 is in position 3-4 to select USB as the 5V power
source.

First build the signed (trusted) mboot image, which the boot ROM requires.  This needs
STM32CubeProgrammer installed for its signing tool:

    make -C ports/stm32/mboot BOARD=STM32N6570_DK build-STM32N6570_DK/firmware-trusted.bin

Then:

1. Set SW2 (BOOT0) to 1 (right position) and SW1 (BOOT1) to 0 (left position), then plug
   a USB cable into CN18.  The boot ROM will enumerate over USB in DFU mode.  The boot ROM
   also does this with SW2 set to 0 if the external SPI flash is empty.

2. Load mboot into RAM.  The boot ROM can't write to the external SPI flash, but it can
   load mboot into RAM and run it:

       dfu-util -a 0 -D ports/stm32/mboot/build-STM32N6570_DK/firmware-trusted.bin -R

   mboot will present a USB DFU device on this USB port, and the red LED2 should be
   blinking at 1Hz to indicate that mboot is active.  If MicroPython firmware is already
   in the external SPI flash, mboot will run it instead, so hold the USER1 button while
   loading mboot, wait until both LEDs are on, then release USER1.

3. Program mboot into the external SPI flash, using mboot's own DFU device:

       dfu-util -a 0 -s 0x70000000 -D ports/stm32/mboot/build-STM32N6570_DK/firmware-trusted.bin

4. Set SW2 (BOOT0) back to 0 (left position), leaving SW1 (BOOT1) at 0, and press NRST.
   mboot should run again just like before.  If it's not active then hold the USER1
   button and press NRST, and wait until both LEDs are on, then release USER1.  Now
   mboot will be active.

## Chip revision and signing header version

The STM32N657X0 chip is available in two silicon revisions, which require different
signing header versions when building the trusted firmware image.  Check the revision
code printed on the chip package:

- **Rev code Z (Cut 1.1)**: use `STM32_N6_HEADER_VERSION = 2.1`
- **Rev code B (Cut 2.0)**: use `STM32_N6_HEADER_VERSION = 2.3` (default in
  `mpconfigboard.mk`)

To override the version without modifying `mpconfigboard.mk`, pass it on the make
command line:

    make -C ports/stm32/mboot BOARD=STM32N6570_DK STM32_N6_HEADER_VERSION=2.3

## Flashing mboot and firmware together via ST-LINK

If the DFU device is not recognised by the host PC on CN18 after mboot is installed, a
reliable alternative is to program both mboot and the main MicroPython firmware directly
via the ST-LINK interface in a single step.  This bypasses DFU mode entirely and writes
both images to the external SPI flash using `STM32_Programmer_CLI`:

    STM32_Programmer_CLI \
      -c port=SWD mode=HOTPLUG ap=1 \
      -el <path-to-cubeprogrammer>/bin/ExternalLoader/MX66UW1G45G_STM32N6570-DK.stldr \
      -w mboot/build-STM32N6570_DK/firmware-trusted.bin 0x70000000 \
      -w build-STM32N6570_DK/firmware.bin 0x70080000 \
      -hardRst

Adjust `<path-to-cubeprogrammer>` to match the STM32CubeProgrammer installation on
your system.  The first `-w` programs the trusted mboot image at the start of the
external SPI flash (0x70000000), and the second `-w` programs the main MicroPython
firmware at 0x70080000 (512KB into the flash, immediately after mboot).

This approach works regardless of whether the USB DFU device enumerates correctly, and
is especially useful on Rev code B boards where the signing header version or alignment
settings may need to be adjusted first.
