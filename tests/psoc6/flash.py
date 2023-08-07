import os, psoc6

# Try to mount the filesystem, and format the flash if it doesn't exist.
# create block device object based on whichever flash is active
bdev = psoc6.QSPI_Flash() if "QSPI_Flash" in dir(psoc6) else psoc6.Flash()
# unmount the filesystem in case its already mounted
os.umount("/")

test_string = "This is a test string."
long_test_string = "This is a very long string. And as a long string that it is, it is only getting longer and longer and the string goes. How long shall it be? Well, not really sure, but let´s try it like this."
print(test_string)

# first priority is always LFS2 filesystem as it is the default
if "VfsLfs2" in dir(os):
    # sector size 4 KB for external flash
    # sector size 512 B for internal flash
    read_size = 0x1000 if "QSPI_Flash" in dir(psoc6) else 0x200
    # page size 512 B for both flashes
    write_size = 0x200

    # create a LFS2 fs and mount it, else format and mount it
    try:
        vfs = os.VfsLfs2(bdev, progsize=write_size, readsize=read_size)
        os.mount(vfs, "/flash")
    except:
        os.VfsLfs2.mkfs(bdev, progsize=write_size, readsize=read_size)
        vfs = os.VfsLfs2(bdev, progsize=write_size, readsize=read_size)
        os.mount(vfs, "/flash")

    # open a file and do some operation
    f = open("/flash/test_lfs2.txt", "w")
    f.write(test_string)
    f.close()

    # read back the contents
    f = open("/flash/test_lfs2.txt", "r")
    if f.read() == test_string:
        print("Test successful")
    f.close()

    # open a file and do some operation
    f = open("/flash/test_lfs2_2.txt", "w")
    f.write(long_test_string)
    f.close()

    # read back the contents
    f = open("/flash/test_lfs2_2.txt", "r")
    if f.read() == long_test_string:
        print("Test successful")
    f.close()

if "VfsFat" in dir(os):
    # create a FAT fs and mount it, else format and mount it
    try:
        vfs = os.VfsFat(bdev)
        os.mount(vfs, "/flash")
    except:
        os.VfsFat.mkfs(bdev)
        vfs = os.VfsFat(bdev)
        os.mount(vfs, "/flash")

    # open a file and do some operation
    f = open("/flash/test_fat.txt", "w")
    f.write(test_string)
    f.close()

    # read back the contents
    f = open("/flash/test_fat.txt", "r")
    if f.read() == test_string:
        print("Test successful")
    f.close()

    # open a file and do some operation
    f = open("/flash/test_fat_2.txt", "w")
    f.write(long_test_string)
    f.close()

    # read back the contents
    f = open("/flash/test_fat_2.txt", "r")
    if f.read() == long_test_string:
        print("Test successful")
    f.close()
