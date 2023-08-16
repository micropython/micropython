import gc
import os
import samd
import sys

bdev = samd.Flash()

# Try to mount the filesystem, and format the flash if it doesn't exist.
fs_type = os.VfsLfs2 if hasattr(os, "VfsLfs2") else os.VfsLfs1

try:
    vfs = fs_type(bdev, progsize=256)
except:
    fs_type.mkfs(bdev, progsize=256)
    vfs = fs_type(bdev, progsize=256)
os.mount(vfs, "/")
sys.path.append("/lib")

del vfs, fs_type, bdev, os, samd, sys
gc.collect()
del gc
