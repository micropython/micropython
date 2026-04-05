import uos, gc, sys

import vfs
import machine

# initialize internal flash
bdev =  machine.FlashWL()
vfs = uos.VfsFat(bdev)
uos.mount(vfs, "/internal")

sys.path.append(".frozen")
sys.path.append("/internal/apps/python")
sys.path.append("/sd/apps/python")
gc.collect()
