import sys
import struct
import hashlib

SEGS_MAX_SIZE = 0x9000

assert len(sys.argv) == 4

md5 = hashlib.md5()

with open(sys.argv[3], 'wb') as fout:

    with open(sys.argv[1], 'rb') as f:
        data_flash = f.read()
        fout.write(data_flash)
        # First 4 bytes include flash size, etc. which may be changed
        # by esptool.py, etc.
        md5.update(data_flash[4:])
        print('flash    ', len(data_flash))

    with open(sys.argv[2], 'rb') as f:
        data_rom = f.read()

    pad = b'\xff' * (SEGS_MAX_SIZE - len(data_flash))
    assert len(pad) >= 4
    fout.write(pad[:-4])
    md5.update(pad[:-4])
    len_data = struct.pack("I", SEGS_MAX_SIZE + len(data_rom))
    fout.write(len_data)
    md5.update(len_data)
    print('padding  ', len(pad))

    fout.write(data_rom)
    md5.update(data_rom)
    print('irom0text', len(data_rom))

    fout.write(md5.digest())

    print('total    ', SEGS_MAX_SIZE + len(data_rom))
    print('md5      ', md5.hexdigest())
