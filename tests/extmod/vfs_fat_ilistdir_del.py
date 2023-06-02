# Test ilistdir __del__ for VfsFat using a RAM device.
import gc

try:
    import uos

    uos.VfsFat
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class RAMBlockDevice:
    ERASE_BLOCK_SIZE = 512

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.ERASE_BLOCK_SIZE)

    def readblocks(self, block, buf, off=0):
        addr = block * self.ERASE_BLOCK_SIZE + off
        for i in range(len(buf)):
            buf[i] = self.data[addr + i]

    def writeblocks(self, block, buf, off=0):
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
    vfs_class.mkfs(bdev)
    vfs = vfs_class(bdev)
    vfs.mkdir("/test_d1")
    vfs.mkdir("/test_d2")
    vfs.mkdir("/test_d3")

    for i in range(10):
        print(i)

        # We want to partially iterate the ilistdir iterator to leave it in an
        # open state, which will then test the finaliser when it's garbage collected.
        idir = vfs.ilistdir("/")
        print(any(idir))

        # Alternate way of partially iterating the ilistdir object, modifying the
        # filesystem while it's open.
        for dname, *_ in vfs.ilistdir("/"):
            vfs.rmdir(dname)
            break
        vfs.mkdir(dname)

        # Also create a fully drained iterator and ensure trying to re-use it
        # throws the correct exception.
        idir_emptied = vfs.ilistdir("/")
        l = list(idir_emptied)
        print(len(l))
        try:
            next(idir_emptied)
        except StopIteration:
            pass

        gc.collect()
        vfs.open("/test", "w").close()


try:
    bdev = RAMBlockDevice(50)
except MemoryError:
    print("SKIP")
    raise SystemExit

test(bdev, uos.VfsFat)
