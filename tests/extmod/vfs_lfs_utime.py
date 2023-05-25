# Test for VfsLfs using a RAM device, mtime feature

try:
    import utime, uos

    utime.time
    utime.sleep
    uos.VfsLfs2
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class RAMBlockDevice:
    ERASE_BLOCK_SIZE = 1024

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.ERASE_BLOCK_SIZE)

    def readblocks(self, block, buf, off):
        addr = block * self.ERASE_BLOCK_SIZE + off
        for i in range(len(buf)):
            buf[i] = self.data[addr + i]

    def writeblocks(self, block, buf, off):
        addr = block * self.ERASE_BLOCK_SIZE + off
        for i in range(len(buf)):
            self.data[addr + i] = buf[i]

    def ioctl(self, op, arg):
        if op == 4:  # block count
            return len(self.data) // self.ERASE_BLOCK_SIZE
        if op == 5:  # block size
            return self.ERASE_BLOCK_SIZE
        if op == 6:  # erase block
            return 0


def test(bdev, vfs_class):
    print("test", vfs_class)

    # Initial format of block device.
    vfs_class.mkfs(bdev)

    # construction
    print("mtime=True")
    vfs = vfs_class(bdev, mtime=True)

    # Create an empty file, should have a timestamp.
    vfs.open("test1", "wt").close()

    # Stat the file before and after setting mtime with os.utime().
    stat1 = vfs.stat("test1")
    tm = utime.mktime((2010, 1, 1, 11, 49, 1, 0, 0))
    vfs.utime("test1", (tm, tm))
    stat2 = vfs.stat("test1")
    print(stat1[8] != 0, stat2[8] != stat1[8], stat2[8] == tm)

    # Check for invalid arguments: should all raise OSError.
    try:
        vfs.utime("test1", None)
    except OSError:
        print("OSError")
    try:
        vfs.utime("test1", (None, None))
    except TypeError:
        print("TypeError")
    try:
        vfs.utime("test1", (None, "Invalid"))
    except TypeError:
        print("TypeError")

    # Unmount.
    vfs.umount()

    # Check that remounting with mtime=False raises errors on utime().
    print("mtime=False")
    vfs = vfs_class(bdev, mtime=False)
    print(vfs.stat("test1") == stat2)

    # Should fail with OSError(EPERM)
    try:
        vfs.utime("test1", (2000, 2000))
    except OSError:
        print("OSError")

    print(vfs.stat("test1") == stat2)
    vfs.umount()


bdev = RAMBlockDevice(30)
test(bdev, uos.VfsLfs2)
