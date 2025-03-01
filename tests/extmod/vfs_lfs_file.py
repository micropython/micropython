# Test for VfsLittle using a RAM device, file IO

try:
    import vfs

    vfs.VfsLfs1
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

    # mkfs
    vfs_class.mkfs(bdev)

    # construction
    fs = vfs_class(bdev)

    # create text, print, write, close
    f = fs.open("test.txt", "wt")
    print(f)
    f.write("littlefs")
    f.close()

    # close already-closed file
    f.close()

    # create binary, print, write, flush, close
    f = fs.open("test.bin", "wb")
    print(f)
    f.write("littlefs")
    f.flush()
    f.close()

    # create for append
    f = fs.open("test.bin", "ab")
    f.write("more")
    f.close()

    # create exclusive
    f = fs.open("test2.bin", "xb")
    f.close()

    # create exclusive with error
    try:
        fs.open("test2.bin", "x")
    except OSError:
        print("open OSError")

    # read default
    with fs.open("test.txt", "") as f:
        print(f.read())

    # read text
    with fs.open("test.txt", "rt") as f:
        print(f.read())

    # read binary
    with fs.open("test.bin", "rb") as f:
        print(f.read())

    # create read and write
    with fs.open("test.bin", "r+b") as f:
        print(f.read(8))
        f.write("MORE")
    with fs.open("test.bin", "rb") as f:
        print(f.read())

    # seek and tell
    f = fs.open("test.txt", "r")
    print(f.tell())
    f.seek(3, 0)
    print(f.tell())
    f.close()

    # open nonexistent
    try:
        fs.open("noexist", "r")
    except OSError:
        print("open OSError")

    # open multiple files at the same time
    f1 = fs.open("test.txt", "")
    f2 = fs.open("test.bin", "b")
    print(f1.read())
    print(f2.read())
    f1.close()
    f2.close()


bdev = RAMBlockDevice(30)
test(bdev, vfs.VfsLfs1)
test(bdev, vfs.VfsLfs2)
