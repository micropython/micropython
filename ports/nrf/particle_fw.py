import sys, struct, zlib

BASE = 0x00030000

with open(sys.argv[1], 'rb') as f:
    data = bytearray(f.read())
struct.pack_into('<IIBBHHBBII', data, 0x200, BASE, BASE + len(data), 0, 0, 0x138, 0x0e, 0x03, 0, 0, 0)
crc = zlib.crc32(data)
print('CRC: {:08x}'.format(crc))
data += struct.pack('>I', crc)
with open(sys.argv[1], 'wb') as f:
    f.write(data)
