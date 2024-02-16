import os
import machine, psoc6

bdev = psoc6.QSPI_Flash()
read_size = 0x1000  # sector size*
write_size = 0x200  # page size
try:
    vfs = os.VfsLfs2(bdev, progsize=read_size, readsize=write_size)
    os.mount(vfs, "/flash")
except:
    os.VfsLfs2.mkfs(bdev, progsize=read_size, readsize=write_size)
    vfs = os.VfsLfs2(bdev, progsize=read_size, readsize=write_size)
    os.mount(vfs, "/flash")

print("External LFS2 filesystem mounted at /flash\n")

del machine, os, psoc6, bdev, vfs, read_size, write_size
