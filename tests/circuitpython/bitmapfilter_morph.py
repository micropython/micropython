from displayio import Bitmap
import bitmapfilter
from dump_bitmap import dump_bitmap


def make_circle_bitmap():
    b = Bitmap(17, 17, 65535)
    for i in range(b.height):
        y = i - 8
        for j in range(b.width):
            x = j - 8
            c = (x * x + y * y) > 64
            b[i, j] = 0xFFFF if c else 0
    return b


def make_quadrant_bitmap():
    b = Bitmap(17, 17, 1)
    for i in range(b.height):
        for j in range(b.width):
            b[i, j] = (i < 8) ^ (j < 8)
    return b


blur = (1, 2, 1, 2, 4, 2, 1, 2, 1)
sharpen = [-1, -2, -1, -2, 4, -2, -1, -2, -1]
b = make_circle_bitmap()
dump_bitmap(b)
bitmapfilter.morph(b, weights=blur)
dump_bitmap(b)

b = make_circle_bitmap()
q = make_quadrant_bitmap()
dump_bitmap(q)
bitmapfilter.morph(b, mask=q, weights=blur, add=1 / 4)
dump_bitmap(b)

# This is a kind of edge filter
b = make_circle_bitmap()
bitmapfilter.morph(b, weights=sharpen, threshold=True, add=0.125, invert=True)
dump_bitmap(b)
