# Test VfsFat class and its finaliser

try:
    import errno, os, vfs

    vfs.VfsFat
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
        if op == 4:  # MP_BLOCKDEV_IOCTL_BLOCK_COUNT
            return len(self.data) // self.sec_size
        if op == 5:  # MP_BLOCKDEV_IOCTL_BLOCK_SIZE
            return self.sec_size


# Create block device, and skip test if not enough RAM
try:
    import errno, os

    bdev = RAMBlockDevice(50)
except MemoryError:
    print("SKIP")
    raise SystemExit

# Format block device and create VFS object
vfs.VfsFat.mkfs(bdev)
fs = vfs.VfsFat(bdev)

# Here we test that opening a file with the heap locked fails correctly.  This
# is a special case because file objects use a finaliser and allocating with a
# finaliser is a different path to normal allocation.  It would be better to
# test this in the core tests but there are no core objects that use finaliser.
import micropython

micropython.heap_lock()
try:
    import errno, os

    fs.open("x", "r")
except MemoryError:
    print("MemoryError")
micropython.heap_unlock()

# Here we test that the finaliser is actually called during a garbage collection.
import gc

# Preallocate global variables, and list of filenames for the test (which may
# in turn allocate new qstrs and/or a new qstr pool).
f = None
n = None
names = ["x%d" % i for i in range(5)]

# Do a large number of single-block allocations to move the GC head forwards,
# ensuring that the files are allocated from never-before-used blocks and
# therefore couldn't possibly have any references to them left behind on
# the stack.
for i in range(1024):
    []

# Run the test: create files without closing them, run GC, then read back files.
# Only read back N-1 files because the last one may not be finalised due to
# references to it being left on the C stack.
for n in names:
    f = fs.open(n, "w")
    f.write(n)
    f = None  # release f without closing
gc.collect()  # should finalise at least the first N-1 files by closing them
for n in names[:-1]:
    with fs.open(n, "r") as f:
        print(f.read())
