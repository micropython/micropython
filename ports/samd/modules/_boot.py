import gc
import uos
import samd

bdev = samd.Flash()

# Try to mount the filesystem, and format the flash if it doesn't exist.
fs_type = uos.VfsLfs2 if hasattr(uos, "VfsLfs2") else uos.VfsLfs1

try:
    vfs = fs_type(bdev, progsize=256)
except:
    fs_type.mkfs(bdev, progsize=256)
    vfs = fs_type(bdev, progsize=256)
uos.mount(vfs, "/")

del vfs, fs_type, bdev, uos, samd
gc.collect()
del gc
