# Test for VfsLittle using a RAM device, with mount/umount

try:
    import os, vfs

    vfs.VfsLfs1
    vfs.VfsLfs2
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class RAMBlockDevice:
    ERASE_BLOCK_SIZE = 1024

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


def test(vfs_class):
    print("test", vfs_class)

    bdev = RAMBlockDevice(30)

    # mount bdev unformatted
    try:
        vfs.mount(bdev, "/lfs")
    except Exception as er:
        print(repr(er))

    # mkfs
    vfs_class.mkfs(bdev)

    # construction
    fs = vfs_class(bdev)

    # mount
    vfs.mount(fs, "/lfs")

    # import
    with open("/lfs/lfsmod.py", "w") as f:
        f.write('print("hello from lfs")\n')
    import lfsmod

    # import package
    os.mkdir("/lfs/lfspkg")
    with open("/lfs/lfspkg/__init__.py", "w") as f:
        f.write('print("package")\n')
    import lfspkg

    # chdir and import module from current directory (needs "" in sys.path)
    os.mkdir("/lfs/subdir")
    os.chdir("/lfs/subdir")
    os.rename("/lfs/lfsmod.py", "/lfs/subdir/lfsmod2.py")
    import lfsmod2

    # umount
    vfs.umount("/lfs")

    # mount read-only
    fs = vfs_class(bdev)
    vfs.mount(fs, "/lfs", readonly=True)

    # test reading works
    with open("/lfs/subdir/lfsmod2.py") as f:
        print("lfsmod2.py:", f.read())

    # test writing fails
    try:
        open("/lfs/test_write", "w")
    except OSError as er:
        print(repr(er))

    # umount
    vfs.umount("/lfs")

    # mount bdev again
    vfs.mount(bdev, "/lfs")

    # umount
    vfs.umount("/lfs")

    # clear imported modules
    sys.modules.clear()


# initialise path
import sys

sys.path.clear()
sys.path.append("/lfs")
sys.path.append("")

# run tests
test(vfs.VfsLfs1)
test(vfs.VfsLfs2)
