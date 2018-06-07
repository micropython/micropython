# Test for VfsLittle using a RAM device

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

uos.VfsLittle.mkfs(bdev)

vfs = uos.VfsLittle(bdev)

print(vfs.statvfs('/'))

f = vfs.open('test', 'w')
f.write('littlefs')
f.close()
f.close()

print(list(vfs.ilistdir()))
vfs.mkdir('testdir')
print(list(vfs.ilistdir()))
vfs.rmdir('testdir')
print(list(vfs.ilistdir()))

with vfs.open('test', 'r') as f:
    print(f.read())

print(vfs.statvfs('/'))

with vfs.open('testbig', 'w') as f:
    data = 'large012' * 32 * 16
    print('data length:', len(data))
    for i in range(4):
        print('write', i)
        f.write(data)

print(vfs.statvfs('/'))
