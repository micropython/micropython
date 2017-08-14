import sys
import uos
import uerrno
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
    bdev = RAMFS(48)
except MemoryError:
    print("SKIP")
    sys.exit()

uos.VfsFat.mkfs(bdev)

assert b"FOO_FILETXT" not in bdev.data
assert b"hello!" not in bdev.data

vfs = uos.VfsFat(bdev, "/ramdisk")

print("getcwd:", vfs.getcwd())

f = vfs.open("foo_file.txt", "w")
f.write("hello!")
f.close()

f2 = vfs.open("foo_file.txt")
print(f2.read())
f2.close()

assert b"FOO_FILETXT" in bdev.data
assert b"hello!" in bdev.data

assert vfs.listdir() == ['foo_file.txt']

vfs.remove('foo_file.txt')
assert vfs.listdir() == []

vfs.mkdir("foo_dir")
assert vfs.listdir() == ['foo_dir']
f = vfs.open("foo_dir/file-in-dir.txt", "w")
f.write("data in file")
f.close()

assert vfs.listdir("foo_dir") == ['file-in-dir.txt']

vfs.rename("foo_dir/file-in-dir.txt", "moved-to-root.txt")
assert vfs.listdir() == ['foo_dir', 'moved-to-root.txt']

vfs.chdir("foo_dir")
print("getcwd:", vfs.getcwd())
assert vfs.listdir() == []

with vfs.open("sub_file.txt", "w") as f:
    f.write("test2")
assert vfs.listdir() == ["sub_file.txt"]

vfs.chdir("..")
print("getcwd:", vfs.getcwd())


vfs.umount()
try:
    vfs.listdir()
except OSError as e:
    assert e.args[0] == uerrno.ENODEV
else:
    raise AssertionError("expected OSError not thrown")

vfs = uos.VfsFat(bdev, "/ramdisk")
assert  vfs.listdir() == ['foo_dir', 'moved-to-root.txt']
