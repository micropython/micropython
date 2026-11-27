# Test ilistdir filter of . and .. for VfsPosix.

try:
    import os, vfs

    vfs.VfsPosix
    os.listdir
    os.rmdir
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


def test(testdir):
    curdir = os.getcwd()
    fs = vfs.VfsPosix(testdir)
    # When VfsPosix is used the intended way via vfs.mount(), it can only be called
    # with relative paths when the CWD is inside or at its root, so simulate that.
    os.chdir(testdir)

    dirs = [".a", "..a", "...a", "a.b", "a..b"]

    for dir in dirs:
        fs.mkdir(dir)

    dirs = []
    for entry in fs.ilistdir("/"):
        dirs.append(entry[0])
    dirs.sort()

    print(dirs)

    # Done with fs, restore CWD.
    os.chdir(curdir)


temp_dir = "vfs_posix_ilistdir_filter_test_dir"
try:
    # We need an empty directory for testing. Otherwise skip.
    for td in os.listdir(temp_dir):
        os.rmdir("/".join((temp_dir, td)))

    os.rmdir(temp_dir)
except OSError:
    print("SKIP")
    raise SystemExit

os.mkdir(temp_dir)
test(temp_dir)
