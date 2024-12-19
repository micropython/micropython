import vfs, sys, pyb


vfs.mount(vfs.VfsRom(vfs.rom_ioctl(1)), "/rom")
# this won't execute boot.py/main.py because the cwd is still /flash
sys.path.insert(0, "/rom")

del vfs, sys, pyb
