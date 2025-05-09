# Test for VfsPosix

try:
    import gc, os, vfs

    vfs.VfsPosix
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# We need a directory for testing that doesn't already exist.
# Skip the test if it does exist.
temp_dir = "micropy_test_dir"
try:
    os.stat(temp_dir)
    print("SKIP")
    raise SystemExit
except OSError:
    pass

# getcwd and chdir
curdir = os.getcwd()
os.chdir("/")
print(os.getcwd())
os.chdir(curdir)
print(os.getcwd() == curdir)

# stat
print(type(os.stat("/")))

# listdir and ilistdir
target = "/"
try:
    import platform

    # On Android non-root users are permitted full filesystem access only to
    # selected directories.  To let this test pass on bionic, the internal
    # user-accessible storage area root is enumerated instead of the
    # filesystem root.  "/storage/emulated/0" should be there on pretty much
    # any recent-ish device; querying the proper location requires a JNI
    # round-trip, not really worth it.
    if platform.platform().startswith("Android-"):
        target = "/storage/emulated/0"
except ImportError:
    pass
print(type(os.listdir(target)))

# mkdir
os.mkdir(temp_dir)

# file create
f = open(temp_dir + "/test", "w")
f.write("hello")
f.close()

# close on a closed file should succeed
f.close()

# construct a file object with a raw fileno
f = open(2)
print(f)

# file read
f = open(temp_dir + "/test", "r")
print(f.read())
f.close()

# file finaliser, also see vfs_fat_finaliser.py
names = [temp_dir + "/x%d" % i for i in range(4)]
basefd = temp_dir + "/nextfd1"
nextfd = temp_dir + "/nextfd2"

with open(basefd, "w") as f:
    base_file_no = f.fileno()

for i in range(1024):  # move GC head forwards by allocating a lot of single blocks
    []


def write_files_without_closing():
    for n in names:
        open(n, "w").write(n)
    sorted(list(range(128)), key=lambda x: x)  # use up Python and C stack so f is really gone


write_files_without_closing()
gc.collect()

with open(nextfd, "w") as f:
    next_file_no = f.fileno()
    print("next_file_no <= base_file_no", next_file_no <= base_file_no)

for n in names + [basefd, nextfd]:
    os.remove(n)

# rename
os.rename(temp_dir + "/test", temp_dir + "/test2")
print(os.listdir(temp_dir))

# construct new VfsPosix with path argument
fs = vfs.VfsPosix(temp_dir)
# when VfsPosix is used the intended way via vfs.mount(), it can only be called
# with relative paths when the CWD is inside or at its root, so simulate that
os.chdir(temp_dir)
print(list(i[0] for i in fs.ilistdir(".")))

# stat, statvfs (statvfs may not exist)
print(type(fs.stat(".")))
if hasattr(fs, "statvfs"):
    assert type(fs.statvfs(".")) is tuple

# check types of ilistdir with str/bytes arguments
print(type(list(fs.ilistdir("."))[0][0]))
print(type(list(fs.ilistdir(b"."))[0][0]))

# chdir should not affect absolute paths (regression test)
fs.mkdir("/subdir")
fs.mkdir("/subdir/micropy_test_dir")
with fs.open("/subdir/micropy_test_dir/test2", "w") as f:
    f.write("wrong")
fs.chdir("/subdir")
with fs.open("/test2", "r") as f:
    print(f.read())
os.chdir(curdir)
fs.remove("/subdir/micropy_test_dir/test2")
fs.rmdir("/subdir/micropy_test_dir")
fs.rmdir("/subdir")

# done with fs, restore CWD
os.chdir(curdir)

# remove
os.remove(temp_dir + "/test2")
print(os.listdir(temp_dir))

# remove with error
try:
    import os

    os.remove(temp_dir + "/test2")
except OSError:
    print("remove OSError")

# rmdir
os.rmdir(temp_dir)
print(temp_dir in os.listdir())

# rmdir with error
try:
    import os

    os.rmdir(temp_dir)
except OSError:
    print("rmdir OSError")
