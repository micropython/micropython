# ruff: noqa: F821
import vfs
import sys
import os
from zephyr import FlashArea, DiskAccess
from micropython import const

_FLASH = const("/flash")
_FLASH_LIB = const("/flash/lib")
_STORAGE_KEY = const("storage")
_FLASH_EXISTS = False


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
    """Create an LFS2 filesystem on the partition labeled storage
    and mount it on /flash.
    Return True if successful, False otherwise.
    """
    if _STORAGE_KEY in FlashArea.Areas:
        bdev = FlashArea(FlashArea.Areas[_STORAGE_KEY], 4096)
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


def mount_all_disks():
    # Now mount all the DiskAreas (if any).
    retval = False
    for da_name in DiskAccess.Disks:
        mount_name = f"/{da_name.lower()}"
        da = DiskAccess(da_name)
        try:
            vfs.mount(da, mount_name)
            sys.path.append(f"{mount_name}/lib")
            os.chdir(mount_name)
            retval = True
        except OSError:
            pass
    return retval


if partition_mounted(_FLASH) or create_flash_partition():
    _FLASH_EXISTS = True

# Prefer disks to /flash
if not mount_all_disks():
    if _FLASH_EXISTS:
        os.chdir(_FLASH)

if _FLASH_EXISTS:
    sys.path.append(_FLASH_LIB)

del sys, vfs, os, const, FlashArea, DiskAccess
del partition_mounted, create_flash_partition, mount_all_disks, _FLASH_EXISTS
