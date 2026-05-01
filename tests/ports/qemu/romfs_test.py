try:
    import binascii, os

    binascii.crc32
    os.listdir
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

TEST_FILES = [
    "0x30d83fe5.bin",
    "0x37bef0eb.bin",
    "0x442f3b5f.bin",
    "0x648793fb.bin",
    "0x913837b6.bin",
    "0xdb14aac7.bin",
]


def is_test_partition():
    # Make sure a ROMFS partition exists first and that it is the one used for
    # CI tests, otherwise skip the test.

    try:
        romfs_files = sorted(os.listdir("/rom"))
    except OSError as e:
        if "ENODEV" in str(e):
            return False, None
        raise OSError(e)

    # os.path.exists isn't available so we have to be a bit more creative.
    if "romfs_sentinel.txt" not in romfs_files:
        return False, None

    try:
        with open("/rom/romfs_sentinel.txt", "rt") as s:
            data = s.read(128)
            return data.startswith("*MPY-ROMFS-TEST-PARTITION*\n"), romfs_files
    except OSError as e:
        if "ENOENT" in str(e):
            return False, None
        raise OSError(e)


valid_partition, romfs_files = is_test_partition()
if not valid_partition:
    print("SKIP")
    raise SystemExit

# The last entry is the CI romfs partition tag.

print(TEST_FILES == romfs_files[:-1])
for f in TEST_FILES:
    with open("/rom/" + f, "rb") as h:
        print(hex(binascii.crc32(h.read())) == f[:-4])
