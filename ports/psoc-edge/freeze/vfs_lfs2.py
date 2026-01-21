import os
import machine, psoc_edge as psocedge

# Create block device instance from external flash
bdev = psocedge.QSPI_Flash()

# sector size 4 KB for external flash
read_size = 0x1000
# page size 256 B
write_size = 0x100

try:
    vfs = os.VfsLfs2(bdev, progsize=write_size, readsize=read_size)
    os.mount(vfs, "/")
except:
    # Format flash if filesystem not found
    os.VfsLfs2.mkfs(bdev, progsize=write_size, readsize=read_size)
    vfs = os.VfsLfs2(bdev, progsize=write_size, readsize=read_size)
    os.mount(vfs, "/")

print("Virtual File System: mounted at '/' with LFS2 format in external flash.\n")

del machine, os, psocedge, bdev, vfs, read_size, write_size
