import gc

gc.threshold((gc.mem_free() + gc.mem_alloc()) // 4)
import os
from flashbdev import bdev

if bdev:
    try:
        os.mount(bdev, "/")
    except:
        import inisetup

        inisetup.setup()

gc.collect()
