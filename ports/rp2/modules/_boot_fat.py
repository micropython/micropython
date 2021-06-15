import os
import machine, rp2


# Try to mount the filesystem, and format the flash if it doesn't exist.
bdev = rp2.Flash()
try:
    vfs = os.VfsFat(bdev)
    os.mount(vfs, "/")
except:
    os.VfsFat.mkfs(bdev)
    vfs = os.VfsFat(bdev)
os.mount(vfs, "/")

del os, bdev, vfs
