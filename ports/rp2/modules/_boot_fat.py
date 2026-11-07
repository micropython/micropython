def init():
    import rp2
    import vfs

    # Try to mount the filesystem, and format the flash if it doesn't exist.
    bdev = rp2.Flash()
    try:
        fs = vfs.VfsFat(bdev)
    except:
        vfs.VfsFat.mkfs(bdev)
        fs = vfs.VfsFat(bdev)
    vfs.mount(fs, "/")


init()
del init
