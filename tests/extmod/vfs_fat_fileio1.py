try:
    import uerrno
    try:
        import uos_vfs as uos
        open = uos.vfs_open
    except ImportError:
        import uos
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    uos.VfsFat
except AttributeError:
    print("SKIP")
    raise SystemExit


class RAMFS:

    SEC_SIZE = 512

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.SEC_SIZE)

    def readblocks(self, n, buf):
        #print("readblocks(%s, %x(%d))" % (n, id(buf), len(buf)))
        for i in range(len(buf)):
            buf[i] = self.data[n * self.SEC_SIZE + i]

    def writeblocks(self, n, buf):
        #print("writeblocks(%s, %x)" % (n, id(buf)))
        for i in range(len(buf)):
            self.data[n * self.SEC_SIZE + i] = buf[i]

    def ioctl(self, op, arg):
        #print("ioctl(%d, %r)" % (op, arg))
        if op == 4:  # BP_IOCTL_SEC_COUNT
            return len(self.data) // self.SEC_SIZE
        if op == 5:  # BP_IOCTL_SEC_SIZE
            return self.SEC_SIZE


try:
    bdev = RAMFS(50)
except MemoryError:
    print("SKIP")
    raise SystemExit

uos.VfsFat.mkfs(bdev)
vfs = uos.VfsFat(bdev)
uos.mount(vfs, '/ramdisk')
uos.chdir('/ramdisk')

# file IO
f = open("foo_file.txt", "w")
print(str(f)[:17], str(f)[-1:])
f.write("hello!")
f.flush()
f.close()
f.close() # allowed
try:
    f.write("world!")
except OSError as e:
    print(e.args[0] == uerrno.EINVAL)

try:
    f.read()
except OSError as e:
    print(e.args[0] == uerrno.EINVAL)

try:
    f.flush()
except OSError as e:
    print(e.args[0] == uerrno.EINVAL)

try:
    open("foo_file.txt", "x")
except OSError as e:
    print(e.args[0] == uerrno.EEXIST)

with open("foo_file.txt", "a") as f:
    f.write("world!")

with open("foo_file.txt") as f2:
    print(f2.read())
    print(f2.tell())

    f2.seek(0, 0) # SEEK_SET
    print(f2.read(1))

    f2.seek(0, 1) # SEEK_CUR
    print(f2.read(1))
    f2.seek(2, 1) # SEEK_CUR
    print(f2.read(1))

    f2.seek(-2, 2) # SEEK_END
    print(f2.read(1))

# using constructor of FileIO type to open a file
# no longer working with new VFS sub-system
#FileIO = type(f)
#with FileIO("/ramdisk/foo_file.txt") as f:
#    print(f.read())

# dirs
vfs.mkdir("foo_dir")

try:
    vfs.rmdir("foo_file.txt")
except OSError as e:
    print(e.args[0] == 20) # uerrno.ENOTDIR

vfs.remove("foo_file.txt")
print(list(vfs.ilistdir()))

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
