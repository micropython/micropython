import uerrno
try:
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
    bdev2 = RAMFS(50)
except MemoryError:
    print("SKIP")
    raise SystemExit

# first we umount any existing mount points the target may have
try:
    uos.umount('/')
except OSError:
    pass
for path in uos.listdir('/'):
    uos.umount('/' + path)

uos.VfsFat.mkfs(bdev)
uos.mount(bdev, '/')

print(uos.getcwd())

f = open('test.txt', 'w')
f.write('hello')
f.close()

print(uos.listdir())
print(uos.listdir('/'))
print(uos.stat('')[:-3])
print(uos.stat('/')[:-3])
print(uos.stat('test.txt')[:-3])
print(uos.stat('/test.txt')[:-3])

f = open('/test.txt')
print(f.read())
f.close()

uos.rename('test.txt', 'test2.txt')
print(uos.listdir())
uos.rename('test2.txt', '/test3.txt')
print(uos.listdir())
uos.rename('/test3.txt', 'test4.txt')
print(uos.listdir())
uos.rename('/test4.txt', '/test5.txt')
print(uos.listdir())

uos.mkdir('dir')
print(uos.listdir())
uos.mkdir('/dir2')
print(uos.listdir())
uos.mkdir('dir/subdir')
print(uos.listdir('dir'))
for exist in ('', '/', 'dir', '/dir', 'dir/subdir'):
    try:
        uos.mkdir(exist)
    except OSError as er:
        print('mkdir OSError', er.args[0] == 17) # EEXIST

uos.chdir('/')
print(uos.stat('test5.txt')[:-3])

uos.VfsFat.mkfs(bdev2)
uos.mount(bdev2, '/sys')
print(uos.listdir())
print(uos.listdir('sys'))
print(uos.listdir('/sys'))

uos.rmdir('dir2')
uos.remove('test5.txt')
print(uos.listdir())

uos.umount('/')
print(uos.getcwd())
print(uos.listdir())
print(uos.listdir('sys'))

# test importing a file from a mounted FS
import sys
sys.path.clear()
sys.path.append('/sys')
with open('sys/test_module.py', 'w') as f:
    f.write('print("test_module!")')
import test_module
