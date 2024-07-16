import machine
import os
import errno
import vfs
import time

# Define constants
MOUNT_POINT = "/sd"
SHORT_TEST_STRING = "This is a short string"
LONG_TEST_STRING = "This is a very long string. And as a long string that it is, it is only getting longer and longer and the string goes. How long shall it be? Well, not really sure, but let´s try it like this."
READ_SIZE = 512
WRITE_SIZE = 512

board = os.uname().machine
if "CY8CPROTO-062-4343W" in board:
    sdcard_config = {
        "slot": 1,
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


def mount_or_format_sd_card(block_device, filesystem, mount_point):
    try:
        print("create vfs")
        vfs = filesystem(block_device)
        print("mount sd card")
        os.mount(vfs, mount_point)
    except OSError:
        print("try formatting")
        filesystem.mkfs(block_device)
        print("create fs")
        vfs = filesystem(block_device)
        print("mount sd card")
        os.mount(vfs, mount_point)
    print(f"\nSD card mounted at {mount_point}\n")


def mount_or_format_sd_card_lfs2(block_device, filesystem, mount_point):
    print("try formatting")
    filesystem.mkfs(block_device, progsize=READ_SIZE, readsize=WRITE_SIZE)
    print("create fs")
    vfs = filesystem(block_device, progsize=READ_SIZE, readsize=WRITE_SIZE)
    print("mount sd card")
    os.mount(vfs, mount_point)
    print(f"\nSD card mounted at {mount_point}\n")


def mount_or_format_sd_card_fat(block_device, filesystem, mount_point):
    print("create fs")
    vfs = filesystem(block_device)
    print("mount sd card")
    os.mount(vfs, mount_point)
    print(f"\nSD card mounted at {mount_point}\n")


def read_write_test(file_name, test_data):
    os.chdir("/sd")
    print(os.listdir())
    file = open("tst.txt", encoding="utf-8", mode="w+")
    file.write(test_data)
    file.close()

    time.sleep(2)

    file = open("tst.txt", encoding="utf-8", mode="r+")
    read_data = file.read()
    file.close()
    os.chdir("/")
    print(read_data)
    return read_data == test_data


def test_lfs2_file_transfer():
    bdev = machine.SDCard(**sdcard_config)

    # Unmount the SD card if mounted
    unmount_sd_card(MOUNT_POINT)

    if "VfsLfs2" in dir(os):
        mount_or_format_sd_card_fat(bdev, os.VfsFat, MOUNT_POINT)

        print("\n***** Test 1: Short string file transfer to SD Card in LFS2 format *****\n")
        # Test short string
        short_test_file = "/SDCard/test_sd_lfs2_short.txt"
        if read_write_test(short_test_file, SHORT_TEST_STRING):
            print("PASS")
        else:
            print("FAIL")

        print("\n***** Test 2: Long string file transfer to SD Card in LFS2 format *****\n")
        # Test long string
        long_test_file = "/SDCard/test_sd_lfs2_long.txt"
        if read_write_test(long_test_file, LONG_TEST_STRING):
            print("PASS")
        else:
            print("FAIL")

    bdev.deinit()


def test_fat_file_transfer():
    bdev = machine.SDCard(**sdcard_config)
    print(bdev)

    # Unmount the SD card if mounted
    unmount_sd_card(MOUNT_POINT)

    # Mount or format the SD card with LFS2 filesystem
    if "VfsFat" in dir(os):
        mount_or_format_sd_card(bdev, os.VfsFat, MOUNT_POINT)

        print("\n***** Test 1: Short string file transfer to SD Card in FAT format *****\n")
        # Test short string
        short_test_file = "short_file.txt"
        if read_write_test(short_test_file, SHORT_TEST_STRING):
            print("PASS")
        else:
            print("FAIL")

        print("\n***** Test 2: Long string file transfer to SD Card in FAT format *****\n")
        # Test short string
        long_test_file = "long_file.txt"
        if read_write_test(long_test_file, LONG_TEST_STRING):
            print("PASS")
        else:
            print("FAIL")

        os.chdir("/sd")

        # create a new file
        f = open("test.txt", "w")
        test_bytes = bytearray(b"\x02\x03\x05\x07")
        n_w = f.write(test_bytes)
        print(n_w == len(test_bytes))
        f.close()
        f = open("test.txt", "a+")
        r = bytes(f.read(), "ascii")
        # check that we can write and read it correctly
        print(r == test_bytes)
        f.close()

        os.chdir("/")

    bdev.deinit()


def test_reintializing_same_slot():
    print("\n***** Test 3: reinitialize the same slot more than once *****\n")
    bdev1 = machine.SDCard(**sdcard_config)
    bdev1.deinit()
    bdev2 = machine.SDCard(**sdcard_config)
    bdev2.deinit()
    print("PASS")


if __name__ == "__main__":
    test_lfs2_file_transfer()
    # test_fat_file_transfer()
    # test_reintializing_same_slot()
