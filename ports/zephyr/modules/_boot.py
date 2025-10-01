# ruff: noqa: F821
import vfs
import sys
import os
from zephyr import FlashArea
from micropython import const

_FLASH = const("/flash")
_FLASH_LIB = const("/flash/lib")
_STORAGE_KEY = const("id_storage")

# Mount the partition labeled Storage on /flash


def mount_flash_partition():
    mounts = vfs.mount()
    for mount in mounts:
        if mount[1] == _FLASH:
            return True
    return False


# Create an LFS2 filesystem on the partition labeled Storage
# and mount it on /flash


def create_flash_partition():
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


if not mount_flash_partition():
    create_flash_partition()

del sys, vfs, os, const, FlashArea
del mount_flash_partition, create_flash_partition
