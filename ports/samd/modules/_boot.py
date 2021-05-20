import gc

print("Free memory:",gc.mem_free()," Allocated Memory:", gc.mem_alloc())
#gc.threshold((gc.mem_free() + gc.mem_alloc()) // 4)
import uos
import samd

samd.Flash.flash_init()
bdev = samd.Flash()

# Try to mount the filesystem, and format the flash if it doesn't exist.
try:
    vfs = uos.VfsLfs1(bdev)
except:
    uos.VfsLfs1.mkfs(bdev)
    vfs = uos.VfsLfs1(bdev)
uos.mount(vfs, "/")
gc.collect()
print("Free memory:",gc.mem_free())

del uos, bdev, vfs, samd, gc
