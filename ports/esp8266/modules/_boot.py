import gc

gc.threshold((gc.mem_free() + gc.mem_alloc()) // 4)
import vfs
from flashbdev import bdev

if bdev:
    try:
        vfs.mount(bdev, "/")
    except:
        import inisetup

        inisetup.setup()

gc.collect()
