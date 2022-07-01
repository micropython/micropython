# _boot.py
# Try to mount the filesystem, and format the flash if it doesn't exist.
# Note: the flash requires the programming size to be aligned to 256 bytes.

import os
import sys
import mimxrt
from machine import Pin

FS_UNDEF = const(0)
FS_LITTLEFS = const(1)
FS_FAT = const(2)


def fs_type(bdev):
    b = bytearray(512)
    bdev.readblocks(0, b)
    if b[40:48] == b"littlefs" or b[8:16] == b"littlefs":
        return FS_LITTLEFS
    elif b[510:512] == b"\x55\xaa":
        return FS_FAT
    else:
        return FS_UNDEF


bdev = mimxrt.Flash()

# try to mount the fs accorfing to the boot sector
# if that fails, (re-)create it with a preference for FAT on
# boards with MSC support.
fs = fs_type(bdev)

if fs == FS_FAT or (fs == FS_UNDEF and hasattr(mimxrt, "MSC")):
    try:
        vfs = os.VfsFat(bdev)
        os.mount(vfs, "/flash")
    except:
        os.VfsFat.mkfs(bdev)
        vfs = os.VfsFat(bdev)
        os.mount(vfs, "/flash")
    usb_mode = "vcp+msc"
else:
    try:
        vfs = os.VfsLfs2(bdev, progsize=256)
        os.mount(vfs, "/flash")
    except:
        os.VfsLfs2.mkfs(bdev, progsize=256)
        vfs = os.VfsLfs2(bdev, progsize=256)
        os.mount(vfs, "/flash")
    usb_mode = "vcp"

os.chdir("/flash")
sys.path.append("/flash")
sys.path.append("/flash/lib")

try:
    from set_usb_mode import usb_mode
except:
    pass

# Configure USB mode according to the default or the
# config file in the file system
if hasattr(mimxrt, "usb_mode"):
    try:
        mimxrt.usb_mode(usb_mode)
    except:
        pass

# do not mount the SD card if SKIPSD exists.
try:
    os.stat("SKIPSD")
except:
    try:
        from machine import SDCard

        sdcard = SDCard(1)

        fat = os.VfsFat(sdcard)
        os.mount(fat, "/sdcard")
        os.chdir("/sdcard")
        sys.path.append("/sdcard")
    except:
        pass  # Fail silently
