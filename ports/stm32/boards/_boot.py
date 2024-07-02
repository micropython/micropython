import vfs, sys, pyb


mapfs = pyb.Flash("mapfs")
vfs.mount(vfs.VfsMap(mapfs.ioctl(0x100, 0), mapfs), "/mapfs")
sys.path.insert(0, "/mapfs")

del vfs, sys, pyb, mapfs
