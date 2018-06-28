# Test for VfsLittle using a RAM device, testing error handling

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

# mkfs
uos.VfsLittle.mkfs(bdev)

# construction
vfs = uos.VfsLittle(bdev)
with open('testfile', 'w') as f:
    f.write('test')
vfs.mkdir('testdir')

# ilistdir
try:
    vfs.ilistdir('noexist')
except OSError:
    print('ilistdir OSError')

# remove
try:
    vfs.remove('noexist')
except OSError:
    print('remove OSError')

# rmdir
try:
    vfs.rmdir('noexist')
except OSError:
    print('rmdir OSError')

# rename
try:
    vfs.rename('noexist', 'somethingelse')
except OSError:
    print('rename OSError')

# mkdir
try:
    vfs.mkdir('testdir')
except OSError:
    print('mkdir OSError')

# chdir to nonexistent
try:
    vfs.chdir('noexist')
except OSError:
    print('chdir OSError')

# chdir to file
try:
    vfs.chdir('testfile')
except OSError:
    print('chdir OSError')

# stat
try:
    vfs.stat('noexist')
except OSError:
    print('stat OSError')
