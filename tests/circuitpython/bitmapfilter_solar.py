from displayio import Bitmap
import bitmapfilter
import ulab
from dump_bitmap import dump_bitmap_rgb_swapped
from blinka_image import decode_resource


def test_pattern():
    return decode_resource("testpattern", 2)


def make_quadrant_bitmap():
    b = Bitmap(17, 17, 1)
    for i in range(b.height):
        for j in range(b.width):
            b[i, j] = (i < 8) ^ (j < 8)
    return b


q = make_quadrant_bitmap()
b = test_pattern()
dump_bitmap_rgb_swapped(b)

print("solarize (masked)")
bitmapfilter.solarize(b, mask=q)
dump_bitmap_rgb_swapped(b)
