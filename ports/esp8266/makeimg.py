import sys
import struct
import hashlib

# This region at the start of flash contains a small header and then segments
# containing .text, .data and .rodata, and so must be large enough to hold all
# of this.  This data is loaded to the appropriate places in RAM by the ROM
# bootloader at boot.  After this in flash comes .irom0.text, which must begin
# on a flash erase-page boundary.
SEGS_MAX_SIZE = 0x9000

assert len(sys.argv) == 4

md5 = hashlib.md5()

with open(sys.argv[3], "wb") as fout:

    with open(sys.argv[1], "rb") as f:
        data_flash = f.read()
        fout.write(data_flash)
        # First 4 bytes include flash size, etc. which may be changed
        # by esptool.py, etc.
        md5.update(data_flash[4:])
        print("flash    ", len(data_flash))

        # Print info about segments in this first part of flash
        num_segs = struct.unpack_from("<BBBBI", data_flash, 0)[1]
        offset = 8
        for seg_num in range(num_segs):
            seg_name = [".text", ".data", ".rodata"][seg_num]
            seg_offset, seg_size = struct.unpack_from("<II", data_flash, offset)
            print(" {:7}  {} at 0x{:x}".format(seg_name, seg_size, seg_offset))
            offset += 8 + seg_size

    with open(sys.argv[2], "rb") as f:
        data_rom = f.read()

    pad = b"\xff" * (SEGS_MAX_SIZE - len(data_flash))
    assert len(pad) >= 4
    fout.write(pad[:-4])
    md5.update(pad[:-4])
    len_data = struct.pack("I", SEGS_MAX_SIZE + len(data_rom))
    fout.write(len_data)
    md5.update(len_data)
    print("padding  ", len(pad))

    fout.write(data_rom)
    md5.update(data_rom)
    print("irom0text", len(data_rom))

    fout.write(md5.digest())

    print("total    ", SEGS_MAX_SIZE + len(data_rom))
    print("md5      ", md5.hexdigest())
