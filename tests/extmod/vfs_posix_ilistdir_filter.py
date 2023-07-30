# Test ilistdir filter of . and .. for VfsPosix.

try:
    import os

    os.VfsPosix
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


def test(testdir):
    curdir = os.getcwd()
    vfs = os.VfsPosix(testdir)
    # When VfsPosix is used the intended way via os.mount(), it can only be called
    # with relative paths when the CWD is inside or at its root, so simulate that.
    os.chdir(testdir)

    dirs = [".a", "..a", "...a", "a.b", "a..b"]

    for dir in dirs:
        vfs.mkdir(dir)

    dirs = []
    for entry in vfs.ilistdir("/"):
        dirs.append(entry[0])
    dirs.sort()

    print(dirs)

    # Done with vfs, restore CWD.
    os.chdir(curdir)


# We need an empty directory for testing.
# Skip the test if it already exists.
temp_dir = "vfs_posix_ilistdir_filter_test_dir"
try:
    os.stat(temp_dir)
    print("SKIP")
    raise SystemExit
except OSError:
    pass

os.mkdir(temp_dir)

try:
    test(temp_dir)
finally:
    # Remove tempdir.
    for td in os.listdir(temp_dir):
        os.rmdir("/".join((temp_dir, td)))

    os.rmdir(temp_dir)
