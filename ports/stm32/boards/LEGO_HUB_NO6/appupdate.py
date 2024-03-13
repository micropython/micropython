# Application firmware update function for LEGO_HUB_NO6.
# MIT license; Copyright (c) 2022-2024 Damien P. George

from micropython import const
import random, struct, machine, fwupdate, spiflash, pyb

_IOCTL_BLOCK_COUNT = const(4)
_IOCTL_BLOCK_SIZE = const(5)

# Mboot addresses the external SPI flash at this location.
_MBOOT_SPIFLASH_BASE_ADDR = 0x80000000

# The raw filesystem is in the first 1MiB of external SPI flash,
# but skip the first and last flash sectors.
_RAW_FILESYSTEM_ADDR = const(4 * 1024)
_RAW_FILESYSTEM_LEN = const(1016 * 1024)


def _copy_file_to_raw_filesystem(filename, flash, block):
    block_count = flash.ioctl(_IOCTL_BLOCK_COUNT, 0)
    block_size = flash.ioctl(_IOCTL_BLOCK_SIZE, 0)
    buf = bytearray(block_size)
    with open(filename, "rb") as file:
        while True:
            n = file.readinto(buf)
            if not n:
                break
            flash.writeblocks(block, buf)
            block += 1
            if block >= block_count:
                print("|", end="")
                block = 0
            print(".", end="")
    print()


def update_app(filename):
    print(f"Updating application firmware from {filename}")

    # We can't use pyb.Flash() because we need to write to the "reserved" 1M area.
    flash = spiflash.SPIFlash(start=_RAW_FILESYSTEM_ADDR, len=_RAW_FILESYSTEM_LEN)

    # Partition the raw filesystem into two segments for wear levelling.
    block_count = flash.ioctl(_IOCTL_BLOCK_COUNT, 0)
    block_size = flash.ioctl(_IOCTL_BLOCK_SIZE, 0)
    block_start = random.randrange(0, block_count)
    print(f"Raw filesystem block layout: 0 .. {block_start} .. {block_count}")

    # Copy the file to the special raw filesystem.
    _copy_file_to_raw_filesystem(filename, flash, block_start)

    # Enter mboot with a request to do a filesystem-load update.
    # Note: the filename doesn't mean anything here, but still needs to be non-empty.
    fwupdate.update_mpy(
        filename,
        fs_type=fwupdate.VFS_RAW,
        fs_base=_MBOOT_SPIFLASH_BASE_ADDR + _RAW_FILESYSTEM_ADDR + block_start * block_size,
        fs_len=(block_count - block_start) * block_size,
        fs_base2=_MBOOT_SPIFLASH_BASE_ADDR + _RAW_FILESYSTEM_ADDR,
        fs_len2=block_start * block_size,
    )
