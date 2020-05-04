# Test for VfsLittle using a RAM device

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

    # mkfs
    vfs_class.mkfs(bdev)

    # construction
    vfs = vfs_class(bdev)

    # statvfs
    print(vfs.statvfs("/"))

    # open, write close
    f = vfs.open("test", "w")
    f.write("littlefs")
    f.close()

    # statvfs after creating a file
    print(vfs.statvfs("/"))

    # ilistdir
    print(list(vfs.ilistdir()))
    print(list(vfs.ilistdir("/")))
    print(list(vfs.ilistdir(b"/")))

    # mkdir, rmdir
    vfs.mkdir("testdir")
    print(list(vfs.ilistdir()))
    print(sorted(list(vfs.ilistdir("testdir"))))
    vfs.rmdir("testdir")
    print(list(vfs.ilistdir()))
    vfs.mkdir("testdir")

    # stat a file
    print(vfs.stat("test"))

    # stat a dir (size seems to vary on LFS2 so don't print that)
    print(vfs.stat("testdir")[:6])

    # read
    with vfs.open("test", "r") as f:
        print(f.read())

    # create large file
    with vfs.open("testbig", "w") as f:
        data = "large012" * 32 * 16
        print("data length:", len(data))
        for i in range(4):
            print("write", i)
            f.write(data)

    # stat after creating large file
    print(vfs.statvfs("/"))

    # rename
    vfs.rename("testbig", "testbig2")
    print(sorted(list(vfs.ilistdir())))
    vfs.chdir("testdir")
    vfs.rename("/testbig2", "testbig2")
    print(sorted(list(vfs.ilistdir())))
    vfs.rename("testbig2", "/testbig2")
    vfs.chdir("/")
    print(sorted(list(vfs.ilistdir())))

    # remove
    vfs.remove("testbig2")
    print(sorted(list(vfs.ilistdir())))

    # getcwd, chdir
    vfs.mkdir("/testdir2")
    vfs.mkdir("/testdir/subdir")
    print(vfs.getcwd())
    vfs.chdir("/testdir")
    print(vfs.getcwd())

    # create file in directory to make sure paths are relative
    vfs.open("test2", "w").close()
    print(vfs.stat("test2"))
    print(vfs.stat("/testdir/test2"))
    vfs.remove("test2")

    # chdir back to root and remove testdir
    vfs.chdir("/")
    print(vfs.getcwd())
    vfs.chdir("testdir")
    print(vfs.getcwd())
    vfs.chdir("..")
    print(vfs.getcwd())
    vfs.chdir("testdir/subdir")
    print(vfs.getcwd())
    vfs.chdir("../..")
    print(vfs.getcwd())
    vfs.chdir("/./testdir2")
    print(vfs.getcwd())
    vfs.chdir("../testdir")
    print(vfs.getcwd())
    vfs.chdir("../..")
    print(vfs.getcwd())
    vfs.chdir(".//testdir")
    print(vfs.getcwd())
    vfs.chdir("subdir/./")
    print(vfs.getcwd())
    vfs.chdir("/")
    print(vfs.getcwd())
    vfs.rmdir("testdir/subdir")
    vfs.rmdir("testdir")
    vfs.rmdir("testdir2")


bdev = RAMBlockDevice(30)
test(bdev, uos.VfsLfs1)
test(bdev, uos.VfsLfs2)
