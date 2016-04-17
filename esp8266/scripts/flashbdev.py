import esp

class FlashBdev:

    SEC_SIZE = 4096
    START_SEC = 0x89000 // SEC_SIZE
    NUM_BLK = 0x73

    def __init__(self, blocks=NUM_BLK):
        self.blocks = blocks

    def readblocks(self, n, buf):
        print("readblocks(%s, %x(%d))" % (n, id(buf), len(buf)))
        esp.flash_read((n + self.START_SEC) * self.SEC_SIZE, buf)

    def writeblocks(self, n, buf):
        print("writeblocks(%s, %x(%d))" % (n, id(buf), len(buf)))
        assert len(buf) <= self.SEC_SIZE, len(buf)
        esp.flash_erase(n + self.START_SEC)
        esp.flash_write((n + self.START_SEC) * self.SEC_SIZE, buf)

    def ioctl(self, op, arg):
        print("ioctl(%d, %r)" % (op, arg))
        if op == 4:  # BP_IOCTL_SEC_COUNT
            return self.blocks
        if op == 5:  # BP_IOCTL_SEC_SIZE
            return self.SEC_SIZE

bdev = FlashBdev()
