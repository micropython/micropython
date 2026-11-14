# MIT license; Copyright (c) 2026 OpenMV LLC.
#
# Test writing ROMFS partitions through vfs.rom_ioctl, which exercises the MRAM
# and OSPI flash drivers.
#
# Only the last 64k of each partition is written, and its original contents are
# restored at the end, so existing ROMFS images are left intact (unless they
# extend into that area).

import vfs

try:
    import uctypes
except ImportError:
    print("SKIP")
    raise SystemExit

if not hasattr(vfs, "rom_ioctl"):
    print("SKIP")
    raise SystemExit

TEST_LEN = 64 * 1024


def rom_write(rom_id, offset, data):
    min_write = vfs.rom_ioctl(3, rom_id, offset, len(data))
    if min_write < 0:
        raise OSError(min_write)
    chunk = 4096
    for i in range(0, len(data), chunk):
        ret = vfs.rom_ioctl(4, rom_id, offset + i, data[i : i + chunk])
        if ret < 0:
            raise OSError(ret)
    vfs.rom_ioctl(5, rom_id)
    return min_write


for rom_id in range(vfs.rom_ioctl(1)):
    seg = vfs.rom_ioctl(2, rom_id)
    offset = len(seg) - TEST_LEN
    # Large partitions can't be sliced at this offset, so access the test area
    # of the memory-mapped segment by address.
    area = uctypes.bytearray_at(uctypes.addressof(seg) + offset, TEST_LEN)
    saved = bytes(area)

    # Write a pattern and read it back through the memory-mapped segment.
    pattern = bytes((i * 7 + rom_id) & 0xFF for i in range(TEST_LEN))
    min_write = rom_write(rom_id, offset, pattern)
    ok_pattern = area == pattern

    # Write a second, different pattern over the first one.
    pattern = bytes(0xFF - b for b in pattern)
    rom_write(rom_id, offset, pattern)
    ok_rewrite = area == pattern

    # Restore the original contents.
    rom_write(rom_id, offset, saved)
    ok_restore = area == saved

    print("romfs", rom_id, min_write in (4, 16), ok_pattern, ok_rewrite, ok_restore)
