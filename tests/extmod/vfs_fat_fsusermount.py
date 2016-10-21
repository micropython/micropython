import sys
import uos
import uerrno
try:
    uos.VfsFat
except AttributeError:
    print("SKIP")
    sys.exit()


class RAMFS:

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

    def ioctl(self, op, arg):
        #print("ioctl(%d, %r)" % (op, arg))
        if op == 4:  # BP_IOCTL_SEC_COUNT
            return len(self.data) // self.SEC_SIZE
        if op == 5:  # BP_IOCTL_SEC_SIZE
            return self.SEC_SIZE


try:
    bdev = RAMFS(48)
except MemoryError:
    print("SKIP")
    sys.exit()

# can't mkfs readonly device
try:
    uos.vfs_mkfs(bdev, "/ramdisk", readonly=True)
except OSError as e:
    print(e)

# mount before mkfs
try:
    uos.vfs_mount(bdev, "/ramdisk")
except OSError as e:
    print(e)

# invalid umount
try:
    uos.vfs_umount("/ramdisk")
except OSError as e:
    print(e.args[0] == uerrno.EINVAL)

try:
    uos.vfs_mount(None, "/ramdisk")
except OSError as e:
    print(e)

try:
    uos.vfs_mkfs(None, "/ramdisk")
except OSError as e:
    print(e)

# valid mkfs/mount
uos.vfs_mkfs(bdev, "/ramdisk")
uos.vfs_mount(bdev, "/ramdisk")

# umount by path
uos.vfs_umount("/ramdisk")

# readonly mount
uos.vfs_mount(bdev, "/ramdisk", readonly=True)
vfs = uos.VfsFat(bdev, "/ramdisk")
try:
    f = vfs.open("file.txt", "w")
except OSError as e:
    print("EROFS:", e.args[0] == 30) # uerrno.EROFS

# device is None == umount
uos.vfs_mount(None, "/ramdisk")

# max mounted devices
dev = []
try:
    for i in range(0,4):
        dev.append(RAMFS(48))
        uos.vfs_mkfs(dev[i], "/ramdisk" + str(i))
        uos.vfs_mount(dev[i], "/ramdisk" + str(i))
except OSError as e:
    print(e)
