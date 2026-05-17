# Tests where the block device returns invalid values

try:
    import vfs

    vfs.VfsFat
    memoryview
except (NameError, ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class BadDev:
    SEC_SIZE = 512

    def __init__(self, blocks):
        self.blocks = blocks

    def readblocks(self, n, buf):
        assert len(buf) == self.SEC_SIZE
        buf[:] = bytearray(self.SEC_SIZE + 1)  # Attempts to enlarge passed-in buf

    def writeblocks(self, n, buf):
        pass

    def ioctl(self, op, arg):
        if op == 4:  # MP_BLOCKDEV_IOCTL_BLOCK_COUNT
            return self.blocks
        if op == 5:  # MP_BLOCKDEV_IOCTL_BLOCK_SIZE
            return self.SEC_SIZE


bdev = BadDev(512)
try:
    vfs.VfsFat.mkfs(bdev)
except ValueError as e:
    print("ValueError")
