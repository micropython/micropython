import os
import machine, psoc6


# Try to mount the filesystem, and format the flash if it doesn't exist.
# TODO: Check "Note: the internal flash requires the programming size to be aligned to 256 bytes." and progSize parameter below !!

# Create block device instance from external or internal flash, whichever is enabled
bdev = psoc6.QSPI_Flash() if "QSPI_Flash" in dir(psoc6) else psoc6.Flash()

# sector size 4 KB for external flash
# sector size 512 B for internal flash
read_size = 0x1000 if "QSPI_Flash" in dir(psoc6) else 0x200
# page size 512 B for both flashes
write_size = 0x200

try:
    vfs = os.VfsLfs2(bdev, progsize=write_size, readsize=read_size)
    os.mount(vfs, "/")
except:
    os.VfsLfs2.mkfs(bdev, progsize=write_size, readsize=read_size)
    vfs = os.VfsLfs2(bdev, progsize=write_size, readsize=read_size)
    os.mount(vfs, "/")

print("Internal LFS2 filesystem mounted at /\n")

del machine, os, psoc6, bdev, vfs, read_size, write_size
