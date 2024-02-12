try:
    import os, vfs

    vfs.VfsFat
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class RAMFS:
    SEC_SIZE = 512

    def __init__(self, blocks):
        self.data = bytearray(blocks * self.SEC_SIZE)

    def readblocks(self, n, buf):
        # print("readblocks(%s, %x(%d))" % (n, id(buf), len(buf)))
        for i in range(len(buf)):
            buf[i] = self.data[n * self.SEC_SIZE + i]

    def writeblocks(self, n, buf):
        # print("writeblocks(%s, %x)" % (n, id(buf)))
        for i in range(len(buf)):
            self.data[n * self.SEC_SIZE + i] = buf[i]

    def ioctl(self, op, arg):
        # print("ioctl(%d, %r)" % (op, arg))
        if op == 4:  # MP_BLOCKDEV_IOCTL_BLOCK_COUNT
            return len(self.data) // self.SEC_SIZE
        if op == 5:  # MP_BLOCKDEV_IOCTL_BLOCK_SIZE
            return self.SEC_SIZE


try:
    bdev = RAMFS(50)
    bdev2 = RAMFS(50)
except MemoryError:
    print("SKIP")
    raise SystemExit

# first we umount any existing mount points the target may have
try:
    vfs.umount("/")
except OSError:
    pass
for path in os.listdir("/"):
    vfs.umount("/" + path)

vfs.VfsFat.mkfs(bdev)
vfs.mount(bdev, "/")

print(os.getcwd())

f = open("test.txt", "w")
f.write("hello")
f.close()

print(os.listdir())
print(os.listdir("/"))
print(os.stat("")[:-3])
print(os.stat("/")[:-3])
print(os.stat("test.txt")[:-3])
print(os.stat("/test.txt")[:-3])

f = open("/test.txt")
print(f.read())
f.close()

os.rename("test.txt", "test2.txt")
print(os.listdir())
os.rename("test2.txt", "/test3.txt")
print(os.listdir())
os.rename("/test3.txt", "test4.txt")
print(os.listdir())
os.rename("/test4.txt", "/test5.txt")
print(os.listdir())

os.mkdir("dir")
print(os.listdir())
os.mkdir("/dir2")
print(os.listdir())
os.mkdir("dir/subdir")
print(os.listdir("dir"))
for exist in ("", "/", "dir", "/dir", "dir/subdir"):
    try:
        os.mkdir(exist)
    except OSError as er:
        print("mkdir OSError", er.errno == 17)  # EEXIST

os.chdir("/")
print(os.stat("test5.txt")[:-3])

vfs.VfsFat.mkfs(bdev2)
vfs.mount(bdev2, "/sys")
print(os.listdir())
print(os.listdir("sys"))
print(os.listdir("/sys"))

os.rmdir("dir2")
os.remove("test5.txt")
print(os.listdir())

vfs.umount("/")
print(os.getcwd())
print(os.listdir())
print(os.listdir("sys"))

# test importing a file from a mounted FS
import sys

sys.path.clear()
sys.path.append("/sys")
with open("sys/test_module.py", "w") as f:
    f.write('print("test_module!")')
import test_module
