import machine
import os
import errno

SHORT_TEST_STRING = "This is a test string."
LONG_TEST_STRING = "This is a very long string. And as a long string that it is, it is only getting longer and longer and the string goes. How long shall it be? Well, not really sure, but let´s try it like this."
READ_SIZE = 512
WRITE_SIZE = 512
MOUNT_POINT_FAT = "/SDCardFat"
MOUNT_POINT_LFS2 = "/SDCardLfs2"

board = os.uname().machine
if "CY8CPROTO-062-4343W" in board:
    sdcard_config = {
        "width": 4,
        "cd": "P13_5",
        "cmd": "P12_4",
        "clk": "P12_5",
        "dat0": "P13_0",
        "dat1": "P13_1",
        "dat2": "P13_2",
        "dat3": "P13_3",
    }
elif "CY8CPROTO-063-BLE" in board:
    print("SKIP")
    raise SystemExit
elif "CY8CKIT-062S2-AI" in board:
    print("SKIP")
    raise SystemExit


def unmount_sd_card(path):
    try:
        os.umount(path)
    except OSError as e:
        if e.args[0] != errno.EINVAL:
            raise Exception(f"Could not unmount {path}")


def mount_or_format_sd_card_vfs2(block_device, filesystem, mount_point):
    try:
        vfs = filesystem(block_device, progsize=WRITE_SIZE, readsize=READ_SIZE)
        os.mount(vfs, mount_point)
    except OSError:
        filesystem.mkfs(block_device, progsize=WRITE_SIZE, readsize=READ_SIZE)
        vfs = filesystem(block_device, progsize=WRITE_SIZE, readsize=READ_SIZE)
        os.mount(vfs, mount_point)
    print(f"\nSD card mounted at {mount_point}\n")


def mount_or_format_sd_card_fat(block_device, filesystem, mount_point):
    try:
        vfs = filesystem(block_device)
        os.mount(vfs, mount_point)
    except OSError:
        filesystem.mkfs(block_device)
        vfs = filesystem(block_device)
        os.mount(vfs, mount_point)
    print(f"\nSD card mounted at {mount_point}\n")


def read_write_test(file_path, test_data):
    with open(file_path, "w") as f:
        f.write(test_data)
    with open(file_path, "r") as f:
        return f.read() == test_data


def test_file_transfer_lfs2():
    # Define the SD card configuration
    bdev = machine.SDCard(**sdcard_config)

    # Unmount the SD card if mounted
    unmount_sd_card(MOUNT_POINT_LFS2)

    # Mount or format the SD card with LFS2 filesystem
    if "VfsLfs2" in dir(os):
        mount_or_format_sd_card_vfs2(bdev, os.VfsLfs2, MOUNT_POINT_LFS2)

        # change to SD card directory
        os.chdir(MOUNT_POINT_LFS2)

        print("\n***** Test 1: Short string file transfer to SD Card in LFS2 format *****\n")
        # Test short string
        short_test_file = "test_sd_lfs2_short.txt"
        if read_write_test(short_test_file, SHORT_TEST_STRING):
            print("PASS")
        else:
            print("FAIL")

        print("\n***** Test 2: Long string file transfer to SD Card in LFS2 format *****\n")
        # Test long string
        long_test_file = "test_sd_lfs2_long.txt"
        if read_write_test(long_test_file, LONG_TEST_STRING):
            print("PASS")
        else:
            print("FAIL")

        # change to root directory
        os.chdir("/")

    bdev.deinit()


def test_file_transfer_fat():
    # Define the SD card configuration
    bdev = machine.SDCard(**sdcard_config)

    # Unmount the SD card if mounted
    unmount_sd_card(MOUNT_POINT_FAT)

    # Mount or format the SD card with LFS2 filesystem
    if "VfsLfs2" in dir(os):
        mount_or_format_sd_card_fat(bdev, os.VfsFat, MOUNT_POINT_FAT)

        # change to SD card directory
        os.chdir(MOUNT_POINT_FAT)

        print("\n***** Test 3: Short string file transfer to SD Card in FAT format *****\n")
        # Test short string
        short_test_file = "test_sd_fat_short.txt"
        if read_write_test(short_test_file, SHORT_TEST_STRING):
            print("PASS")
        else:
            print("FAIL")

        print("\n***** Test 4: Long string file transfer to SD Card in FAT format *****\n")
        # Test long string
        long_test_file = "test_sd_fat_long.txt"
        if read_write_test(long_test_file, LONG_TEST_STRING):
            print("PASS")
        else:
            print("FAIL")

        # change to root directory
        os.chdir("/")

    bdev.deinit()


def test_reintializing_same_slot():
    print("\n***** Test 5: reinitialize the same slot more than once *****\n")
    bdev1 = machine.SDCard(**sdcard_config)
    bdev1.deinit()
    bdev2 = machine.SDCard(**sdcard_config)
    bdev2.deinit()
    print("PASS")


if __name__ == "__main__":
    test_file_transfer_lfs2()
    test_file_transfer_fat()
    test_reintializing_same_slot()
