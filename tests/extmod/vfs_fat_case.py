try:
    import uerrno
    import uos
except ImportError:
    print("missing u")
    print("SKIP")
    raise SystemExit

try:
    uos.VfsFat
except AttributeError:
    print("missing VfsFat")
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
        return 0

    def writeblocks(self, n, buf):
        # print("writeblocks(%s, %x)" % (n, id(buf)))
        for i in range(len(buf)):
            self.data[n * self.SEC_SIZE + i] = buf[i]
        return 0

    def ioctl(self, op, arg):
        # print("ioctl(%d, %r)" % (op, arg))
        if op == 4:  # MP_BLOCKDEV_IOCTL_BLOCK_COUNT
            return len(self.data) // self.SEC_SIZE
        if op == 5:  # MP_BLOCKDEV_IOCTL_BLOCK_SIZE
            return self.SEC_SIZE


try:
    bdev = RAMFS(50)
except MemoryError:
    print("SKIP")
    raise SystemExit

uos.VfsFat.mkfs(bdev)
vfs = uos.VfsFat(bdev)
uos.mount(vfs, "/ramdisk")
uos.chdir("/ramdisk")

vfs.label = "labelæ"
# This label would normally be LABELÆ but our limited upper casing does "LABELæ"
print(vfs.label)

# Check ASCII case-insensitivity
vfs.mkdir("fooaz")
print(uos.listdir(""))
vfs.rmdir("fOOAZ")

# Check ASCII case-insensitivity for long names (8+ characters)
vfs.mkdir("123456789fooaz")
print(uos.listdir(""))
vfs.rmdir("123456789fOOAZ")

# Characters outside of a-z are case sensitive.
vfs.mkdir("extended_æ")
print(uos.listdir(""))
# Normally this would work ok. With our limited uppercasing, it won't.
try:
    vfs.rmdir("extended_Æ")
except OSError as e:
    print(e.errno == uerrno.ENOENT)

vfs.rmdir("extended_æ")

# Emoji test for fun.
vfs.mkdir("emoji_😀")
vfs.rmdir("emoji_😀")
