import sys
try:
    import uerrno
    try:
        import uos_vfs as uos
        open = uos.vfs_open
    except ImportError:
        import uos
except ImportError:
    print("SKIP")
    sys.exit()

try:
    uos.VfsFat
except AttributeError:
    print("SKIP")
    sys.exit()


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
    sys.exit()

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
    try:
        f2.seek(1, 1) # SEEK_END
    except OSError as e:
        print(e.args[0] == uerrno.EOPNOTSUPP)

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
