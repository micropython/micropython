# Test for VfsLittle using a RAM device, with mount/umount

try:
    import uos
    uos.VfsLittle
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

class RAMFS:
    ERASE_BLOCK_SIZE = 1024

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.ERASE_BLOCK_SIZE)

    def readblocks(self, block, off, buf):
        #print("readblocks(%d, %d, %x(%d), %d)" % (block, off, id(buf), len(buf), off + len(buf)))
        addr = block * self.ERASE_BLOCK_SIZE + off
        for i in range(len(buf)):
            buf[i] = self.data[addr + i]

    def writeblocks(self, block, off, buf):
        #print("writeblocks(%d, %d, %x(%d), %d)" % (block, off, id(buf), len(buf), off + len(buf)))
        addr = block * self.ERASE_BLOCK_SIZE + off
        for i in range(len(buf)):
            self.data[addr + i] = buf[i]

    def ioctl(self, op, arg):
        #print("ioctl(%d, %r)" % (op, arg))
        if op == 0: # sync
            return 0
        if op == 1: # erase block size
            return self.ERASE_BLOCK_SIZE
        if op == 2: # erase block count
            return len(self.data) // self.ERASE_BLOCK_SIZE
        if op == 3: # read/write size
            return 128
        if op == 4: # erase a block
            return 0

bdev = RAMFS(30)

# initialise path
import sys
sys.path.clear()
sys.path.append('/lfs')

# mkfs
uos.VfsLittle.mkfs(bdev)

# construction
vfs = uos.VfsLittle(bdev)

# mount
uos.mount(vfs, '/lfs')

# import
with open('/lfs/lfsmod.py', 'w') as f:
    f.write('print("hello from lfs")\n')
import lfsmod

# import package
uos.mkdir('/lfs/lfspkg')
with open('/lfs/lfspkg/__init__.py', 'w') as f:
    f.write('print("package")\n')
import lfspkg

# umount
uos.umount('/lfs')
