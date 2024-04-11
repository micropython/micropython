# Test for VfsPosix error conditions

try:
    import os, sys, vfs

    vfs.VfsPosix
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

if sys.platform == "win32":
    # Windows doesn't let you delete the current directory, so this cannot be
    # tested.
    print("SKIP")
    raise SystemExit

# We need an empty directory for testing.
# Skip the test if it already exists.
temp_dir = "vfs_posix_enoent_test_dir"
try:
    os.stat(temp_dir)
    print("SKIP")
    raise SystemExit
except OSError:
    pass

curdir = os.getcwd()
os.mkdir(temp_dir)
os.chdir(temp_dir)
os.rmdir(curdir + "/" + temp_dir)
try:
    print("getcwd():", os.getcwd())
except OSError as e:
    # expecting ENOENT = 2
    print("getcwd():", repr(e))

try:
    print("VfsPosix():", vfs.VfsPosix("something"))
except OSError as e:
    # expecting ENOENT = 2
    print("VfsPosix():", repr(e))
