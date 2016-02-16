import sys
import uos
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

uos.VfsFat.mkfs(bdev)

assert b"FOO_FILETXT" not in bdev.data
assert b"hello!" not in bdev.data

vfs = uos.VfsFat(bdev, "/ramdisk")

f = vfs.open("foo_file.txt", "w")
f.write("hello!")
f.close()

f2 = vfs.open("foo_file.txt")
print(f2.read())
f2.close()

assert b"FOO_FILETXT" in bdev.data
assert b"hello!" in bdev.data
