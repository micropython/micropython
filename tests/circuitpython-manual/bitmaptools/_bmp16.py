import ulab.numpy as np
import displayio
import bitmaptools

import struct

base_header = b"BMFX\x02\x00\x00\x00\x00\x00F\x00\x00\x008\x00\x00\x00@\x01\x00\x00\xf0\x00\x00\x00\x01\x00\x10\x00\x03\x00\x00\x00\x00X\x02\x00\xd7\r\x00\x00\xd7\r\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xf8\x00\x00\xe0\x07\x00\x00\x1f\x00\x00\x00\x00\x00\x00\x00"


def writebmp16(filename, bitmap):
    header = bytearray(base_header)
    header[18:26] = struct.pack("<II", bitmap.width, bitmap.height)
    with open(filename, "wb") as f:
        f.write(header)
        b = np.frombuffer(bitmap, dtype=np.uint16)
        for i in range(bitmap.height):
            j = (bitmap.height - i - 1) * bitmap.width
            f.write(b[j : j + bitmap.width])


def loadbmp16(filename, width=320, height=240):
    """This specialized routine loads 16bpp uncompressed bmp files with a
    70-byte header. It is not appropriate for generic bmp files."""

    bitmap = displayio.Bitmap(width, height, 65536)
    with open(filename, "rb") as f:
        f.seek(70)
        bitmaptools.readinto(
            bitmap,
            f,
            bits_per_pixel=16,
            element_size=2,
            reverse_rows=True,
        )

    return bitmap


if __name__ == "__main__":
    if "/" in __file__:
        here = __file__.rsplit("/", 1)[0]
    else:
        here = "."
    b = loadbmp16(here + "/minerva16.bmp")
    print(b[0, 0])
    print(b[160, 160])
    for i, p in enumerate(sorted(set(memoryview(b)))):
        print("%04x" % p, end="\n" if (i % 8) == 7 else " ")
    if i % 8 != 7:
        print()
