import vfs
import sys
import machine, rp2

# Try to mount the ROMFS.
try:
    vfs.mount(vfs.VfsRom(vfs.rom_ioctl(2)), "/rom")
    sys.path.insert(0, "/rom")
except:
    pass

# Try to mount the filesystem, and format the flash if it doesn't exist.
# Note: the flash requires the programming size to be aligned to 256 bytes.
bdev = rp2.Flash()
try:
    fs = vfs.VfsLfs2(bdev, progsize=256)
except:
    vfs.VfsLfs2.mkfs(bdev, progsize=256)
    fs = vfs.VfsLfs2(bdev, progsize=256)
vfs.mount(fs, "/")

del vfs, sys, bdev, fs
