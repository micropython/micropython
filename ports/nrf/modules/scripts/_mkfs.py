import os, nrf

try:
    from os import VfsLfs1

    os.VfsLfs1.mkfs(nrf.Flash())
except ImportError:
    try:
        from os import VfsLfs2

        os.VfsLfs2.mkfs(nrf.Flash())
    except ImportError:
        try:
            from os import VfsFat

            os.VfsFat.mkfs(nrf.Flash())
        except ImportError:
            pass
        except OSError as e:
            if e.args[0] == 5:  # I/O Error
                flashbdev_size = (nrf.Flash.ioctl(4, 0) * nrf.Flash.ioctl(5, 0)) // 1024
                print()
                print("Is `FS_SIZE=%iK` enough for FAT filesystem?" % flashbdev_size)
