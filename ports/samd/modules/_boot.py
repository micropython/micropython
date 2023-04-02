import gc
import uos
import samd

samd.Flash.flash_init()
bdev = samd.Flash()

# Try to mount the filesystem, and format the flash if it doesn't exist.
fs_type = uos.VfsLfs2 if hasattr(uos, "VfsLfs2") else uos.VfsLfs1

try:
    vfs = fs_type(bdev)
except:
    fs_type.mkfs(bdev)
    vfs = fs_type(bdev)
uos.mount(vfs, "/")

gc.collect()
del uos, vfs, gc
