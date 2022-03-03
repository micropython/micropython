import gc
import vfs
from esp32 import Partition
from flashbdev import bdev

if romfs := Partition.find(Partition.TYPE_DATA, label="romfs"):
    import sys

    sys.path.insert(0, "/romfs")
del romfs

try:
    if bdev:
        vfs.mount(bdev, "/")
except OSError:
    import inisetup

    inisetup.setup()

gc.collect()
