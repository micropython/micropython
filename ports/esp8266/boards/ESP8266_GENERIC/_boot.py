# Minimal _boot.py for the 512kiB variant. Does not set up a block device or
# filesystem. Other variants use esp8266/modules/_boot.py.

import gc, vfs

gc.threshold((gc.mem_free() + gc.mem_alloc()) // 4)

# Try to mount the ROMFS.
try:
    vfs.mount(vfs.VfsRom(vfs.rom_ioctl(2)), "/")
except:
    pass

