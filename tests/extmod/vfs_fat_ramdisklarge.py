# test making a FAT filesystem on a very large block device

try:
    import uos
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    uos.VfsFat
except AttributeError:
    print("SKIP")
    raise SystemExit


class RAMBDevSparse:

    SEC_SIZE = 512

    def __init__(self, blocks):
        self.blocks = blocks
        self.data = {}

    def readblocks(self, n, buf):
        #print("readblocks(%s, %x(%d))" % (n, id(buf), len(buf)))
        assert len(buf) == self.SEC_SIZE
        if n not in self.data:
            self.data[n] = bytearray(self.SEC_SIZE)
        buf[:] = self.data[n]

    def writeblocks(self, n, buf):
        #print("writeblocks(%s, %x)" % (n, id(buf)))
        mv = memoryview(buf)
        for off in range(0, len(buf), self.SEC_SIZE):
            s = n + off // self.SEC_SIZE
            if s not in self.data:
                self.data[s] = bytearray(self.SEC_SIZE)
            self.data[s][:] = mv[off:off + self.SEC_SIZE]

    def ioctl(self, op, arg):
        #print("ioctl(%d, %r)" % (op, arg))
        if op == 4:  # MP_BLOCKDEV_IOCTL_BLOCK_COUNT
            return self.blocks
        if op == 5:  # MP_BLOCKDEV_IOCTL_BLOCK_SIZE
            return self.SEC_SIZE


try:
    bdev = RAMBDevSparse(4 * 1024 * 1024 * 1024 // RAMBDevSparse.SEC_SIZE)
    uos.VfsFat.mkfs(bdev)
except MemoryError:
    print("SKIP")
    raise SystemExit

vfs = uos.VfsFat(bdev)
uos.mount(vfs, "/ramdisk")

print("statvfs:", vfs.statvfs("/ramdisk"))

f = open('/ramdisk/test.txt', 'w')
f.write('test file')
f.close()

print("statvfs:", vfs.statvfs("/ramdisk"))

f = open('/ramdisk/test.txt')
print(f.read())
f.close()

uos.umount(vfs)
