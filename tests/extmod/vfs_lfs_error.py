# Test for VfsLittle using a RAM device, testing error handling

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
    except OSError:
        print("mkfs OSError")

    # mount with invalid filesystem
    try:
        vfs_class(bdev)
    except OSError:
        print("mount OSError")

    # set up for following tests
    vfs_class.mkfs(bdev)
    vfs = vfs_class(bdev)
    with vfs.open("testfile", "w") as f:
        f.write("test")
    vfs.mkdir("testdir")

    # ilistdir
    try:
        vfs.ilistdir("noexist")
    except OSError:
        print("ilistdir OSError")

    # remove
    try:
        vfs.remove("noexist")
    except OSError:
        print("remove OSError")

    # rmdir
    try:
        vfs.rmdir("noexist")
    except OSError:
        print("rmdir OSError")

    # rename
    try:
        vfs.rename("noexist", "somethingelse")
    except OSError:
        print("rename OSError")

    # mkdir
    try:
        vfs.mkdir("testdir")
    except OSError:
        print("mkdir OSError")

    # chdir to nonexistent
    try:
        vfs.chdir("noexist")
    except OSError:
        print("chdir OSError")
    print(vfs.getcwd())  # check still at root

    # chdir to file
    try:
        vfs.chdir("testfile")
    except OSError:
        print("chdir OSError")
    print(vfs.getcwd())  # check still at root

    # stat
    try:
        vfs.stat("noexist")
    except OSError:
        print("stat OSError")

    # error during seek
    with vfs.open("testfile", "r") as f:
        f.seek(1 << 30)  # SEEK_SET
        try:
            f.seek(1 << 30, 1)  # SEEK_CUR
        except OSError:
            print("seek OSError")


bdev = RAMBlockDevice(30)
test(bdev, uos.VfsLfs1)
test(bdev, uos.VfsLfs2)
