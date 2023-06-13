import gc
import os
from flashbdev import bdev

try:
    if bdev:
        os.mount(bdev, "/")
except OSError:
    import inisetup

    vfs = inisetup.setup()

gc.collect()
