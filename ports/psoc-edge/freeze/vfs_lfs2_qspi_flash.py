import os
import machine, psoc_edge 

bdev = psoc_edge.QSPI_Flash()
read_size = 0x1000  # sector size*
write_size = 0x100  # page size
try:
    vfs = os.VfsLfs2(bdev, progsize=read_size, readsize=write_size)
    os.mount(vfs, "/flash")
except:
    os.VfsLfs2.mkfs(bdev, progsize=read_size, readsize=write_size)
    vfs = os.VfsLfs2(bdev, progsize=read_size, readsize=write_size)
    os.mount(vfs, "/flash")

print("Virtual File System: mounted at '/flash' with LFS2 format in external QSPI flash.\n")

del machine, os, psoc_edge, bdev, vfs, read_size, write_size
