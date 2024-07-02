import machine
import os
import errno

SHORT_TEST_STRING = "This is a test string."
LONG_TEST_STRING = "This is a very long string. And as a long string that it is, it is only getting longer and longer and the string goes. How long shall it be? Well, not really sure, but let´s try it like this."

board = os.uname().machine
if "CY8CPROTO-063-BLE" in board:
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


def mount_or_format_sd_card(block_device, filesystem, mount_point, read_size, write_size):
    try:
        vfs = filesystem(block_device, progsize=write_size, readsize=read_size)
        os.mount(vfs, mount_point)
    except OSError:
        filesystem.mkfs(block_device, progsize=write_size, readsize=read_size)
        vfs = filesystem(block_device, progsize=write_size, readsize=read_size)
        os.mount(vfs, mount_point)
    print(f"SD card mounted at {mount_point}\n")


def read_write_test(file_path, test_data):
    with open(file_path, "w") as f:
        f.write(test_data)
    with open(file_path, "r") as f:
        return f.read() == test_data


def test_file_transfer():
    # Define the SD card configuration
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
    bdev = machine.SDCard(**sdcard_config)

    # Define constants
    MOUNT_POINT = "/SDCard"
    READ_SIZE = 512
    WRITE_SIZE = 512

    # Unmount the SD card if mounted
    unmount_sd_card(MOUNT_POINT)

    # Mount or format the SD card with LFS2 filesystem
    if "VfsLfs2" in dir(os):
        mount_or_format_sd_card(bdev, os.VfsLfs2, MOUNT_POINT, READ_SIZE, WRITE_SIZE)

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


def test_reintializing_same_slot():
    print("\n***** Test 2: reinitialize the same slot more than once *****\n")

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
    bdev1 = machine.SDCard(**sdcard_config)
    bdev2 = machine.SDCard(**sdcard_config)
    bdev3 = machine.SDCard(**sdcard_config)
    print("PASS")


def test_negative_slot_number():
    print("\n***** Test 3: slot number exceeding the number of available slots *****\n")

    try:
        sdcard_config = {
            "slot": 2,
            "width": 4,
            "cd": "P13_5",
            "cmd": "P12_4",
            "clk": "P12_5",
            "dat0": "P13_0",
            "dat1": "P13_1",
            "dat2": "P13_2",
            "dat3": "P13_3",
        }
        bdev = machine.SDCard(**sdcard_config)
    except Exception:
        print("FAIL")


if __name__ == "__main__":
    test_file_transfer()
    test_reintializing_same_slot()
    test_negative_slot_number()
