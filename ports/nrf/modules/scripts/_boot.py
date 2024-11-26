import gc
import vfs
import sys
import nrf
import os

try:
    vfs.mount(vfs.VfsRom(vfs.rom_ioctl(1)), "/rom")
    sys.path.insert(0, "/rom")
except:
    pass

try:
    bdev = nrf.Flash()
    fs_type = (
        vfs.VfsLfs2
        if hasattr(vfs, "VfsLfs2")
        else (vfs.VfsLfs1 if hasattr(vfs, "VfsLfs1") else vfs.VfsFat)
    )
    vfs.mount(fs_type(bdev), "/flash")
except AttributeError:
    fs_type = None
except:
    try:
        fs_type.mkfs(bdev)
        vfs.mount(fs_type(bdev), "/flash")
    except:
        pass

if fs_type is not None:
    os.chdir("/flash")
    sys.path.append("/flash")
    sys.path.append("/flash/lib")

del os, nrf, vfs, sys, bdev, fs_type
gc.collect()
del gc
