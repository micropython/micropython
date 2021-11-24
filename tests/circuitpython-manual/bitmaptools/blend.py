import bitmaptools
import displayio
import _bmp16

if "/" in __file__:
    here = __file__.rsplit("/", 1)[0]
else:
    here = "."

c = displayio.Colorspace.BGR565

b1 = _bmp16.loadbmp16(here + "/minerva16.bmp")
b2 = _bmp16.loadbmp16(here + "/blinka16.bmp")
b3 = displayio.Bitmap(320, 240, 65536)

for i in (
    0,
    1 / 64,
    3 / 64,
    3 / 32,
    3 / 16,
    0.5,
    1 - 3 / 16,
    1 - 3 / 32,
    1 - 3 / 64,
    1 - 1 / 64,
    1,
):
    bitmaptools.alphablend(b3, b1, b2, c, i)
    _bmp16.writebmp16(f"blend-{i:.2f}.bmp", b3)
    bitmaptools.alphablend(b3, b1, b2, c, i, 0)
    _bmp16.writebmp16(f"fade-{i:.2f}.bmp", b3)
