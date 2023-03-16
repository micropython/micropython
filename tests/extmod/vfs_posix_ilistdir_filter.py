# Test ilistdir filter of . and .. for VfsPosix.

try:
    import os

    os.VfsPosix
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


def test(testdir):
    vfs = os.VfsPosix(testdir)

    expected = { ".a" : True,
            "..a" : True,
            "...a" : True,
            "a." : True }
    actual = { }

    for dir in expected:
        vfs.mkdir(dir)

    for entry in vfs.ilistdir("/"):
        actual[entry[0]] = True

    assert expected == actual


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

test(temp_dir)

# Remove tempdir.
for td in os.listdir(temp_dir):
    os.rmdir("/".join((temp_dir, td)))

os.rmdir(temp_dir)
