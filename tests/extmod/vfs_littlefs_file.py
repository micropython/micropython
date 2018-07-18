# Test for VfsLittle using a RAM device, file IO

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

# create text, print, write, close
f = vfs.open('test.txt', 'wt')
print(f)
f.write('littlefs')
f.close()

# close already-closed file
f.close()

# create binary, print, write, flush, close
f = vfs.open('test.bin', 'wb')
print(f)
f.write('littlefs')
f.flush()
f.close()

# create for append
f = vfs.open('test.bin', 'ab')
f.write('more')
f.close()

# create exclusive
f = vfs.open('test2.bin', 'xb')
f.close()

# create exclusive with error
try:
    vfs.open('test2.bin', 'x')
except OSError:
    print('open OSError')

# read default
with vfs.open('test.txt', '') as f:
    print(f.read())

# read text
with vfs.open('test.txt', 'rt') as f:
    print(f.read())

# read binary
with vfs.open('test.bin', 'rb') as f:
    print(f.read())

# create read and write
with vfs.open('test.bin', 'r+b') as f:
    print(f.read(8))
    f.write('MORE')
with vfs.open('test.bin', 'rb') as f:
    print(f.read())

# seek and tell
f = vfs.open('test.txt', 'r')
print(f.tell())
f.seek(3, 0)
print(f.tell())
f.close()

# open nonexistent
try:
    vfs.open('noexist', 'r')
except OSError:
    print('open OSError')
