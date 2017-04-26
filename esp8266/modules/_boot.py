import gc
gc.threshold((gc.mem_free() + gc.mem_alloc()) // 4)
import uos
from flashbdev import bdev

try:
    if bdev:
        vfs = uos.VfsFat(bdev)
        uos.mount(vfs, '/flash')
        uos.chdir('/flash')
except OSError:
    import inisetup
    vfs = inisetup.setup()

gc.collect()
