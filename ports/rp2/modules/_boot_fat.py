import vfs
import machine, rp2


# Try to mount the filesystem, and format the flash if it doesn't exist.
bdev = rp2.Flash()
try:
    vfs.mount(vfs.VfsFat(bdev), "/")
except:
    vfs.VfsFat.mkfs(bdev)
    vfs.mount(vfs.VfsFat(bdev), "/")

del vfs, bdev
