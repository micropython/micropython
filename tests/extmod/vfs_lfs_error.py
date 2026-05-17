# Test for VfsLittle using a RAM device, testing error handling

try:
    import errno, vfs

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

    # mkfs with too-small block device
    try:
        vfs_class.mkfs(RAMBlockDevice(1))
    except OSError as er:
        print("mkfs OSError", er.errno > 0)

    # mount with invalid filesystem
    try:
        vfs_class(bdev)
    except OSError as er:
        print("mount OSError", er.errno > 0)

    # set up for following tests
    vfs_class.mkfs(bdev)
    fs = vfs_class(bdev)
    with fs.open("testfile", "w") as f:
        f.write("test")
    fs.mkdir("testdir")

    # ilistdir
    try:
        fs.ilistdir("noexist")
    except OSError as er:
        print("ilistdir OSError", er)

    # remove
    try:
        fs.remove("noexist")
    except OSError as er:
        print("remove OSError", er)

    # rmdir
    try:
        fs.rmdir("noexist")
    except OSError as er:
        print("rmdir OSError", er)

    # rename
    try:
        fs.rename("noexist", "somethingelse")
    except OSError as er:
        print("rename OSError", er)

    # mkdir
    try:
        fs.mkdir("testdir")
    except OSError as er:
        print("mkdir OSError", er)

    # chdir to nonexistent
    try:
        fs.chdir("noexist")
    except OSError as er:
        print("chdir OSError", er)
    print(fs.getcwd())  # check still at root

    # chdir to file
    try:
        fs.chdir("testfile")
    except OSError as er:
        print("chdir OSError", er)
    print(fs.getcwd())  # check still at root

    # stat
    try:
        fs.stat("noexist")
    except OSError as er:
        print("stat OSError", er)

    # error during seek
    with fs.open("testfile", "r") as f:
        f.seek(1 << 30)  # SEEK_SET
        try:
            f.seek(1 << 30, 1)  # SEEK_CUR
        except OSError as er:
            print("seek OSError", er)


bdev = RAMBlockDevice(30)
test(bdev, vfs.VfsLfs1)
test(bdev, vfs.VfsLfs2)
