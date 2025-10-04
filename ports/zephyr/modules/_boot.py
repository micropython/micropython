# ruff: noqa: F821
import vfs
import sys
import os
from zephyr import FlashArea
from micropython import const

_SD = const("/sd")
_FLASH = const("/flash")
_FLASH_LIB = const("/flash/lib")
_STORAGE_KEY = const("id_storage")


def partition_mounted(name):
    """Check if a given partition is already mounted.
    Return True if mounted, False otherwise.
    """
    mounts = vfs.mount()
    for mount in mounts:
        if mount[1] == name:
            return True
    return False


def create_flash_partition():
    """Create an LFS2 filesystem on the partition labeled Storage
    and mount it on /flash.
    Return True if successful, False otherwise.
    """
    for key, value in FlashArea.__dict__.items():
        if key.lower() == _STORAGE_KEY:
            bdev = FlashArea(value, 4096)
            retval = True
            try:
                vfs.mount(bdev, _FLASH)
            except OSError:
                try:
                    vfs.VfsLfs2.mkfs(bdev)
                    vfs.mount(bdev, _FLASH)
                except OSError:
                    retval = False
            return retval
    return False


if partition_mounted(_FLASH) or create_flash_partition():
    _FLASH_EXISTS = True
    sys.path.append(_FLASH_LIB)

# Prefer /sd to /flash for initial working directory.
if partition_mounted(_SD):
    os.chdir(_SD)
elif _FLASH_EXISTS:
    os.chdir(_FLASH)


del sys, vfs, os, const, FlashArea
del partition_mounted, create_flash_partition, _FLASH_EXISTS
