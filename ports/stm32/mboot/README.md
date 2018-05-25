Mboot - MicroPython boot loader
===============================

Mboot is a custom bootloader for STM32 MCUs, and currently supports the
STM32F4xx and STM32F7xx families.  It can provide a standard USB DFU interface
on either the FS or HS peripherals, as well as a sophisticated, custom I2C
interface.  It fits in 16k of flash space.

How to use
----------

1. Configure your board to use a boot loader by editing the mpconfigboard.mk
   and mpconfigboard.h files.  For example, for an F767 be sure to have these
   lines in mpconfigboard.mk:

    LD_FILES = boards/stm32f767.ld boards/common_bl.ld
    TEXT0_ADDR = 0x08008000

   And this in mpconfigboard.h (recommended to put at the end of the file):

    // Bootloader configuration
    #define MBOOT_I2C_PERIPH_ID 1
    #define MBOOT_I2C_SCL (pin_B8)
    #define MBOOT_I2C_SDA (pin_B9)
    #define MBOOT_I2C_ALTFUNC (4)

   To configure a pin to force entry into the boot loader the following
   options can be used (with example configuration):

    #define MBOOT_BOOTPIN_PIN (pin_A0)
    #define MBOOT_BOOTPIN_PULL (MP_HAL_PIN_PULL_UP)
    #define MBOOT_BOOTPIN_ACTIVE (0)

2. Build the board's main application firmware as usual.

3. Build mboot via:

    $ cd mboot
    $ make BOARD=<board-id>

   That should produce a DFU file for mboot.  It can be deployed using
   USB DFU programming via (it will be placed at location 0x08000000):

    $ make BOARD=<board-id> deploy

4. Reset the board while holding USR until all 3 LEDs are lit (the 4th option in
   the cycle) and then release USR.  LED0 will then blink once per second to
   indicate that it's in mboot

5. Use either USB DFU or I2C to download firmware.  The script mboot.py shows how
   to communicate with the I2C boot loader interface.  It should be run on a
   pyboard connected via I2C to the target board.
