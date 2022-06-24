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
    elif b[510:512] == b'\x55\xaa':
        return FS_FAT
    else:
        return FS_UNDEF

bdev = mimxrt.Flash()

# In case of MSC support mount as FAT
# Create a FAT FS is needed
if hasattr(mimxrt, "MSC"):
    try:
        vfs = os.VfsFat(bdev)
        os.mount(vfs, "/flash")
    except:
        os.VfsFat.mkfs(bdev)
        vfs = os.VfsFat(bdev)
        os.mount(vfs, "/flash")
# otherwise analyze the boot sector an mount accordingly
# if no valid boot sector, creat a LFS file system
else:
    fs = fs_type(bdev)
    if fs == FS_LITTLEFS:
        vfs = os.VfsLfs2(bdev, progsize=256)
    elif fs == FS_FAT:
        vfs = os.VfsFat(bdev)
    else:
        os.VfsLfs2.mkfs(bdev, progsize=256)
        vfs = os.VfsLfs2(bdev, progsize=256)
    os.mount(vfs, "/flash")

os.chdir("/flash")
sys.path.append("/flash")
sys.path.append("/flash/lib")

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
