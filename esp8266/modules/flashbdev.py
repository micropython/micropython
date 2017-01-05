import esp

class FlashBdev:

    SEC_SIZE = 4096
    RESERVED_SECS = 1
    START_SEC = esp.flash_user_start() // SEC_SIZE + RESERVED_SECS
    NUM_BLK = 0x6b - RESERVED_SECS

    def __init__(self, blocks=NUM_BLK):
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

def set_bl_flash_size(real_size):
    if real_size == 256*1024:
        code = 1
    elif real_size == 512*1024:
        code = 0
    elif real_size == 1024*1024:
        code = 2
    elif real_size == 2048*1024:
        code = 3
    elif real_size == 4096*1024:
        code = 4
    else:
        code = 2
    buf = bytearray(4096)
    esp.flash_read(0, buf)
    buf[3] = (buf[3] & 0xf) | (code << 4)
    esp.flash_erase(0)
    esp.flash_write(0, buf)

size = esp.flash_size()
if size < 1024*1024:
    bdev = None
else:
    # 20K at the flash end is reserved for SDK params storage
    bdev = FlashBdev((size - 20480) // FlashBdev.SEC_SIZE - FlashBdev.START_SEC)
