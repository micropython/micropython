import gc
import vfs
from esp32 import Partition
from flashbdev import bdev

"""
if romfs := Partition.find(Partition.TYPE_DATA, label="romfs"):
    import sys

    # TODO support the buffer protocol on Partition types to make this work
    vfs.mount(vfs.VfsRom(romfs), "/rom")
    sys.path.insert(0, "/rom")
del romfs
"""

try:
    if bdev:
        vfs.mount(bdev, "/")
except OSError:
    import inisetup

    inisetup.setup()

gc.collect()
