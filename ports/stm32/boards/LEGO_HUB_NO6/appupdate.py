# Application firmware update function for LEGO_HUB_NO6.
# MIT license; Copyright (c) 2022 Damien P. George

from micropython import const
import struct, machine, fwupdate, spiflash, pyb

_IOCTL_BLOCK_COUNT = const(4)
_IOCTL_BLOCK_SIZE = const(5)

_SPIFLASH_UPDATE_KEY_ADDR = const(1020 * 1024)
_SPIFLASH_UPDATE_KEY_VALUE = const(0x12345678)

_FILESYSTEM_ADDR = const(0x8000_0000 + 1024 * 1024)

# Roundabout way to get actual filesystem size from config.
# This takes into account the 1M "reserved" section of the flash memory.
flash = pyb.Flash(start=0)
_FILESYSTEM_LEN = flash.ioctl(_IOCTL_BLOCK_COUNT, None) * flash.ioctl(_IOCTL_BLOCK_SIZE, None)


def update_app(filename):
    print(f"Updating application firmware from {filename}")

    # Create the elements for the mboot filesystem-load operation.
    elems = fwupdate.update_app_elements(filename, _FILESYSTEM_ADDR, _FILESYSTEM_LEN)
    if not elems:
        return

    # Create the update key.
    key = struct.pack("<I", _SPIFLASH_UPDATE_KEY_VALUE)

    # Create a SPI flash object.
    spi = machine.SoftSPI(
        sck=machine.Pin.board.FLASH_SCK,
        mosi=machine.Pin.board.FLASH_MOSI,
        miso=machine.Pin.board.FLASH_MISO,
        baudrate=50_000_000,
    )
    cs = machine.Pin(machine.Pin.board.FLASH_NSS, machine.Pin.OUT, value=1)

    # We can't use pyb.Flash() because we need to write to the "reserved" 1M area.
    flash = spiflash.SPIFlash(spi, cs)

    # Write the update key and elements to the SPI flash.
    flash.erase_block(_SPIFLASH_UPDATE_KEY_ADDR)
    flash.write(_SPIFLASH_UPDATE_KEY_ADDR, key + elems)

    # Enter mboot with a request to do a filesystem-load update.
    # If there is a power failure during the update (eg battery removed) then
    # mboot will read the SPI flash update key and elements and retry.
    machine.bootloader(elems)
