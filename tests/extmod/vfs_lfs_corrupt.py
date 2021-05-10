# Test for VfsLittle using a RAM device, testing error handling from corrupt block device

try:
    import uos

    uos.VfsLfs1
    uos.VfsLfs2
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class RAMBlockDevice:
    ERASE_BLOCK_SIZE = 1024

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.ERASE_BLOCK_SIZE)
        self.ret = 0

    def readblocks(self, block, buf, off):
        addr = block * self.ERASE_BLOCK_SIZE + off
        for i in range(len(buf)):
            buf[i] = self.data[addr + i]
        return self.ret

    def writeblocks(self, block, buf, off):
        addr = block * self.ERASE_BLOCK_SIZE + off
        for i in range(len(buf)):
            self.data[addr + i] = buf[i]
        return self.ret

    def ioctl(self, op, arg):
        if op == 4:  # block count
            return len(self.data) // self.ERASE_BLOCK_SIZE
        if op == 5:  # block size
            return self.ERASE_BLOCK_SIZE
        if op == 6:  # erase block
            return 0


def corrupt(bdev, block):
    addr = block * bdev.ERASE_BLOCK_SIZE
    for i in range(bdev.ERASE_BLOCK_SIZE):
        bdev.data[addr + i] = i & 0xFF


def create_vfs(bdev, vfs_class):
    bdev.ret = 0
    vfs_class.mkfs(bdev)
    vfs = vfs_class(bdev)
    with vfs.open("f", "w") as f:
        for i in range(100):
            f.write("test")
    return vfs


def test(bdev, vfs_class):
    print("test", vfs_class)

    # statvfs
    vfs = create_vfs(bdev, vfs_class)
    corrupt(bdev, 0)
    corrupt(bdev, 1)
    try:
        print(vfs.statvfs(""))
    except OSError:
        print("statvfs OSError")

    # error during read
    vfs = create_vfs(bdev, vfs_class)
    f = vfs.open("f", "r")
    bdev.ret = -5  # EIO
    try:
        f.read(10)
    except OSError:
        print("read OSError")

    # error during write
    vfs = create_vfs(bdev, vfs_class)
    f = vfs.open("f", "a")
    bdev.ret = -5  # EIO
    try:
        f.write("test")
    except OSError:
        print("write OSError")

    # error during close
    vfs = create_vfs(bdev, vfs_class)
    f = vfs.open("f", "w")
    f.write("test")
    bdev.ret = -5  # EIO
    try:
        f.close()
    except OSError:
        print("close OSError")

    # error during flush
    vfs = create_vfs(bdev, vfs_class)
    f = vfs.open("f", "w")
    f.write("test")
    bdev.ret = -5  # EIO
    try:
        f.flush()
    except OSError:
        print("flush OSError")
    bdev.ret = 0
    f.close()


bdev = RAMBlockDevice(30)
test(bdev, uos.VfsLfs1)
test(bdev, uos.VfsLfs2)
