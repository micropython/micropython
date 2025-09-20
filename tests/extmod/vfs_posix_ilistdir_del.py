# Test ilistdir __del__ for VfsPosix.

try:
    import gc, os, vfs

    vfs.VfsPosix
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


def test(testdir):
    curdir = os.getcwd()
    fs = vfs.VfsPosix(testdir)
    # When VfsPosix is used the intended way via vfs.mount(), it can only be called
    # with relative paths when the CWD is inside or at its root, so simulate that.
    # (Although perhaps calling with a relative path was an oversight in this case
    # and the respective line below was meant to read `fs.rmdir("/" + dname)`.)
    os.chdir(testdir)
    fs.mkdir("/test_d1")
    fs.mkdir("/test_d2")
    fs.mkdir("/test_d3")

    for i in range(10):
        print(i)

        # We want to partially iterate the ilistdir iterator to leave it in an
        # open state, which will then test the finaliser when it's garbage collected.
        idir = fs.ilistdir("/")
        print(any(idir))

        # Alternate way of partially iterating the ilistdir object, modifying the
        # filesystem while it's open.
        for dname, *_ in fs.ilistdir("/"):
            fs.rmdir(dname)
            break
        fs.mkdir(dname)

        # Also create a fully drained iterator and ensure trying to reuse it
        # throws the correct exception.
        idir_emptied = fs.ilistdir("/")
        l = list(idir_emptied)
        print(len(l))
        try:
            next(idir_emptied)
        except StopIteration:
            pass

        gc.collect()

        # Create and delete a file, try to flush out any filesystem
        # corruption that may be caused over the loops.
        fs.open("/test", "w").close()
        fs.remove("/test")

    # Done with fs, restore CWD.
    os.chdir(curdir)


# We need an empty directory for testing.
# Skip the test if it already exists.
temp_dir = "vfs_posix_ilistdir_del_test_dir"
try:
    os.stat(temp_dir)
    print("SKIP")
    raise SystemExit
except OSError:
    pass

os.mkdir(temp_dir)

test(temp_dir)

# Remove tempdir.
for td in os.listdir(temp_dir):
    os.rmdir("/".join((temp_dir, td)))

os.rmdir(temp_dir)
