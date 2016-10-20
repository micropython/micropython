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

print(b"FOO_FILETXT" not in bdev.data)
print(b"hello!" not in bdev.data)

vfs = uos.VfsFat(bdev, "/ramdisk")
print("statvfs:", vfs.statvfs("/ramdisk"))

print("getcwd:", vfs.getcwd())

f = vfs.open("foo_file.txt", "w")
f.write("hello!")
f.close()

f2 = vfs.open("foo_file.txt")
print(f2.read())
f2.close()

print(b"FOO_FILETXT" in bdev.data)
print(b"hello!" in bdev.data)

print(vfs.listdir())

try:
    vfs.rmdir("foo_file.txt")
except OSError as e:
    print(e.args[0] == 20) # uerrno.ENOTDIR

vfs.remove('foo_file.txt')
print(vfs.listdir())

vfs.mkdir("foo_dir")
print(vfs.listdir())

try:
    vfs.remove("foo_dir")
except OSError as e:
    print(e.args[0] == uerrno.EISDIR)

f = vfs.open("foo_dir/file-in-dir.txt", "w")
f.write("data in file")
f.close()

print(vfs.listdir("foo_dir"))

vfs.rename("foo_dir/file-in-dir.txt", "moved-to-root.txt")
print(vfs.listdir())

vfs.chdir("foo_dir")
print("getcwd:", vfs.getcwd())
print(vfs.listdir())

with vfs.open("sub_file.txt", "w") as f:
    f.write("test2")
print(vfs.listdir())

try:
    vfs.chdir("sub_file.txt")
except OSError as e:
    print(e.args[0] == uerrno.ENOENT)

vfs.chdir("..")
print("getcwd:", vfs.getcwd())

try:
    vfs.rmdir("foo_dir")
except OSError as e:
    print(e.args[0] == uerrno.EACCES)

vfs.remove("foo_dir/sub_file.txt")
vfs.rmdir("foo_dir")
print(vfs.listdir())

vfs.umount()
try:
    vfs.listdir()
except OSError as e:
    print(e.args[0] == uerrno.ENODEV)

vfs = uos.VfsFat(bdev, "/ramdisk")
print(vfs.listdir())
