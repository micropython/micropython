import sys, vfs

try:
    vfs.mount(vfs.VfsRom(vfs.rom_ioctl(2, 0)), "/rom")
    # Note: this won't execute boot.py/main.py because the cwd is still /flash.
    sys.path.insert(0, "/rom")
except:
    pass

del sys, vfs
