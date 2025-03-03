import gc

gc.threshold((gc.mem_free() + gc.mem_alloc()) // 4)

import sys, vfs
from flashbdev import bdev

# Try to mount the ROMFS.
try:
    vfs.mount(vfs.VfsRom(vfs.rom_ioctl(2)), "/rom")
    sys.path.insert(0, "/rom")
except:
    pass

if bdev:
    try:
        vfs.mount(bdev, "/")
    except:
        import inisetup

        inisetup.setup()

gc.collect()
