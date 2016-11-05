import sys
import uos
import uerrno
try:
    uos.VfsFat
    uos.vfs_mkfs
    uos.vfs_mount
except AttributeError:
    print("SKIP")
    sys.exit()

class RAMFS_OLD:

    SEC_SIZE = 512

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.SEC_SIZE)

    def readblocks(self, n, buf):
        #print("readblocks(%s, %x(%d))" % (n, id(buf), len(buf)))
        for i in range(len(buf)):
            buf[i] = self.data[n * self.SEC_SIZE + i]

    def writeblocks(self, n, buf):
        #print("writeblocks(%s, %x)" % (n, id(buf)))
        for i in range(len(buf)):
            self.data[n * self.SEC_SIZE + i] = buf[i]

    def sync(self):
        pass

    def count(self):
        return len(self.data) // self.SEC_SIZE


try:
    bdev = RAMFS_OLD(48)
except MemoryError:
    print("SKIP")
    sys.exit()

uos.vfs_mkfs(bdev, "/ramdisk")
uos.vfs_mount(bdev, "/ramdisk")

# file io
vfs = uos.VfsFat(bdev, "/ramdisk")
with vfs.open("file.txt", "w") as f:
    f.write("hello!")

print(vfs.listdir())

with vfs.open("file.txt", "r") as f:
    print(f.read())

vfs.remove("file.txt")
print(vfs.listdir())

# umount by device
uos.vfs_umount(bdev)
try:
    vfs.listdir()
except OSError as e:
    print(e.args[0] == uerrno.ENODEV)
