# Test for VfsFat using a RAM device, mtime feature

try:
    import utime, uos

    utime.time
    utime.sleep
    uos.VfsFat
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class RAMBlockDevice:
    ERASE_BLOCK_SIZE = 512

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.ERASE_BLOCK_SIZE)

    def readblocks(self, block, buf):
        addr = block * self.ERASE_BLOCK_SIZE
        for i in range(len(buf)):
            buf[i] = self.data[addr + i]

    def writeblocks(self, block, buf):
        addr = block * self.ERASE_BLOCK_SIZE
        for i in range(len(buf)):
            self.data[addr + i] = buf[i]

    def ioctl(self, op, arg):
        if op == 4:  # block count
            return len(self.data) // self.ERASE_BLOCK_SIZE
        if op == 5:  # block size
            return self.ERASE_BLOCK_SIZE


def test(bdev, vfs_class):
    print("test", vfs_class)

    # Initial format of block device.
    vfs_class.mkfs(bdev)

    # construction
    vfs = vfs_class(bdev)

    # Create an empty file, should have a timestamp.
    current_time = int(utime.time())
    vfs.open("test1", "wt").close()

    # Wait 2 seconds so mtime will increase (FAT has 2 second resolution).
    utime.sleep(2)

    # Create another empty file, should have a timestamp.
    vfs.open("test2", "wt").close()

    # Stat the files and check mtime is non-zero.
    stat1 = vfs.stat("test1")
    stat2 = vfs.stat("test2")
    print(stat1[8] != 0, stat2[8] != 0)

    # Check that test1 has mtime which matches time.time() at point of creation.
    # TODO this currently fails on the unix port because FAT stores timestamps
    # in localtime and stat() is UTC based.
    # print(current_time - 1 <= stat1[8] <= current_time + 1)

    # Check that test1 is older than test2.
    print(stat1[8] < stat2[8])

    # Unmount.
    vfs.umount()


bdev = RAMBlockDevice(50)
test(bdev, uos.VfsFat)
