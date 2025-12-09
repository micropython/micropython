# Tests where the block device returns invalid values

try:
    import vfs

    vfs.VfsFat
    vfs.VfsLfs2
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class RAMBlockDevice:
    ERASE_BLOCK_SIZE = 512

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.ERASE_BLOCK_SIZE)
        self.read_res = 0
        self.write_res = 0

    def readblocks(self, block, buf, off=0):
        # print("readblocks", block, len(buf), off)
        addr = block * self.ERASE_BLOCK_SIZE + off
        for i in range(len(buf)):
            buf[i] = self.data[addr + i]
        return self.read_res

    def writeblocks(self, block, buf, off=None):
        if off is None:
            # erase, then write
            off = 0
        addr = block * self.ERASE_BLOCK_SIZE + off
        for i in range(len(buf)):
            self.data[addr + i] = buf[i]
        return self.write_res

    def ioctl(self, op, arg):
        if op == 4:  # block count
            return len(self.data) // self.ERASE_BLOCK_SIZE
        if op == 5:  # block size
            return self.ERASE_BLOCK_SIZE
        if op == 6:  # erase block
            return 0


try:
    bdev = RAMBlockDevice(50)
except MemoryError:
    print("SKIP")
    raise SystemExit


def test(vfs_class):
    print(vfs_class)
    bdev.read_res = 0  # reset function results
    bdev.write_res = 0

    vfs_class.mkfs(bdev)
    fs = vfs_class(bdev)

    with fs.open("test", "w") as f:
        f.write("a" * 64)

    for res in (0, -5, 5, 33, "invalid"):
        # -5 is a legitimate negative failure (EIO), positive integer
        # is not

        # This variant will fail on open
        bdev.read_res = res
        try:
            with fs.open("test", "r") as f:
                print("opened")
        except Exception as e:
            print(type(e), e)

        # This variant should succeed on open, may fail on read
        # unless the filesystem cached the contents already
        bdev.read_res = 0
        try:
            with fs.open("test", "r") as f:
                bdev.read_res = res
                print("read 1", f.read(1))
                print("read rest", f.read())
        except Exception as e:
            print(type(e), e)


test(vfs.VfsLfs2)
test(vfs.VfsFat)
