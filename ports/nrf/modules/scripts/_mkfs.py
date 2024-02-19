import vfs, nrf

try:
    from vfs import VfsLfs1

    vfs.VfsLfs1.mkfs(nrf.Flash())
except ImportError:
    try:
        from vfs import VfsLfs2

        vfs.VfsLfs2.mkfs(nrf.Flash())
    except ImportError:
        try:
            from vfs import VfsFat

            vfs.VfsFat.mkfs(nrf.Flash())
        except ImportError:
            pass
        except OSError as e:
            if e.args[0] == 5:  # I/O Error
                flashbdev_size = (nrf.Flash.ioctl(4, 0) * nrf.Flash.ioctl(5, 0)) // 1024
                print()
                print("Is `FS_SIZE=%iK` enough for FAT filesystem?" % flashbdev_size)
