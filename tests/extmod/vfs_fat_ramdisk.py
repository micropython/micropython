import sys
import uos
import uerrno
try:
    uos.VfsFat
except AttributeError:
    print("SKIP")
    sys.exit()

from common.ramfs import RAMFS_IOCTL_RW

try:
    bdev = RAMFS_IOCTL_RW(48)
except MemoryError:
    print("SKIP")
    sys.exit()

uos.VfsFat.mkfs(bdev)

print(b"FOO_FILETXT" not in bdev.data)
print(b"hello!" not in bdev.data)

vfs = uos.VfsFat(bdev, "/ramdisk")

try:
    vfs.statvfs("/null")
except OSError as e:
    print(e.args[0] == uerrno.ENODEV)

print("statvfs:", vfs.statvfs("/ramdisk"))
print("getcwd:", vfs.getcwd())

try:
    vfs.stat("no_file.txt")
except OSError as e:
    print(e.args[0] == uerrno.ENOENT)

with vfs.open("foo_file.txt", "w") as f:
    f.write("hello!")
print(vfs.listdir())

print("stat root:", vfs.stat("/"))
print("stat disk:", vfs.stat("/ramdisk/"))
print("stat file:", vfs.stat("foo_file.txt"))

print(b"FOO_FILETXT" in bdev.data)
print(b"hello!" in bdev.data)

vfs.mkdir("foo_dir")
vfs.chdir("foo_dir")
print("getcwd:", vfs.getcwd())
print(vfs.listdir())

with vfs.open("sub_file.txt", "w") as f:
    f.write("subdir file")

try:
    vfs.chdir("sub_file.txt")
except OSError as e:
    print(e.args[0] == uerrno.ENOENT)

vfs.chdir("..")
print("getcwd:", vfs.getcwd())

vfs.umount()
try:
    vfs.listdir()
except OSError as e:
    print(e.args[0] == uerrno.ENODEV)

try:
    vfs.getcwd()
except OSError as e:
    print(e.args[0] == uerrno.ENODEV)

vfs = uos.VfsFat(bdev, "/ramdisk")
print(vfs.listdir(b""))
