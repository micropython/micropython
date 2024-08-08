import vfs
import sys
import machine, rp2


mapfs = rp2.Flash("mapfs")
vfs.mount(vfs.VfsMap(mapfs.ioctl(0x100, 0), mapfs), "/mapfs")
sys.path.insert(0, "/mapfs")


# Try to mount the filesystem, and format the flash if it doesn't exist.
# Note: the flash requires the programming size to be aligned to 256 bytes.
bdev = rp2.Flash()
try:
    fs = vfs.VfsLfs2(bdev, progsize=256)
except:
    vfs.VfsLfs2.mkfs(bdev, progsize=256)
    fs = vfs.VfsLfs2(bdev, progsize=256)
vfs.mount(fs, "/")

del vfs, sys, bdev, fs, mapfs
