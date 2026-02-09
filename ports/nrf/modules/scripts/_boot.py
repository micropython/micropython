def setup_fs():
    import gc
    import vfs
    import sys
    import nrf
    import os

    fs_type = getattr(vfs, "VfsLfs2", getattr(vfs, "VfsLfs1", getattr(vfs, "VfsFat", None)))
    try:
        bdev = nrf.Flash()
        vfs.mount(bdev, "/flash")
    except:
        if fs_type is not None:
            try:
                fs_type.mkfs(bdev)
                vfs.mount(bdev, "/flash")
            except:
                return

    os.chdir("/flash")
    sys.path.append("/flash")
    sys.path.append("/flash/lib")

    gc.collect()


setup_fs()
del setup_fs
