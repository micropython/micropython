# Test for VfsPosix

try:
    import gc, os, vfs, errno

    vfs.VfsPosix
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# We need a directory for testing that doesn't already exist.
# Skip the test if it does exist.
temp_dir = "vfs_posix_readonly_test_dir"
try:
    os.stat(temp_dir)
    raise SystemExit("Target directory {} exists".format(temp_dir))
except OSError:
    pass

# mkdir (skip test if whole filesystem is readonly)
try:
    os.mkdir(temp_dir)
except OSError as e:
    if e.errno == errno.EROFS:
        print("SKIP")
        raise SystemExit

fs_factory = lambda: vfs.VfsPosix(temp_dir)

# mount
fs = fs_factory()
vfs.mount(fs, "/vfs")

with open("/vfs/file", "w") as f:
    f.write("content")

# test reading works
with open("/vfs/file") as f:
    print("file:", f.read())

os.mkdir("/vfs/emptydir")

# umount
vfs.umount("/vfs")

# mount read-only
fs = fs_factory()
vfs.mount(fs, "/vfs", readonly=True)

# test reading works
with open("/vfs/file") as f:
    print("file 2:", f.read())

# test writing fails
try:
    with open("/vfs/test_write", "w"):
        pass
    print("opened")
except OSError as er:
    print(repr(er))

# test removing fails
try:
    os.unlink("/vfs/file")
    print("unlinked")
except OSError as er:
    print(repr(er))

# test renaming fails
try:
    os.rename("/vfs/file2", "/vfs/renamed")
    print("renamed")
except OSError as er:
    print(repr(er))

# test removing directory fails
try:
    os.rmdir("/vfs/emptydir")
    print("rmdir'd")
except OSError as er:
    print(repr(er))

# test creating directory fails
try:
    os.mkdir("/vfs/emptydir2")
    print("mkdir'd")
except OSError as er:
    print(repr(er))

# umount
vfs.umount("/vfs")

fs = fs_factory()
vfs.mount(fs, "/vfs")

os.rmdir("/vfs/emptydir")
os.unlink("/vfs/file")

os.rmdir(temp_dir)
