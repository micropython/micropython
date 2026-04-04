import uos, gc, sys

import vfs
import mch2022

# initialize internal flash
bdev = mch2022.wl_blockdev_make_new()
vfs = uos.VfsFat(bdev)
uos.mount(vfs, "/internal")

sys.path.append(".frozen")
sys.path.append("/internal/apps/python")
sys.path.append("/sd/apps/python")
gc.collect()
