import gc
import uos
from flashbdev import bdev

try:
    if bdev:
        uos.mount(bdev, '/')
        print("Mounted /")
except OSError:
    import inisetup
    vfs = inisetup.setup()

gc.collect()
