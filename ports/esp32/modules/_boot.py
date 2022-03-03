import gc
import vfs
import sys
from flashbdev import bdev

try:
    vfs.mount(vfs.VfsRom(vfs.rom_ioctl(2)), "/rom")
    sys.path.insert(0, "/rom")
except:
    pass

try:
    if bdev:
        vfs.mount(bdev, "/")
except OSError:
    import inisetup

    inisetup.setup()

gc.collect()

del sys
