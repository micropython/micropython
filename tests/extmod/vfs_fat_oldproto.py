try:
    import errno, os, vfs

    vfs.VfsFat
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class RAMFS_OLD:
    SEC_SIZE = 512

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.SEC_SIZE)

    def readblocks(self, n, buf):
        # print("readblocks(%s, %x(%d))" % (n, id(buf), len(buf)))
        for i in range(len(buf)):
            buf[i] = self.data[n * self.SEC_SIZE + i]

    def writeblocks(self, n, buf):
        # print("writeblocks(%s, %x)" % (n, id(buf)))
        for i in range(len(buf)):
            self.data[n * self.SEC_SIZE + i] = buf[i]

    def sync(self):
        pass

    def count(self):
        return len(self.data) // self.SEC_SIZE


try:
    bdev = RAMFS_OLD(50)
except MemoryError:
    print("SKIP")
    raise SystemExit

vfs.VfsFat.mkfs(bdev)
fs = vfs.VfsFat(bdev)
vfs.mount(fs, "/ramdisk")

# file io
with fs.open("file.txt", "w") as f:
    f.write("hello!")

print(list(fs.ilistdir()))

with fs.open("file.txt", "r") as f:
    print(f.read())

fs.remove("file.txt")
print(list(fs.ilistdir()))
