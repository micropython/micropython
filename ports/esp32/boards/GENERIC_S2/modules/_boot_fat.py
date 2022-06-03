import gc
import uos
from flashbdev import bdev

try:
    if bdev:
        uos.mount(bdev, "/")
except OSError:
    import inisetup_fat

    vfs = inisetup_fat.setup()

gc.collect()
