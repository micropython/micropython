# ruff: noqa: F821
import vfs
import sys
import os
from zephyr import FlashArea
from micropython import const

_FLASH = const("/flash")
_FLASH_LIB = const("/flash/lib")
_STORAGE_KEY = const("id_storage")


def flash_partition_mounted():
    """Check if /flash is already mounted"""
    mounts = vfs.mount()
    for mount in mounts:
        if mount[1] == _FLASH:
            return True
    return False


def create_flash_partition():
    """Create an LFS2 filesystem on the partition labeled Storage
    and mount it on /flash"""
    for key, value in FlashArea.__dict__.items():
        if key.lower() == _STORAGE_KEY:
            bdev = FlashArea(value, 4096)
            try:
                vfs.mount(bdev, _FLASH)
            except OSError:
                vfs.VfsLfs2.mkfs(bdev)
                vfs.mount(bdev, _FLASH)
            sys.path.append(_FLASH_LIB)
            os.chdir(_FLASH)


if not flash_partition_mounted():
    create_flash_partition()

del sys, vfs, os, const, FlashArea
del flash_partition_mounted, create_flash_partition
