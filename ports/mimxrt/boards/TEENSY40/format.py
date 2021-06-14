# format.py
# Re-create the file system

import os
import mimxrt

bdev = mimxrt.Flash()

os.VfsLfs2.mkfs(bdev, progsize=256)
vfs = os.VfsLfs2(bdev, progsize=256)
os.mount(vfs, "/")
