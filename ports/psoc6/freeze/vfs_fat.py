import os
import machine, psoc6


# Try to mount the filesystem, and format the flash if it doesn't exist.
# Create block device instance from external or internal flash, whichever is enabled
bdev = psoc6.QSPI_Flash() if "QSPI_Flash" in dir(psoc6) else psoc6.Flash()

try:
    vfs = os.VfsFat(bdev)
    os.mount(vfs, "/")
except:
    os.VfsFat.mkfs(bdev)
    vfs = os.VfsFat(bdev)
    os.mount(vfs, "/")

print("FAT filesystem mounted at /\n")

del machine, os, psoc6, bdev, vfs
