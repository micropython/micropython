import gc
import vfs
from esp32 import Partition
from flashbdev import bdev

if mapfs := Partition.find(Partition.TYPE_DATA, label="mapfs"):
    import sys

    sys.path.insert(0, "/mapfs")
del mapfs

try:
    if bdev:
        vfs.mount(bdev, "/")
except OSError:
    import inisetup

    inisetup.setup()

gc.collect()
