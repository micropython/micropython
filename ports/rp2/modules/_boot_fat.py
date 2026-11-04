def init():
    import rp2
    import vfs

    # Try to mount the filesystem, and format the flash if it doesn't exist.
    # Note: VfsFat doesn't raise when there is no filesystem, only mount raises.
    bdev = rp2.Flash()
    try:
        vfs.mount(vfs.VfsFat(bdev), "/")
    except:
        vfs.VfsFat.mkfs(bdev)
        vfs.mount(vfs.VfsFat(bdev), "/")


init()
del init
