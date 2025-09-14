# Test for VfsPosix with relative paths

try:
    import os, vfs

    vfs.VfsPosix
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

# We need a directory for testing that doesn't already exist.
# Skip the test if it does exist.
temp_dir = "vfs_posix_paths_test_dir"
try:
    os.stat(temp_dir)
    print("SKIP")
    raise SystemExit
except OSError:
    pass

curdir = os.getcwd()
os.mkdir(temp_dir)

# construct new VfsPosix with absolute path argument
temp_dir_abs = os.getcwd() + os.sep + temp_dir
fs = vfs.VfsPosix(temp_dir_abs)
# when VfsPosix is used the intended way via vfs.mount(), it can only be called
# with relative paths when the CWD is inside or at its root, so simulate that
os.chdir(temp_dir_abs)
fs.mkdir("subdir")
fs.mkdir("subdir/one")
print('listdir("/"):', sorted(i[0] for i in fs.ilistdir("/")))
print('listdir("."):', sorted(i[0] for i in fs.ilistdir(".")))
print('getcwd() in ("", "/"):', fs.getcwd() in ("", "/"))
print('chdir("subdir"):', fs.chdir("subdir"))
print("getcwd():", fs.getcwd())
print('mkdir("two"):', fs.mkdir("two"))
f = fs.open("file.py", "w")
f.write("print('hello')")
f.close()
print('listdir("/"):', sorted(i[0] for i in fs.ilistdir("/")))
print('listdir("/subdir"):', sorted(i[0] for i in fs.ilistdir("/subdir")))
print('listdir("."):', sorted(i[0] for i in fs.ilistdir(".")))
try:
    f = fs.open("/subdir/file.py", "r")
    print(f.read())
    f.close()
except Exception as e:
    print(e)
import sys

sys.path.insert(0, "")
try:
    import file

    print(file)
except Exception as e:
    print(e)
del sys.path[0]
fs.remove("file.py")
fs.rmdir("two")
fs.rmdir("/subdir/one")
fs.chdir("/")
fs.rmdir("/subdir")

# done with fs, restore CWD
os.chdir(curdir)

# some integration tests with a mounted VFS
vfs.mount(vfs.VfsPosix(temp_dir_abs), "/mnt")
os.mkdir("/mnt/dir")
print('chdir("/mnt/dir"):', os.chdir("/mnt/dir"))
print("getcwd():", os.getcwd())
print('chdir("/mnt"):', os.chdir("/mnt"))
print("getcwd():", os.getcwd())
print('chdir("/"):', os.chdir("/"))
print("getcwd():", os.getcwd())
print('chdir("/mnt/dir"):', os.chdir("/mnt/dir"))
print("getcwd():", os.getcwd())
print('chdir(".."):', os.chdir(".."))
print("getcwd():", os.getcwd())
os.rmdir("/mnt/dir")
vfs.umount("/mnt")

# restore CWD
os.chdir(curdir)

# rmdir
os.rmdir(temp_dir)
print(temp_dir in os.listdir())
