import gc
gc.threshold((gc.mem_free() + gc.mem_alloc()) // 4)
import uos
from flashbdev import bdev

if bdev:
    try:
        uos.mount(bdev, '/')
    except:
        import inisetup
        inisetup.setup()

gc.collect()
