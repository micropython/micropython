import sys

SEGS_MAX_SIZE = 0x9000

assert len(sys.argv) == 4

with open(sys.argv[3], 'wb') as fout:

    with open(sys.argv[1], 'rb') as f:
        data_flash = f.read()
        fout.write(data_flash)
        print('flash    ', len(data_flash))

    pad = b'\xff' * (SEGS_MAX_SIZE - len(data_flash))
    fout.write(pad)
    print('padding  ', len(pad))

    with open(sys.argv[2], 'rb') as f:
        data_rom = f.read()
        fout.write(data_rom)
        print('irom0text', len(data_rom))

    print('total    ', SEGS_MAX_SIZE + len(data_rom))
