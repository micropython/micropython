# _boot.py
# Try to mount the filesystem, and format the flash if it doesn't exist.
# Note: the flash requires the programming size to be aligned to 256 bytes.

import os
import vfs
import sys
import mimxrt
from machine import Pin

bdev = mimxrt.Flash()
try:
    fs = vfs.VfsLfs2(bdev, progsize=256)
except:
    vfs.VfsLfs2.mkfs(bdev, progsize=256)
    fs = vfs.VfsLfs2(bdev, progsize=256)
vfs.mount(fs, "/flash")
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

        fat = vfs.VfsFat(sdcard)
        vfs.mount(fat, "/sdcard")
        os.chdir("/sdcard")
        sys.path.append("/sdcard")
    except:
        pass  # Fail silently
