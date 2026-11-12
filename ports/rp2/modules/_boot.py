def init():
    import rp2
    import vfs

    # The flash requires the programming size to be aligned to 256 bytes.
    progsize = 256

    # Try to mount the filesystem, and format the flash if it doesn't exist.
    bdev = rp2.Flash()
    try:
        fs = vfs.VfsLfs2(bdev, progsize=progsize)
    except:
        vfs.VfsLfs2.mkfs(bdev, progsize=progsize)
        fs = vfs.VfsLfs2(bdev, progsize=progsize)
    vfs.mount(fs, "/")


init()
del init
