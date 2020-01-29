# Test VfsFat class and its finaliser

try:
    import uerrno, uos
    uos.VfsFat
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class RAMBlockDevice:
    def __init__(self, blocks, sec_size=512):
        self.sec_size = sec_size
        self.data = bytearray(blocks * self.sec_size)

    def readblocks(self, n, buf):
        for i in range(len(buf)):
            buf[i] = self.data[n * self.sec_size + i]

    def writeblocks(self, n, buf):
        for i in range(len(buf)):
            self.data[n * self.sec_size + i] = buf[i]

    def ioctl(self, op, arg):
        if op == 4: # MP_BLOCKDEV_IOCTL_BLOCK_COUNT
            return len(self.data) // self.sec_size
        if op == 5: # MP_BLOCKDEV_IOCTL_BLOCK_SIZE
            return self.sec_size


# Create block device, and skip test if not enough RAM
try:
    bdev = RAMBlockDevice(50)
except MemoryError:
    print("SKIP")
    raise SystemExit

# Format block device and create VFS object
uos.VfsFat.mkfs(bdev)
vfs = uos.VfsFat(bdev)

# Here we test that opening a file with the heap locked fails correctly.  This
# is a special case because file objects use a finaliser and allocating with a
# finaliser is a different path to normal allocation.  It would be better to
# test this in the core tests but there are no core objects that use finaliser.
import micropython
micropython.heap_lock()
try:
    vfs.open('x', 'r')
except MemoryError:
    print('MemoryError')
micropython.heap_unlock()

# Here we test that the finaliser is actually called during a garbage collection.
import gc
N = 4
for i in range(N):
    n = 'x%d' % i
    f = vfs.open(n, 'w')
    f.write(n)
    f = None # release f without closing
    [0, 1, 2, 3] # use up Python stack so f is really gone
gc.collect() # should finalise all N files by closing them
for i in range(N):
    with vfs.open('x%d' % i, 'r') as f:
        print(f.read())
