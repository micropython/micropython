import os
import machine, psoc6


# Try to mount the filesystem, and format the flash if it doesn't exist.
bdev = psoc6.QSPI_Flash()

try:
    vfs = os.VfsFat(bdev)
    os.mount(vfs, "/flash")
except:
    os.VfsFat.mkfs(bdev)
    vfs = os.VfsFat(bdev)
    os.mount(vfs, "/flash")

print("Virtual File System: mounted at '/flash' with FAT format in external QSPI flash.\n")

del machine, os, psoc6, bdev, vfs
