# ruff: noqa: F821
import sys
import os
from micropython import const
import vfs  # vfs is always available
import zephyr

# FlashArea depends on CONFIG_FLASH_MAP
FlashArea = getattr(zephyr, "FlashArea", None)

# DiskAccess depends on CONFIG_DISK_ACCESS
DiskAccess = getattr(zephyr, "DiskAccess", None)

# zephyr.FileSystem depends on CONFIG_FILE_SYSTEM and CONFIG_FLASH_MAP
FileSystem = getattr(zephyr, "FileSystem", None)

_FLASH = const("/flash")
_FLASH_LIB = const("/flash/lib")
_STORAGE_KEY = const("storage")
_FLASH_EXISTS = False


def mount_filesystem_flash():
    """Create a filesystem if needed on the FS partition "/flash"
    and mount it on /flash.
    Return True if successful, False otherwise.
    """
    if _FLASH in FileSystem.fstab():
        fs = FileSystem(_FLASH)
        retval = True
        try:
            vfs.mount(fs, _FLASH)
        except OSError:
            if getattr(fs, "mkfs", None):
                try:
                    fs.mkfs()
                    vfs.mount(fs, _FLASH)
                except OSError:
                    print("Error formatting flash partition")
                    retval = False
            else:
                retval = False
        return retval
    return False


def create_flash_partition():
    """Create an LFS2 filesystem on the partition labeled storage
    and mount it on /flash.
    Return True if successful, False otherwise.
    """
    if _STORAGE_KEY in FlashArea.areas:
        bdev = FlashArea(*FlashArea.areas[_STORAGE_KEY])
        retval = True
        try:
            vfs.mount(bdev, _FLASH)
        except OSError:
            try:
                vfs.VfsLfs2.mkfs(bdev)
                vfs.mount(bdev, _FLASH)
            except OSError:
                print("Error formatting flash partition")
                retval = False
        return retval
    return False


def mount_all_disks():
    """Now mount all the DiskAreas (if any)."""
    retval = False
    for da_name in DiskAccess.disks:
        mount_name = f"/{da_name.lower()}"
        da = DiskAccess(da_name)
        try:
            vfs.mount(da, mount_name)
            sys.path.append(f"{mount_name}/lib")
            os.chdir(mount_name)
            retval = True
        except OSError as e:
            print(f"Error mounting {da_name}: {e}")
    return retval


# Prefer FileSystem over FlashArea Access
if FileSystem and mount_filesystem_flash():
    _FLASH_EXISTS = True
elif FlashArea and create_flash_partition():
    _FLASH_EXISTS = True

# Prefer disks to /flash
if not (DiskAccess and mount_all_disks()):
    if _FLASH_EXISTS:
        os.chdir(_FLASH)

# Place /flash/lib last on sys.path
if _FLASH_EXISTS:
    sys.path.append(_FLASH_LIB)

# Cleanup globals for boot.py/main.py
del FlashArea, DiskAccess, FileSystem, zephyr
del sys, vfs, os, const
del mount_filesystem_flash, create_flash_partition, mount_all_disks, _FLASH_EXISTS
