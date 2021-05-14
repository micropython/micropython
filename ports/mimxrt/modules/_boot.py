# _boot.py
print("This is the dummy _boot.py dedicated to run intial tasks")
print("like mounting the file system")

# Try to mount the filesystem, and format the flash if it doesn't exist.
# Note: the flash requires the programming size to be aligned to 256 bytes.

import os
import mimxrt

bdev = mimxrt.Flash()

try:
    vfs = os.VfsLfs2(bdev, progsize=256)
except:
    os.VfsLfs2.mkfs(bdev, progsize=256)
    vfs = os.VfsLfs2(bdev, progsize=256)
os.mount(vfs, "/")

# del os, bdev, vfs
