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
vfs = uos.VfsFat(bdev, "/ramdisk")

# file IO
f = vfs.open("foo_file.txt", "w")
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
    vfs.open("foo_file.txt", "x")
except OSError as e:
    print(e.args[0] == uerrno.EEXIST)

with vfs.open("foo_file.txt", "a") as f:
    f.write("world!")

with vfs.open("foo_file.txt") as f2:
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

# dirs
vfs.mkdir("foo_dir")

try:
    vfs.rmdir("foo_file.txt")
except OSError as e:
    print(e.args[0] == 20) # uerrno.ENOTDIR

try:
    vfs.mkdir("foo_dir")
except OSError as e:
    print(e.args[0] == uerrno.EEXIST)

try:
    vfs.remove("foo_dir")
except OSError as e:
    print(e.args[0] == uerrno.EISDIR)

try:
    vfs.remove("no_file.txt")
except OSError as e:
    print(e.args[0] == uerrno.ENOENT)

try:
    vfs.rename("foo_dir", "/null")
except OSError as e:
    print(e.args[0] == uerrno.ENODEV)

# file in dir
with vfs.open("foo_dir/file-in-dir.txt", "w+t") as f:
    f.write("data in file")

with vfs.open("foo_dir/file-in-dir.txt", "r+b") as f:
    print(f.read())

with vfs.open("foo_dir/sub_file.txt", "w") as f:
    f.write("subdir file")

# directory not empty
try:
    vfs.rmdir("foo_dir")
except OSError as e:
    print(e.args[0] == uerrno.EACCES)

# trim full path
vfs.rename("foo_dir/file-in-dir.txt", "/ramdisk/foo_dir/file.txt")
print(vfs.listdir("foo_dir"))

vfs.rename("foo_dir/file.txt", "moved-to-root.txt")
print(vfs.listdir())

# valid removes
vfs.remove("foo_dir/sub_file.txt")
vfs.remove("foo_file.txt")
vfs.rmdir("foo_dir")
print(vfs.listdir())

# disk full
try:
    bsize = vfs.statvfs("/ramdisk")[0]
    free = vfs.statvfs("/ramdisk")[2] + 1
    f = vfs.open("large_file.txt", "wb")
    f.write(bytearray(bsize * free))
except OSError as e:
    print("ENOSPC:", e.args[0] == 28) # uerrno.ENOSPC
