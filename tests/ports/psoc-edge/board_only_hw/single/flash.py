import os
import psoc_edge

# Try to mount the filesystem, and format the flash if it doesn't exist.
# Create block device object for QSPI flash
bdev = psoc_edge.QSPI_Flash()

# Unmount the filesystem in case it's already mounted
try:
    os.umount("/")
except:
    pass

test_string = "This is a test string."
long_test_string = "This is a very long string. And as a long string that it is, it is only getting longer and longer and the string goes. How long shall it be? Well, not really sure, but let´s try it like this."

# Test LFS2 filesystem
if "VfsLfs2" in dir(os):
    # sector size 4 KB for external QSPI flash
    read_size = 0x1000
    # page size 256 B for QSPI flash
    write_size = 0x100

    # Create a LFS2 fs and mount it, else format and mount it
    try:
        vfs = os.VfsLfs2(bdev, progsize=write_size, readsize=read_size)
        os.mount(vfs, "/")
    except:
        os.VfsLfs2.mkfs(bdev, progsize=write_size, readsize=read_size)
        vfs = os.VfsLfs2(bdev, progsize=write_size, readsize=read_size)
        os.mount(vfs, "/")

    # Open a file and do some operation
    f = open("/test_lfs2.txt", "w")
    f.write(test_string)
    f.close()

    # Read back the contents
    f = open("/test_lfs2.txt", "r")
    if f.read() == test_string:
        print("Test successful")
    f.close()

    # Open a file and do some operation
    f = open("/test_lfs2_2.txt", "w")
    f.write(long_test_string)
    f.close()

    # Read back the contents
    f = open("/test_lfs2_2.txt", "r")
    if f.read() == long_test_string:
        print("Test successful")
    f.close()
