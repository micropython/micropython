Mboot - MicroPython boot loader
===============================

Mboot is a custom bootloader for STM32 MCUs, and currently supports the
STM32F4xx and STM32F7xx families.  It can provide a standard USB DFU interface
on either the FS or HS peripherals, as well as a sophisticated, custom I2C
interface.  It can also load and program firmware in .dfu.gz format from a
filesystem.  It can fit in 16k of flash space, but all features enabled requires
32k.

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

   Mboot supports programming external SPI flash via the DFU and I2C
   interfaces.  SPI flash will be mapped to an address range.  To
   configure it use the following options (edit as needed):

    #define MBOOT_SPIFLASH_ADDR (0x80000000)
    #define MBOOT_SPIFLASH_BYTE_SIZE (2 * 1024 * 1024)
    #define MBOOT_SPIFLASH_LAYOUT "/0x80000000/64*32Kg"
    #define MBOOT_SPIFLASH_ERASE_BLOCKS_PER_PAGE (32 / 4)
    #define MBOOT_SPIFLASH_SPIFLASH (&spi_bdev.spiflash)
    #define MBOOT_SPIFLASH_CONFIG (&spiflash_config)

   This assumes that the board declares and defines the relevant SPI flash
   configuration structs, eg in the board-specific bdev.c file.  The
   `MBOOT_SPIFLASH_LAYOUT` string will be seen by the USB DFU utility and
   must describe the SPI flash layout.  Note that the number of pages in
   this layout description (the `64` above) cannot be larger than 99 (it
   must fit in two digits) so the reported page size (the `32Kg` above)
   must be made large enough so the number of pages fits in two digits.
   Alternatively the layout can specify multiple sections like
   `32*16Kg,32*16Kg`, in which case `MBOOT_SPIFLASH_ERASE_BLOCKS_PER_PAGE`
   must be changed to `16 / 4` to match the `16Kg` value.

   Mboot supports up to two external SPI flash devices.  To configure the
   second one use the same configuration names as above but with
   `SPIFLASH2`, ie `MBOOT_SPIFLASH2_ADDR` etc.

   To enable loading firmware from a filesystem use:

    #define MBOOT_FSLOAD (1)

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

Entering Mboot from application code
------------------------------------

To enter Mboot from a running application do the following:

1. Make sure I and D caches are disabled.

2. Load register r0 with the value 0x70ad0000.  The lower 7 bits can be
   optionally or'd with the desired I2C address.

3. Load the MSP with the value held at 0x08000000.

4. Jump to the value held at 0x08000004.

Additional data can be passed to Mboot from application code by storing this
data in a special region of RAM.  This region begins at the address held at
location 0x08000000 (which will point to just after Mboot's stack).  A
maximum of 1024 bytes can be stored here.  To indicate to Mboot that this
region is valid load register r0 with 0x70ad0080 (instead of step 2 above),
optionally or'd with the desired I2C address.

Data in this region is a sequence of elements.  Each element has the form:

    <type:u8> <len:u8> <payload...>

where `type` and `len` are bytes (designated by `u8`) and `payload` is 0 or
more bytes.  `len` must be the number of bytes in `payload`.

The last element in the data sequence must be the end element:

* END: type=1, len=0

Note: MicroPython's `machine.bootloader()` function performs steps 1-4
above, and also accepts an optional bytes argument as additional data to
pass through to Mboot.

Loading firmware from a filesystem
----------------------------------

To get Mboot to load firmware from a filesystem and automatically program it
requires passing data elements (see above) which tell where the filesystems
are located and what filename to program.  The elements to use are:

* MOUNT: type=2, len=10, payload=(<mount-point:u8> <fs-type:u8> <base-addr:u32> <byte-len:u32>)

* FSLOAD: type=3, len=1+n, payload=(<mount-point:u8> <filename...>)

`u32` means unsigned 32-bit little-endian integer.

The firmware to load must be a gzip'd DfuSe file (.dfu.gz).

The provided fwupdate.py script contains helper functions to call into Mboot
with the correct data, and also to update Mboot itself.

Example: Mboot on PYBv1.x
-------------------------

By default mboot is not used on PYBv1.x, but full mboot configuration is provided
for these boards to demonstrate how it works and for testing.  To build and
deploy mboot on these pyboards the only difference from the normal build process
is to pass `USE_MBOOT=1` to make, so that the mboot configuration is used instead
of the non-mboot configuration.

In detail for PYBv1.0 (for PYBv1.1 use PYBV11 instead of PYBV10):

1. Make sure the pyboard is in factory DFU mode (power up with BOOT0 connected to
   3V3), then build mboot and deploy it (from the stm32/mboot/ directory):

    $ make BOARD=PYBV10 USE_MBOOT=1 clean all deploy

   This will put mboot on the pyboard.

2. Now put the pyboard in mboot mode by holding down USR, pressing RST, and
   continue to hold down USR until the blue LED is lit (the 4th option in the
   cycle) and then release USR.  The red LED will blink once per second to
   indicate that it's in mboot.  Then build the MicroPython firmware and deploy
   it (from the stm32/ directory):

    $ make BOARD=PYBV10 USE_MBOOT=1 clean all deploy

   MicroPython will now be on the device and should boot straightaway.

On PYBv1.x without mboot the flash layout is as follows:

    0x08000000  0x08004000      0x08020000
    | ISR text  | filesystem    | rest of MicroPython firmware

On PYBv1.x with mboot the flash layout is as follows:

    0x08000000  0x08004000      0x08020000
    | mboot     | filesystem    | ISR and full MicroPython firmware

Note that the filesystem remains intact when going to/from an mboot configuration
so its contents will be preserved.
