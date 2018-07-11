import esp

class FlashBdev:

    SEC_SIZE = 4096
    START_SEC = esp.flash_user_start() // SEC_SIZE

    def __init__(self, blocks):
        self.blocks = blocks

    def readblocks(self, n, buf):
        #print("readblocks(%s, %x(%d))" % (n, id(buf), len(buf)))
        esp.flash_read((n + self.START_SEC) * self.SEC_SIZE, buf)

    def writeblocks(self, n, buf):
        #print("writeblocks(%s, %x(%d))" % (n, id(buf), len(buf)))
        #assert len(buf) <= self.SEC_SIZE, len(buf)
        esp.flash_erase(n + self.START_SEC)
        esp.flash_write((n + self.START_SEC) * self.SEC_SIZE, buf)

    def ioctl(self, op, arg):
        #print("ioctl(%d, %r)" % (op, arg))
        if op == 4:  # BP_IOCTL_SEC_COUNT
            return self.blocks
        if op == 5:  # BP_IOCTL_SEC_SIZE
            return self.SEC_SIZE

size = esp.flash_size()
if size < 1024*1024:
    # flash too small for a filesystem
    bdev = None
else:
    # for now we use a fixed size for the filesystem
    bdev = FlashBdev(2048 * 1024 // FlashBdev.SEC_SIZE)
