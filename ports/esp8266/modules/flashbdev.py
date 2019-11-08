import esp

class FlashBdev:

    SEC_SIZE = 4096
    RESERVED_SECS = 1
    START_SEC = esp.flash_user_start() // SEC_SIZE + RESERVED_SECS
    NUM_BLK = 0x6b - RESERVED_SECS

    def __init__(self, blocks=NUM_BLK):
        self.blocks = blocks

    def readblocks(self, n, buf, off=0):
        #print("readblocks(%s, %x(%d), %d)" % (n, id(buf), len(buf), off))
        esp.flash_read((n + self.START_SEC) * self.SEC_SIZE + off, buf)

    def writeblocks(self, n, buf, off=None):
        #print("writeblocks(%s, %x(%d), %d)" % (n, id(buf), len(buf), off))
        #assert len(buf) <= self.SEC_SIZE, len(buf)
        if off is None:
            esp.flash_erase(n + self.START_SEC)
            off = 0
        esp.flash_write((n + self.START_SEC) * self.SEC_SIZE + off, buf)

    def ioctl(self, op, arg):
        #print("ioctl(%d, %r)" % (op, arg))
        if op == 4:  # MP_BLOCKDEV_IOCTL_BLOCK_COUNT
            return self.blocks
        if op == 5:  # MP_BLOCKDEV_IOCTL_BLOCK_SIZE
            return self.SEC_SIZE
        if op == 6:  # MP_BLOCKDEV_IOCTL_BLOCK_ERASE
            esp.flash_erase(arg + self.START_SEC)
            return 0

size = esp.flash_size()
if size < 1024*1024:
    bdev = None
else:
    # 20K at the flash end is reserved for SDK params storage
    bdev = FlashBdev((size - 20480) // FlashBdev.SEC_SIZE - FlashBdev.START_SEC)
