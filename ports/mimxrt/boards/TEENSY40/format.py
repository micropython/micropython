# format.py
# Re-create the file system

import vfs
import mimxrt

bdev = mimxrt.Flash()

vfs.VfsLfs2.mkfs(bdev, progsize=256)
fs = vfs.VfsLfs2(bdev, progsize=256)
vfs.mount(fs, "/")
