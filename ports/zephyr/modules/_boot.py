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
_FS_EXISTS = False


def mount_filesystem_flash():
    """Create a filesystem if needed on the FS partition "/flash"
    and mount it on /flash.
    Return True if successful, False otherwise.
    """
    if _FLASH not in FileSystem.fstab():
        return False
    fs = FileSystem(_FLASH)
    try:
        vfs.mount(fs, _FLASH)
    except OSError:
        if not hasattr(fs, "mkfs"):
            return False
        try:
            fs.mkfs()
            vfs.mount(fs, _FLASH)
        except OSError:
            print("Error formatting flash partition")
            return False
    return True


def create_flash_partition():
    """Create an LFS2 filesystem on the partition labeled storage
    and mount it on /flash.
    Return True if successful, False otherwise.
    """
    if _STORAGE_KEY not in FlashArea.areas:
        return False
    bdev = FlashArea(*FlashArea.areas[_STORAGE_KEY])
    try:
        vfs.mount(bdev, _FLASH)
    except OSError:
        if not hasattr(vfs, "VfsLfs2"):
            return False
        try:
            vfs.VfsLfs2.mkfs(bdev)
            vfs.mount(bdev, _FLASH)
        except OSError:
            print("Error formatting flash partition")
            return False
    return True


def mount_all_filesystems():
    """Mount all filesystems excluding "/flash"
    Return True if successful, False otherwise.
    """
    retval = False
    for fs_str in FileSystem.fstab():
        if fs_str == _FLASH:
            continue
        fs = FileSystem(fs_str)
        try:
            vfs.mount(fs, fs_str)
            sys.path.append(f"{fs_str}/lib")
            os.chdir(fs_str)
            retval = True
        except OSError as e:
            print(f"Error mounting {fs_str}: {e}")
    return retval


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
if FileSystem and mount_all_filesystems():
    _FS_EXISTS = True
if FileSystem and mount_filesystem_flash():
    _FLASH_EXISTS = True
elif FlashArea and create_flash_partition():
    _FLASH_EXISTS = True

# Prefer filesystems (excluding /flash) to disks
# Prefer disks to /flash
if not (_FS_EXISTS or (DiskAccess and mount_all_disks())):
    if _FLASH_EXISTS:
        os.chdir(_FLASH)

# Place /flash/lib last on sys.path
if _FLASH_EXISTS:
    sys.path.append(_FLASH_LIB)

# Cleanup globals for boot.py/main.py
del FlashArea, DiskAccess, FileSystem, zephyr
del sys, vfs, os, const
del mount_filesystem_flash, create_flash_partition, mount_all_disks, mount_all_filesystems
del _FLASH_EXISTS, _FS_EXISTS
