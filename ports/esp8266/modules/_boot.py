import gc
gc.threshold((gc.mem_free() + gc.mem_alloc()) // 4)
from flashbdev import bdev
import storage

try:
    if bdev:
        vfs = storage.VfsFat(bdev)
        storage.mount(vfs, '/')
except OSError:
    import inisetup
    inisetup.setup()

gc.collect()
