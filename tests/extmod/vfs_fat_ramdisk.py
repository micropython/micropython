try:
    import errno, os, vfs

    vfs.VfsFat
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class RAMFS:
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

    def ioctl(self, op, arg):
        # print("ioctl(%d, %r)" % (op, arg))
        if op == 4:  # MP_BLOCKDEV_IOCTL_BLOCK_COUNT
            return len(self.data) // self.SEC_SIZE
        if op == 5:  # MP_BLOCKDEV_IOCTL_BLOCK_SIZE
            return self.SEC_SIZE


try:
    bdev = RAMFS(50)
    vfs.VfsFat.mkfs(bdev)
except MemoryError:
    print("SKIP")
    raise SystemExit

print(b"FOO_FILETXT" not in bdev.data)
print(b"hello!" not in bdev.data)

fs = vfs.VfsFat(bdev)
vfs.mount(fs, "/ramdisk")

print("statvfs:", fs.statvfs("/ramdisk"))
print("getcwd:", fs.getcwd())

try:
    fs.stat("no_file.txt")
except OSError as e:
    print(e.errno == errno.ENOENT)

with fs.open("foo_file.txt", "w") as f:
    f.write("hello!")
print(list(fs.ilistdir()))

print("stat root:", fs.stat("/")[:-3])  # timestamps differ across runs
print("stat file:", fs.stat("foo_file.txt")[:-3])  # timestamps differ across runs

print(b"FOO_FILETXT" in bdev.data)
print(b"hello!" in bdev.data)

fs.mkdir("foo_dir")
fs.chdir("foo_dir")
print("getcwd:", fs.getcwd())
print(list(fs.ilistdir()))

with fs.open("sub_file.txt", "w") as f:
    f.write("subdir file")

try:
    fs.chdir("sub_file.txt")
except OSError as e:
    print(e.errno == errno.ENOENT)

fs.chdir("..")
print("getcwd:", fs.getcwd())

vfs.umount(fs)

fs = vfs.VfsFat(bdev)
print(list(fs.ilistdir(b"")))

# list a non-existent directory
try:
    fs.ilistdir(b"no_exist")
except OSError as e:
    print("ENOENT:", e.errno == errno.ENOENT)
