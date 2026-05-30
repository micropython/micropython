import uos, gc, sys

import vfs
import machine

# initialize internal flash
bdev =  machine.FlashWL()
vfs = uos.VfsFat(bdev)
uos.mount(vfs, "/internal")

sys.path.append(".frozen")
gc.collect()
