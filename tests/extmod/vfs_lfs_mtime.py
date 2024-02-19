# Test for VfsLfs using a RAM device, mtime feature

try:
    import time, vfs

    time.time
    time.sleep
    vfs.VfsLfs2
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
    fs = vfs_class(bdev, mtime=True)

    # Create an empty file, should have a timestamp.
    current_time = int(time.time())
    fs.open("test1", "wt").close()

    # Wait 1 second so mtime will increase by at least 1.
    time.sleep(1)

    # Create another empty file, should have a timestamp.
    fs.open("test2", "wt").close()

    # Stat the files and check mtime is non-zero.
    stat1 = fs.stat("test1")
    stat2 = fs.stat("test2")
    print(stat1[8] != 0, stat2[8] != 0)

    # Check that test1 has mtime which matches time.time() at point of creation.
    print(current_time <= stat1[8] <= current_time + 1)

    # Check that test1 is older than test2.
    print(stat1[8] < stat2[8])

    # Wait 1 second so mtime will increase by at least 1.
    time.sleep(1)

    # Open test1 for reading and ensure mtime did not change.
    fs.open("test1", "rt").close()
    print(fs.stat("test1") == stat1)

    # Open test1 for writing and ensure mtime increased from the previous value.
    fs.open("test1", "wt").close()
    stat1_old = stat1
    stat1 = fs.stat("test1")
    print(stat1_old[8] < stat1[8])

    # Unmount.
    fs.umount()

    # Check that remounting with mtime=False can read the timestamps.
    print("mtime=False")
    fs = vfs_class(bdev, mtime=False)
    print(fs.stat("test1") == stat1)
    print(fs.stat("test2") == stat2)
    f = fs.open("test1", "wt")
    f.close()
    print(fs.stat("test1") == stat1)
    fs.umount()

    # Check that remounting with mtime=True still has the timestamps.
    print("mtime=True")
    fs = vfs_class(bdev, mtime=True)
    print(fs.stat("test1") == stat1)
    print(fs.stat("test2") == stat2)
    fs.umount()


try:
    bdev = RAMBlockDevice(30)
except MemoryError:
    print("SKIP")
    raise SystemExit

test(bdev, vfs.VfsLfs2)
